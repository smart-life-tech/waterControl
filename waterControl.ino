#include <Arduino.h>

// Pins for switches
const int SWITCH_1_PIN = 2;
const int SWITCH_2_PIN = 3;
const int SWITCH_3_PIN = 4;

// Pins for relays
const int RELAY_1_PIN = 5; // Relay to turn right
const int RELAY_2_PIN = 6; // Relay to turn left

// Pin for the flashing lamp
const int LAMP_PIN = 7;

// Time durations (in milliseconds)
const unsigned long EXTEND_TIME = 5000;              // 17 seconds
const unsigned long SLIGHTLY_OPEN_TIME = 5000;       // 5 seconds
const unsigned long CLOSE_AND_REVERSE_TIME = 2000;   // 2 seconds
const unsigned long REVERSE_TIME = 7000;             // 20 seconds
const unsigned long OPEN_TIME_AFTER_REVERSE = 12000; // 000 20 minutes

const unsigned long BLINK_INTERVAL = 1000; // Blink interval in milliseconds
unsigned long previousMillis2 = 0;         // Holds the previous time the LED was updated

// Delay time before changing direction
const unsigned long DIRECTION_CHANGE_DELAY = 2000; // 2 seconds

// State variables
enum State
{
  IDLE,
  EXTENDING,
  SLIGHTLY_OPENING,
  CLOSING,
  REVERSING,
  OPEN_AFTER_REVERSE
};
void blinking()
{
  unsigned long currentMillis2 = millis();

  // Check if it's time to update the LED state
  if (currentMillis2 - previousMillis2 >= BLINK_INTERVAL)
  {
    // Save the current time for the next interval
    previousMillis2 = currentMillis2;

    // Toggle the LED state
    digitalWrite(LAMP_PIN, !digitalRead(LAMP_PIN));
  }
}
State currentState = REVERSING;
State lastState = IDLE;
unsigned long stateStartTime = 0;
bool run = true;
void setup()
{
  pinMode(SWITCH_1_PIN, INPUT_PULLUP);
  pinMode(SWITCH_2_PIN, INPUT_PULLUP);
  pinMode(SWITCH_3_PIN, INPUT_PULLUP);

  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);

  pinMode(LAMP_PIN, OUTPUT);
  Serial.begin(115200);
  stateStartTime = millis();
  lastState = REVERSING;
  currentState = REVERSING;
}

void loop()
{
  unsigned long currentMillis = millis();

  // Check water level switches
  bool switch1State = digitalRead(SWITCH_1_PIN);
  bool switch2State = digitalRead(SWITCH_2_PIN);
  bool switch3State = digitalRead(SWITCH_3_PIN);
  // if (!switch3State)blinking();
  if (!switch1State && !switch2State && !switch3State)
  {
    currentState = EXTENDING;
  }
  else if (switch1State && switch2State && !switch3State)
  {
    currentState = SLIGHTLY_OPENING;
  }
  else if (switch1State && !switch2State && !switch3State)
  {
    currentState = CLOSING;
  }
  else if (switch1State && switch2State && switch3State)
  {
    currentState = REVERSING;
  }
  if (currentState != lastState)
  {
    lastState = currentState;
    run = true;
    stateStartTime = millis();
  }
  // Check if both relays should be off to prevent overheating or burning out

  if (run)
  {
    switch (currentState)
    {
    case IDLE:
      Serial.println("idle");
      // Motor is not moving, check conditions to start a new movement
      if (!switch1State && !switch2State && !switch3State)
      {
        currentState = EXTENDING;
      }
      else if (switch1State && switch2State && !switch3State)
      {
        currentState = SLIGHTLY_OPENING;
      }
      else if (switch1State && !switch2State && !switch3State)
      {
        currentState = CLOSING;
      }
      else if (switch1State && switch2State && switch3State)
      {
        currentState = REVERSING;
        // digitalWrite(LAMP_PIN, HIGH); // Turn on the lamp
      }
      break;

    case EXTENDING:

      // Motor is extending
      if (currentMillis - stateStartTime >= EXTEND_TIME)
      {
        Serial.println("done extending");
        run = false;
        // stateStartTime = millis();
        currentState = IDLE;
      }
      else
      {
        // Extend the motor (turn right) if not already extended
        digitalWrite(RELAY_2_PIN, HIGH); // Turn off the relay to turn left
        if (currentMillis - stateStartTime >= DIRECTION_CHANGE_DELAY)
        {
          Serial.println("extending relay 1 activated");
          digitalWrite(RELAY_1_PIN, LOW); // Activate the relay to turn right
        }
      }
      break;

    case SLIGHTLY_OPENING:
      // Motor is slightly opening

      if (currentMillis - stateStartTime >= SLIGHTLY_OPEN_TIME)
      {
        Serial.println("slightly opening done");
        run = false;
        currentState = IDLE;
      }
      else
      {
        digitalWrite(RELAY_1_PIN, HIGH); // Turn off the relay to turn left
        if (currentMillis - stateStartTime >= DIRECTION_CHANGE_DELAY)
        {
          Serial.println("slightly open relay 2 ");
          digitalWrite(RELAY_2_PIN, LOW); // Activate the relay to tur
          // Implement the logic to slightly open the motor
          // You may need to control the motor speed here
          // For example, you can use PWM to control the motor speed
          // and adjust the PWM value to achieve a slight opening effect
          // For simplicity, this example does not implement the motor speed control
        }
        break;

      case CLOSING:
        // Motor is closing
        Serial.println("closing");
        if (currentMillis - stateStartTime >= CLOSE_AND_REVERSE_TIME)
        {
          currentState = IDLE;
          run = false;
        }
        else
        {
          // Close the motor (turn left) if not already closed
          digitalWrite(RELAY_1_PIN, HIGH); // Turn off the relay to turn right
          if (currentMillis - stateStartTime >= DIRECTION_CHANGE_DELAY)
          {
            digitalWrite(RELAY_2_PIN, LOW); // Activate the relay to turn left
          }
        }
        break;

      case REVERSING:
        // Motor is reversing

        if (currentMillis - stateStartTime >= REVERSE_TIME)
        {
          currentState = OPEN_AFTER_REVERSE;
          Serial.println("done reversing");
          digitalWrite(RELAY_1_PIN, HIGH);
          digitalWrite(RELAY_2_PIN, HIGH);
          run = false;
        }
        else
        {
          Serial.println("reversing");
          blinking();
          // Reverse the motor (turn left) if not already reversed
          digitalWrite(RELAY_1_PIN, HIGH); // Turn off the relay to turn right
          if (currentMillis - stateStartTime >= DIRECTION_CHANGE_DELAY)
          {
            digitalWrite(RELAY_2_PIN, LOW); // Activate the relay to turn left
          }
        }
        break;
      }
    }
    if (currentState == IDLE)
    {
      digitalWrite(RELAY_1_PIN, HIGH);
      digitalWrite(RELAY_2_PIN, HIGH);
    }
    if (currentState == OPEN_AFTER_REVERSE)
    {
      Serial.println("open after reverse");
      // Motor is open after reversing, keep it open for 20 minutes
      digitalWrite(RELAY_1_PIN, HIGH); // Turn off the relay to turn right
      digitalWrite(RELAY_2_PIN, HIGH); // Turn off the relay to turn left
      // digitalWrite(LAMP_PIN, LOW); // Turn off the lamp

      while (currentMillis - stateStartTime <= OPEN_TIME_AFTER_REVERSE)
      {
        run = false;
        currentState = IDLE;
      }
    }
  }
}
