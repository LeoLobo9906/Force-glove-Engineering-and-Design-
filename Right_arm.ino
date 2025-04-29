// Include the RadioHead ASK (Amplitude Shift Keying) library for RF communication
#include <RH_ASK.h>

// Include SPI library (required by RH_ASK library)
#include <SPI.h>

// Include Wire library for I2C communication (used by LCD)
#include <Wire.h>

// Include the LiquidCrystal_I2C library to control the I2C LCD screen
#include <LiquidCrystal_I2C.h>

// Create an instance of the RF driver (for the RF transmitter module)
RH_ASK rf_driver;

// Create an instance of the LCD. Parameters: (I2C address, columns, rows)
// 0x27 is a common address for 16x2 LCDs
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the analog pin where the pressure sensor is connected
#define FORCE_SENSOR_PIN A0

// Define the minimum and maximum raw sensor values for calibration
const int sensorMin = 600;   // Lowest expected sensor reading
const int sensorMax = 8600;  // Highest expected sensor reading

// Define the maximum value to scale the pressure reading to
const int outputMax = 10000; // Maximum pressure reading after mapping

// The setup function runs once when the microcontroller starts
void setup() {
  rf_driver.init();    // Initialize the RF module
  lcd.init();          // Initialize the LCD screen
  lcd.backlight();     // Turn on the LCD backlight for visibility

  lcd.setCursor(0, 0); // Set the cursor to the first row, first column
  lcd.print("Right Ready"); // Display a status message
  delay(1000);         // Wait for 1 second before starting main loop
}

// The loop function runs repeatedly after setup
void loop() {
  // Read the analog value from the pressure sensor
  int analogReading = analogRead(FORCE_SENSOR_PIN);

  // Constrain the sensor value within expected min/max bounds,
  // then map that value to a scaled pressure value (0 to outputMax)
  long rightPressure = map(
    constrain(analogReading, sensorMin, sensorMax), // Keep within calibration range
    sensorMin, sensorMax,                           // Source range
    0, outputMax                                     // Target range
  );

  // Create a character array to hold the pressure value as a string
  char msg[10];
  itoa(rightPressure, msg, 10); // Convert the long value to a C-style string (base 10)

  // Send the pressure data using RF communication
  rf_driver.send((uint8_t *)msg, strlen(msg)); // Send message as byte array
  rf_driver.waitPacketSent();                  // Wait until the RF module finishes transmitting

  // Update the LCD screen with the current pressure value
  lcd.clear();               // Clear previous content on the display
  lcd.setCursor(0, 0);       // Move cursor to the first line
  lcd.print("R Pressure: "); // Label for the pressure reading
  lcd.print(rightPressure); // Display the actual pressure value

  // Pause for a second before repeating
  delay(1000);
}
