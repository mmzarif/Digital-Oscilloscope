#include <Arduino.h>
#include <LiquidCrystal.h>

int myFunction(int, int);

void calibrate();
void findMaxVoltage(); 
void findMinVoltage(); 
void findAvgVoltage(); 
void peakToPeak(); 

//declare 6 button pins
const int pk2pkButton = 6;
const int vmaxButton = 7;
const int vminButton = 8;
const int vavgButton = 9;
const int frqButton = 10;
const int calibrateButton = 11;

float minVoltage = 5.0; // Initialize min voltage to a high value
float maxVoltage = 0.0; // Initialize max voltage to a low value
float avgVoltage = 0.0; // Initialize average voltage to 0.0
float peakToPeakVoltage = 0.0; // Initialize peak-to-peak voltage to 0.0

//declare ADC pin to act as oscilloscope probe
const int probe1 = A0; // ADC pin for probe 1

const int voltageResolution = A1; 

float frequency = 0.0; // Frequency variable

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  bool pk2pkButtonState = LOW;
  bool vmaxButtonState = LOW;
  bool vminButtonState = LOW;
  bool vavgButtonState = LOW;
  bool frqButtonState = LOW;
  bool calibrateButtonState = LOW;
  delay(2000);
  Serial.begin(9600);
}

void loop() {
  float voltage = analogRead(probe1); // Read the voltage from probe 1
  //map the voltage to a range of 0-5V with 10-bit resolution
  voltage = map(voltage, 0, 1023, 0, 5000) / 1000; // Convert to volts
  Serial.println(voltage);

  if (digitalRead(pk2pkButton) == HIGH) {
    lcd.clear();
    lcd.print("pk2pkButton pressed");
    delay(1000);
  }
  if (digitalRead(vmaxButton) == HIGH) {
    lcd.clear();
    lcd.print("vmaxButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    findMaxVoltage(); // Call the function to find max voltage
    lcd.print("Max: ");
    lcd.print(maxVoltage); // Print max voltage
    lcd.print(" V");
    delay(1000);
  }
  if (digitalRead(vminButton) == HIGH) {
    lcd.clear();
    lcd.print("vminButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    findMinVoltage(); // Call the function to find min voltage
    lcd.print("Min: ");
    lcd.print(minVoltage); // Print min voltage
    lcd.print(" V");
    delay(1000);
  }
  if (digitalRead(vavgButton) == HIGH) {
    lcd.clear();
    lcd.print("vavgButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    findAvgVoltage(); // Call the function to find average voltage
    lcd.print("Avg: ");
    lcd.print(avgVoltage); // Print average voltage
    lcd.print(" V");
    delay(1000);
  }
  if (digitalRead(frqButton) == HIGH) {
    lcd.clear();
    lcd.print("frqButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    lcd.print("Freq: ");
    lcd.print(frequency); // Print frequency
    lcd.print(" Hz");
    delay(1000);
  }
  if (digitalRead(calibrateButton) == HIGH) {
    lcd.clear();
    lcd.print("calibrateButton pressed");
    calibrate(); // Call the calibration function
    delay(1000);
  }
  // Add a small delay to avoid button bounce
  delay(50);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void calibrate() {

  lcd.clear();
  lcd.print("Calibrating...");
  //measure voltages from probe for 30 seconds to find min and max
  unsigned long startTime = millis();
  
  while (millis() - startTime < 30000) { // 30 seconds
    float voltage = analogRead(probe1); // Read the voltage from probe 1
    voltage = map(voltage, 0, 1023, 0, 5000) / 1000; // Convert to volts
    if (voltage < minVoltage) {
      minVoltage = voltage; // Update min voltage
    }
    if (voltage > maxVoltage) {
      maxVoltage = voltage; // Update max voltage
    }
    //find time period of the signal by measuring the time between 0V and 5V crossings
    while (analogRead(probe1) != 0) {
      delay(1); // Wait for the signal to cross 0V
    }
    if (voltage > 0 && voltage < 5) {
      unsigned long time = millis();
      //wait for the signal to cross 5V
      while (analogRead(probe1) < 5) {
        delay(1);
      }
      unsigned long crossingTime = millis() - time;
      long period = crossingTime*4; // Time period in milliseconds
      //calculate frequency from time period
      frequency = 1000.0 / period; // Frequency in Hz
      lcd.setCursor(0, 1); // Set cursor to second line
      lcd.print("Freq: ");
      lcd.print(frequency);
      lcd.print(" Hz");

    }
    delay(100); // Small delay to avoid rapid reading
  }
  delay(2000);
  lcd.clear();
  lcd.print("Calibration done!");
  delay(2000);
}

//find max voltage in 10 seconds
void findMaxVoltage() {
  unsigned long startTime = millis();
  maxVoltage = 0.0; // Reset max voltage
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); // Read the voltage from probe 1
    voltage = map(voltage, 0, 1023, 0, 5000) / 1000; // Convert to volts
    if (voltage > maxVoltage) {
      maxVoltage = voltage; // Update max voltage
    }
    delay(100); 
  }
}
//find min voltage in 10 seconds
void findMinVoltage() {
  unsigned long startTime = millis();
  minVoltage = 5.0; // Reset min voltage
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); // Read the voltage from probe 1
    voltage = map(voltage, 0, 1023, 0, 5000) / 1000; // Convert to volts
    if (voltage < minVoltage) {
      minVoltage = voltage; // Update min voltage
    }
    delay(100); 
  }
}
//find average voltage in 10 seconds
void findAvgVoltage() {
  unsigned long startTime = millis();
  float totalVoltage = 0.0; // Initialize total voltage
  int count = 0; // Initialize count of readings
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); 
    voltage = map(voltage, 0, 1023, 0, 5000) / 1000; 
    totalVoltage += voltage; // Add to total voltage
    count++; 
    delay(100);
  }
  avgVoltage = totalVoltage / count; 
}

//find peak to peak voltage in 10 seconds
void peakToPeak() {
  unsigned long startTime = millis();
  float minVoltage = 5.0; 
  float maxVoltage = 0.0; 
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); 
    voltage = map(voltage, 0, 1023, 0, 5000) / 1000; 
    if (voltage < minVoltage) {
      minVoltage = voltage; 
    }
    if (voltage > maxVoltage) {
      maxVoltage = voltage; 
    }
    delay(100);
  }
  peakToPeakVoltage = maxVoltage - minVoltage; // Calculate peak-to-peak voltage
}