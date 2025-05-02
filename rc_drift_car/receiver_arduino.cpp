#include <SPI.h>           // SPI communication library for NRF24L01
#include <nRF24L01.h>      // NRF24L01 definitions
#include <RF24.h>          // RF24 radio communication library
#include <Servo.h>         // Library to control servo motors

// Create RF24 object with CE on pin 9 and CSN on pin 10
RF24 radio(9, 10);

// Set communication address (must match the transmitter)
const byte address[6] = "00001";

// Create servo objects
Servo motor; // ESC (Electronic Speed Controller) for brushless motor
Servo steering; // Servo for steering

// Define the pins used for ESC and steering servo
const int motorPin = 6;      // ESC signal connected to pin D6
const int steeringPin = 5;   // Steering servo signal connected to pin D5

// Define structure to receive data
struct Data {
    int steering;  // Joystick X value (steering)
    int throttle;  // Joystick Y value (throttle)
};

void setup() {
    Serial.begin(9600);      // Start serial monitor for debugging
    radio.begin();             // Initialize the radio module
    radio.openReadingPipe(0, address); // Open reading pipe with the same address
    radio.setPALevel(RF24_PA_LOW);   // Set power level to low
    radio.startListening();             // Set radio module to receiver mode

    motor.attach(motorPin);          // Attach ESC to the specified pin
    steering.attach(steeringPin);    // Attach steering servo to the specified pin

    motor.writeMicroseconds(1000);   // Initialize ESC with minimum throttle
    delay(2000);               // Wait 2 seconds to arm the ESC
}

void loop() {
    if (radio.available()) {         // Check if data is received
        Data data;
        radio.read(&data, sizeof(data));  // Read the incoming data

        //  Map joystick steering value to servo angle (e.g., 40° to 125°)
        int steeringAngle = map(data.steering, 0, 1023, 40, 125);
        steering.write(steeringAngle);   // Set steering servo angle

        // Process throttle for ESC
        data.throttle = constrain(data.throttle, 513, 1024); // Limit to half-stick up
        int motorSpeed = map(data.throttle, 513, 1024, 1000, 2000); // Scale to ESC signal
        motor.writeMicroseconds(motorSpeed); // Send PWM to ESC

        // Print debug info
        Serial.print("Steering: "); Serial.print(steeringAngle);
        Serial.print(" | Throttle: "); Serial.println(motorSpeed);
    }
}
