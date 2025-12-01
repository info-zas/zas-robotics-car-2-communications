// Remote Arduino = Bluetooth Gateway + NRF24 Transmitter

#include <SoftwareSerial.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

SoftwareSerial BT(10, 11);  // HC-05 RX, TX

RF24 radio(9,10);           // CE, CSN
const byte address[6] = "HOP01";

Adafruit_SSD1306 display(128,64,&Wire,-1);

// Data structure sent via NRF24
struct Packet {
  char cmd;   // 'F','B','L','R','S','V'
};

void showOLED(String msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("BT->NRF");
  display.println(msg);
  display.display();
}

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // NRF24 setup
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(90);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  if (BT.available()) {
    String cmd = BT.readStringUntil('\n');
    cmd.trim();

    Packet p;
    p.cmd = cmd[0];   // First character only

    radio.write(&p, sizeof(p));
    showOLED(cmd);

    Serial.println("Forwarded: " + cmd);
  }
}
