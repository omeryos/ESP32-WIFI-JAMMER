// UtilsJammer.cpp
#include "UtilsJammer.h"
#include <Arduino.h>
#include <BluetoothSerial.h>    // Include Bluetooth library
#include <LiquidCrystal_I2C.h>  // Include LCD library

// Declare external references to your Bluetooth and LCD objects if they are instantiated elsewhere
extern BluetoothSerial SerialBT;
extern LiquidCrystal_I2C lcd;
extern int activeChannels[];
extern int activeChannelCount;


void printAll(char* printText) {
  lcd.clear();
  Serial.println(printText);
  SerialBT.println(printText);  // Using Bluetooth Serial
  // Printing to LCD
  // Print up to 20 characters on the first line
  lcd.setCursor(0, 0);
  lcd.print(String(printText).substring(0, 20));
  // Print the next 20 characters on the second line
  if (strlen(printText) > 20) {
    lcd.setCursor(0, 1);
    lcd.print(String(printText).substring(20, 40));
  }
  // Print the next 20 characters on the third line
  if (strlen(printText) > 40) {
    lcd.setCursor(0, 2);
    lcd.print(String(printText).substring(40, 60));
  }
  // Print the next 20 characters on the fourth line
  if (strlen(printText) > 60) {
    lcd.setCursor(0, 3);
    lcd.print(String(printText).substring(60, 80));
  }
}
void displayActiveChannels() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Active Channels:");

    int channelsPerLine = 7;  // Set to 7 channels per line

    for (int i = 0; i < activeChannelCount; i++) {
        if (i % channelsPerLine == 0) {
            lcd.setCursor(0, i / channelsPerLine + 1);  // Move to the next line after every 7 channels
        }
        lcd.print(activeChannels[i]);
        lcd.print(" ");
    }
}
bool isChannelAlreadyDetected(int channel) {
    for (int i = 0; i < activeChannelCount; i++) {
        if (activeChannels[i] == channel) {
            return true; 
        }
    }
    return false; 
}
