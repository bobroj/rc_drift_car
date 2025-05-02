#include <SPI.h>           // SPI communication library (needed for NRF24L01)
#include <nRF24L01.h>      // Definitions for the nRF24L01 radio
#include <RF24.h>          // Main RF24 radio communication library

// Create a radio object with CE on pin 9 and CSN on pin 10
RF24 radio(9, 10);

// Unique address for communication (must match the receiver)
const byte address[6] = "00001";

// Structure to send two values together: steering (servo) and throttle (ESC)
struct Data {
    int steering;  // Steering joystick value
    int throttle;  // Throttle joystick value
};

// Define analog pins where joysticks are connected
#define STEERING_PIN A0  // Left joystick X-axis (steering)
#define THROTTLE_PIN A1  // Right joystick Y-axis (throttle)

void setup() {
    Serial.begin(9600);             // Start serial monitor for debugging
    radio.begin();                  // Initialize the radio module
    radio.openWritingPipe(address); // Open a writing pipe to the given address
    radio.setPALevel(RF24_PA_LOW);  // Set power level to low (reduces interference)
    radio.stopListening();          // Set the module as a transmitter
}

void loop() {
    Data data;  // Create a structure to store joystick values

    // Read analog values from the joysticks
    data.steering = analogRead(STEERING_PIN);   // Read left joystick (steering)
    data.throttle = analogRead(THROTTLE_PIN);   // Read right joystick (throttle)

    // Print values to serial monitor (optional for debugging)
    Serial.print("Steering: "); Serial.print(data.steering);
    Serial.print(" | Throttle: "); Serial.println(data.throttle);

    // Send the structure via radio
    radio.write(&data, sizeof(data));

    delay(100);  // Small delay between transmissions
}
