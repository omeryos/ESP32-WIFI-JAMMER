// UtilsJammer.cpp
#include "UtilsJammer.h"
#include <Arduino.h>
#include <BluetoothSerial.h>    
#include <LiquidCrystal_I2C.h>  


extern BluetoothSerial SerialBT;
extern LiquidCrystal_I2C lcd;
extern int activeChannels[];
extern int activeChannelCount;


void printAll(char* printText) {
  lcd.clear();
  Serial.println(printText);
  SerialBT.println(printText);  

  lcd.setCursor(0, 0);
  lcd.print(String(printText).substring(0, 20));
 
  if (strlen(printText) > 20) {
    lcd.setCursor(0, 1);
    lcd.print(String(printText).substring(20, 40));
  }
  
  if (strlen(printText) > 40) {
    lcd.setCursor(0, 2);
    lcd.print(String(printText).substring(40, 60));
  }
  
  if (strlen(printText) > 60) {
    lcd.setCursor(0, 3);
    lcd.print(String(printText).substring(60, 80));
  }
}
void displayActiveChannels() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Active Channels:");

    int channelsPerLine = 7;  

    for (int i = 0; i < activeChannelCount; i++) {
        if (i % channelsPerLine == 0) {
            lcd.setCursor(0, i / channelsPerLine + 1);  
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
