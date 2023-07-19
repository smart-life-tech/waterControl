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
const unsigned long EXTEND_TIME = 17000;               // 17 seconds
const unsigned long SLIGHTLY_OPEN_TIME = 5000;         // 5 seconds
const unsigned long CLOSE_AND_REVERSE_TIME = 2000;     // 2 seconds
const unsigned long REVERSE_TIME = 20000;              // 20 seconds
const unsigned long OPEN_TIME_AFTER_REVERSE = 1200000; // 20 minutes

// Delay time before changing direction
const unsigned long DIRECTION_CHANGE_DELAY = 2000; // 2 seconds

void setup()
{
    pinMode(SWITCH_1_PIN, INPUT_PULLUP);
    pinMode(SWITCH_2_PIN, INPUT_PULLUP);
    pinMode(SWITCH_3_PIN, INPUT_PULLUP);

    pinMode(RELAY_1_PIN, OUTPUT);
    pinMode(RELAY_2_PIN, OUTPUT);

    pinMode(LAMP_PIN, OUTPUT);
}

void loop()
{
    // Check water level switches
    bool switch1State = digitalRead(SWITCH_1_PIN);
    bool switch2State = digitalRead(SWITCH_2_PIN);
    bool switch3State = digitalRead(SWITCH_3_PIN);

    // Turn off both relays to ensure they are never active at the same time
    digitalWrite(RELAY_1_PIN, LOW);
    digitalWrite(RELAY_2_PIN, LOW);

    // If all switches are open, extend the linear motor for 17 seconds
    if (!switch1State && !switch2State && !switch3State)
    {
        extendMotor(EXTEND_TIME);
    }

    // If switch 1 and switch 2 are closed, slightly open the motor for 5 seconds
    else if (switch1State && switch2State && !switch3State)
    {
        openMotor(SLIGHTLY_OPEN_TIME);
    }

    // If switch 1 was closed but switch 2 was open, close the motor for 2 seconds
    else if (switch1State && !switch2State && !switch3State)
    {
        closeMotor(CLOSE_AND_REVERSE_TIME);
    }

    // If all switches are closed, reverse the motor for 20 seconds and flash the lamp
    else if (switch1State && switch2State && switch3State)
    {
        reverseMotorWithLamp(REVERSE_TIME);
    }

    // Keep the motor open for 20 minutes after reversing, regardless of other switches
    delay(OPEN_TIME_AFTER_REVERSE);
}

// Function to extend the motor for a given time
void extendMotor(unsigned long duration)
{
    digitalWrite(RELAY_1_PIN, HIGH); // Activate the relay to turn right
    delay(DIRECTION_CHANGE_DELAY);
    digitalWrite(RELAY_2_PIN, LOW); // Turn off the relay to turn left
    delay(duration);
}

// Function to slightly open the motor for a given time
void openMotor(unsigned long duration)
{
    // Implement the logic to slightly open the motor
    // You may need to control the motor speed here
    // For example, you can use PWM to control the motor speed
    // and adjust the PWM value to achieve a slight opening effect
    // For simplicity, this example does not implement the motor speed control
    delay(duration);
}

// Function to close the motor for a given time
void closeMotor(unsigned long duration)
{
    digitalWrite(RELAY_2_PIN, HIGH); // Activate the relay to turn left
    delay(DIRECTION_CHANGE_DELAY);
    digitalWrite(RELAY_1_PIN, LOW); // Turn off the relay to turn right
    delay(duration);
}

// Function to reverse the motor for a given time and flash the lamp
void reverseMotorWithLamp(unsigned long duration)
{
    digitalWrite(LAMP_PIN, HIGH);    // Turn on the lamp
    digitalWrite(RELAY_2_PIN, HIGH); // Activate the relay to turn left
    delay(DIRECTION_CHANGE_DELAY);
    digitalWrite(RELAY_1_PIN, LOW); // Turn off the relay to turn right
    delay(duration);
    digitalWrite(LAMP_PIN, LOW); // Turn off the lamp
}
