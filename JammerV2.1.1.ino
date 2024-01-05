#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "BluetoothSerial.h"
#include <LiquidCrystal_I2C.h>
#include "UtilsJammer.h"

BluetoothSerial SerialBT;
RF24 radio(5, 27);                   // CE, CSN
LiquidCrystal_I2C lcd(0x27, 20, 4);  // LCD address 0x27

const byte address[6] = "00001";
const int maxChannels = 126; 
int activeChannels[maxChannels];
int activeChannelCount = 0;
int transmissionDuration = 1000;
int ledPin = 4;
int button1 = 12;
int button2 = 2;
int button3 = 15;
int channel = 0;
int scanning = 0;
volatile bool isSelected = false;
char printText[100];

volatile bool stateButton1 = false;
volatile bool stateButton2 = false;
volatile bool stateButton3 = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(button1, INPUT_PULLDOWN);
  pinMode(button2, INPUT_PULLDOWN);
  pinMode(button3, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(button1), handleButton1Interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(button2), handleButton2Interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(button3), handleButton3Interrupt, RISING);

  Serial.begin(115200);
  SerialBT.begin("ESP32BT JAMMER");

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX, 1);
  radio.stopListening();

  lcd.init();
  lcd.backlight();

  delay(1000);
  if (radio.isChipConnected() && radio.isPVariant()) {
    strcpy(printText, "nrf24L01+ module is connected and responding.");
    printAll(printText);
    delay(1500);
    strcpy(printText, "nrf24L01+ OK");
    printAll(printText);
  } else {
    strcpy(printText, "nrf24L01+ Error");
    printAll(printText);
    return;
  }
  strcpy(printText, "Setup complete");
  printAll(printText);
}

void loop() {
   if (scanning < 6) {
    for (int channel = 0; channel <= 125; channel++) {
      radio.setChannel(channel);
      radio.startListening();
      delay(5);
      if (radio.testCarrier() && !isChannelAlreadyDetected(channel)) {
        Serial.print("Activity detected on channel: ");
        Serial.println(channel);
        if (activeChannelCount < maxChannels) {
          activeChannels[activeChannelCount++] = channel;
        }
      }
      radio.stopListening();
    }
    delay(20);
    scanning++;
    displayActiveChannels();
  }


  const char text[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                      "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                      "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                      "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                      "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                      "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

  if (isSelected) {
    startTransmission(text);
  }
}

void startTransmission(const char* text) {
  delay(500);
  stateButton1 = stateButton2 = stateButton3 = false;
  for (int i = channel; i <= channel + 23; i++) {
    radio.setChannel(i);
    radio.stopListening();
    strcpy(printText, "channel selected, transmitting");
    SerialBT.println(i);
    printAll(printText);
    unsigned long startTime = millis();
    while (millis() - startTime < transmissionDuration) {
      if (stateButton1 || stateButton2 || stateButton3) {
        goto endTransmission;
      }
      radio.write(text, sizeof(text));
      digitalWrite(ledPin, HIGH);
    }
    digitalWrite(ledPin, LOW);
    if (i == channel + 23) {
      i = channel;
    }
  }
endTransmission:
  digitalWrite(ledPin, LOW);
  strcpy(printText, "End of transmission");
  printAll(printText);
  isSelected = false;
  stateButton1 = stateButton2 = stateButton3 = false;
  delay(1200);
  displayActiveChannels();
}

void handleButton1Interrupt() {
  handleInterruptCommon(&stateButton1, 12);
}

void handleButton2Interrupt() {
  handleInterruptCommon(&stateButton2, 36);
}

void handleButton3Interrupt() {
  handleInterruptCommon(&stateButton3, 60);
}

void handleInterruptCommon(volatile bool* stateButton, int newChannel) {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200) {
    *stateButton = true;
    if (isSelected) {
      isSelected = false;
    } else {
      channel = newChannel;
      isSelected = true;
    }
  }
  lastInterruptTime = interruptTime;
}

