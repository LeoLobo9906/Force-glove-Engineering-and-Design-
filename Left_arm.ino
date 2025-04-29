#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RH_ASK.h>

// RF receiver setup
RH_ASK rf_driver;

// LCD setup (I2C address 0x27 is common)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pressure sensor pin
#define FORCE_SENSOR_PIN A0

// LED pins
#define LED_PIN_RIGHT 13  // LED turns on if right pressure is more
#define LED_PIN_LEFT 12   // LED turns on if left pressure is more

// Calibration values
const int sensorMin = 670;
const int sensorMax = 9162;
const int outputMax = 10000; // Max pressure value

long rightPressure = -1;  // Initialize to -1 to track if data is received

void setup() {
  rf_driver.init(); // Initialize RF communication
  lcd.init();       // Initialize LCD
  lcd.backlight();  // Turn on the LCD backlight

  lcd.setCursor(0, 0);
  lcd.print("Left Ready");
  delay(1000);

  pinMode(LED_PIN_RIGHT, OUTPUT); // Right LED pin
  pinMode(LED_PIN_LEFT, OUTPUT);  // Left LED pin
}

void loop() {
  // Read the left pressure sensor
  int analogReading = analogRead(FORCE_SENSOR_PIN);
  long leftPressure = map(
    constrain(analogReading, sensorMin, sensorMax),
    sensorMin, sensorMax,
    0, outputMax
  );

  // Receive message from the right device
  uint8_t buf[32];
  uint8_t buflen = sizeof(buf);
  if (rf_driver.recv(buf, &buflen)) {
    buf[buflen] = '\0';  // Null-terminate the string
    rightPressure = atol((char*)buf); // Convert received message to long
  }

  // Display left pressure on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("L Pressure: ");
  lcd.print(leftPressure);

  // LED Logic
  if (rightPressure >= 0 && rightPressure > leftPressure + 100) {
    lcd.setCursor(0, 1);
    lcd.print("Right > Left");

    digitalWrite(LED_PIN_RIGHT, HIGH);  // Right LED ON
    digitalWrite(LED_PIN_LEFT, LOW);    // Left LED OFF
  }
  else if (rightPressure >= 0 && leftPressure > rightPressure + 100) {
    lcd.setCursor(0, 1);
    lcd.print("Left > Right");

    digitalWrite(LED_PIN_RIGHT, LOW);   // Right LED OFF
    digitalWrite(LED_PIN_LEFT, HIGH);   // Left LED ON
  }
  else {


    digitalWrite(LED_PIN_RIGHT, LOW);
    digitalWrite(LED_PIN_LEFT, LOW);
  }

  delay(1000);  // Wait before looping again
}
