#include <Arduino.h>
#include <LiquidCrystal.h>

void calibrate();
void findMaxVoltage(); 
void findMinVoltage(); 
void findAvgVoltage(); 
void peakToPeak(); 
float mapVoltage();

//declare 6 button pins
const int pk2pkButton = 6;
const int vmaxButton = 7;
const int vminButton = 8;
const int vavgButton = 9;
const int frqButton = 10;
const int calibrateButton = 13;

float minVoltage = 10.0; // Initialize min voltage to a high value
float maxVoltage = -10.0; // Initialize max voltage to a low value
float avgVoltage = 0.0; // Initialize average voltage to 0.0
float peakToPeakVoltage = 0.0; // Initialize peak-to-peak voltage to 0.0

//declare ADC pin to act as oscilloscope probe
const int probe1 = A0; // ADC pin for probe 1

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
  delay(2000); // Wait for 2 seconds
  lcd.clear(); // Clear the LCD

  // Set button pins as input with pull-up resistors
  pinMode(pk2pkButton, INPUT_PULLUP);
  pinMode(vmaxButton, INPUT_PULLUP);
  pinMode(vminButton, INPUT_PULLUP);
  pinMode(vavgButton, INPUT_PULLUP);
  pinMode(frqButton, INPUT_PULLUP);
  pinMode(calibrateButton, INPUT_PULLUP);
  pinMode(probe1, INPUT); // Set probe pin as input
  
  lcd.print("calibrating on start up...");
  delay(2000); // Wait for 2 seconds
  Serial.begin(9600);
  calibrate(); // Call the calibration function
  lcd.clear();
  lcd.print("ready!");
}

void loop() {
  float voltage = analogRead(probe1); // Read the voltage from probe 1
  voltage = mapVoltage(voltage);
  Serial.println(voltage);

  if (digitalRead(pk2pkButton) == LOW) {
    lcd.clear();
    lcd.print("pk2pkButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    peakToPeak(); // Call the function to find peak-to-peak voltage
    lcd.print("pk2pk: ");
    lcd.print(peakToPeakVoltage);
    lcd.print(" V");
  }
  
  if (digitalRead(vmaxButton) == LOW) {
    lcd.clear();
    lcd.print("vmaxButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    findMaxVoltage(); // Call the function to find max voltage
    lcd.print("Max: ");
    lcd.print(maxVoltage); // Print max voltage
    lcd.print(" V");
  }
  
  if (digitalRead(vminButton) == LOW) {
    lcd.clear();
    lcd.print("vminButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    findMinVoltage(); // Call the function to find min voltage
    lcd.print("Min: ");
    lcd.print(minVoltage); // Print min voltage
    lcd.print(" V");
  } 
  
  if (digitalRead(vavgButton) == LOW) {
    lcd.clear();
    lcd.print("vavgButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    findAvgVoltage(); // Call the function to find average voltage
    lcd.print("Avg: ");
    lcd.print(avgVoltage); // Print average voltage
    lcd.print(" V");
  } 
  
  if (digitalRead(frqButton) == LOW) {
    lcd.clear();
    lcd.print("frqButton pressed");
    lcd.setCursor(0, 1); // Set cursor to second line
    lcd.print("Freq: ");
    lcd.print(frequency); // Print frequency
    lcd.print(" Hz");
  }
  
  if (digitalRead(calibrateButton) == LOW) {
    lcd.clear();
    lcd.print("calibrateButton pressed");
    delay(1000); // Wait for 1 second
    calibrate(); // Call the calibration function
    delay(50);
  }
}

void calibrate() {

  lcd.clear();
  lcd.print("Calibrating...");
  //measure voltages from probe for 30 seconds to find min and max
  unsigned long startTime = millis();
  float voltage;
  maxVoltage = -10.0;
  minVoltage = 10.0;
  
  while (millis() - startTime < 10000) { // 30 seconds
    voltage = analogRead(probe1); // Read the voltage from probe 1
    //voltage = map(voltage, 0, 1023, 0, 5000) / 1000; // Convert to volts DOES NOT HANDLE FRACTIONS
    voltage = mapVoltage(voltage);
    if (voltage < minVoltage) {
      minVoltage = voltage; // Update min voltage
    }
    if (voltage > maxVoltage) {
      maxVoltage = voltage; // Update max voltage
    }
    Serial.println(voltage);
  }

  avgVoltage = (maxVoltage + minVoltage) / 2; // Calculate average voltage
  startTime = millis(); // Reset start time for frequency measurement
  unsigned long crossingTime = 0; // Initialize crossing time
  int crossings = 0; // Initialize crossing count
  frequency = 0.0; // Reset frequency
  while (millis() - startTime < 10000) { // 30 seconds
    voltage = digitalRead(probe1);
    voltage = mapVoltage(voltage);
    if (voltage == avgVoltage) {
      crossings++;
      crossingTime += millis() - startTime - crossingTime; // Time since the last crossing
    }
    Serial.println(voltage);
  }
  if (crossings > 0) {
    crossingTime /= crossings; // Average time between crossings
    // Calculate frequency from time period
    long period = crossingTime * 4; // Time period in milliseconds
    frequency = 1/(period*1000); // Frequency in Hz
    lcd.setCursor(0, 1); // Set cursor to second line
    lcd.print("Freq: ");
    lcd.print(frequency);
    lcd.print(" Hz");
    lcd.setCursor(0, 0); // Set cursor to first line
    delay(100); // Small delay to avoid rapid reading
  }
  delay(3000);
  lcd.clear();
  lcd.print("Calibration done!");
  delay(1000);
}

//find max voltage in 10 seconds
void findMaxVoltage() {
  unsigned long startTime = millis();
  maxVoltage = -10.0; // Reset max voltage
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); // Read the voltage from probe 1
    voltage = mapVoltage(voltage);
    if (voltage > maxVoltage) {
      maxVoltage = voltage; // Update max voltage
    }
    Serial.println(voltage);
    //delay(100); 
  }
}
//find min voltage in 10 seconds
void findMinVoltage() {
  unsigned long startTime = millis();
  minVoltage = 10.0; // Reset min voltage
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); // Read the voltage from probe 1
    voltage = mapVoltage(voltage);
    if (voltage < minVoltage) {
      minVoltage = voltage; // Update min voltage
    }
    Serial.println(voltage);
    //delay(100); 
  }
}
//find average voltage in 10 seconds
void findAvgVoltage() {
  unsigned long startTime = millis();
  float totalVoltage = 0.0; // Initialize total voltage
  int count = 0; // Initialize count of readings
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); 
    voltage = mapVoltage(voltage);
    totalVoltage += voltage; // Add to total voltage
    count++; 
    //delay(100);
    Serial.println(voltage);
  }
  avgVoltage = totalVoltage / count; 
}

//find peak to peak voltage in 10 seconds
void peakToPeak() {
  unsigned long startTime = millis();
  float minVoltage = 10.0; 
  float maxVoltage = -10.0; 
  while (millis() - startTime < 10000) { // 10 seconds
    float voltage = analogRead(probe1); 
    voltage = mapVoltage(voltage);
    if (voltage < minVoltage) {
      minVoltage = voltage; 
    }
    if (voltage > maxVoltage) {
      maxVoltage = voltage; 
    }
    Serial.println(voltage);
    //delay(100);
  }
  peakToPeakVoltage = maxVoltage - minVoltage; // Calculate peak-to-peak voltage
}

float mapVoltage(float voltage) {
  if (voltage < 511.5)
    return -((511.5 - voltage) * 5.0 / 511.5);
  else 
    return ((voltage - 511.5) * 5.0 / 511.5);
}
