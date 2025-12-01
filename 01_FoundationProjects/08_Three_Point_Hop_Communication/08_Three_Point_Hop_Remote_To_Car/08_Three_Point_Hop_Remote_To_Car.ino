// Car Arduino = NRF24 Receiver + Motor Control

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// NRF24
RF24 radio(9,10);
const byte address[6] = "HOP01";

// OLED
Adafruit_SSD1306 display(128,64,&Wire,-1);

// Motor pins
#define Left1      7
#define Left2      8
#define Left_Speed 6
#define Right1     3
#define Right2     4
#define Right_Speed 5

// Payload structure
struct Packet {
  char cmd;
};

Packet incoming;

// Motor helpers
void moveForward() {
  digitalWrite(Left1,HIGH); digitalWrite(Left2,LOW);
  digitalWrite(Right1,HIGH); digitalWrite(Right2,LOW);
  analogWrite(Left_Speed,200);
  analogWrite(Right_Speed,200);
}

void moveBackward() {
  digitalWrite(Left1,LOW); digitalWrite(Left2,HIGH);
  digitalWrite(Right1,LOW); digitalWrite(Right2,HIGH);
  analogWrite(Left_Speed,200);
  analogWrite(Right_Speed,200);
}

void turnLeft() {
  digitalWrite(Left1,LOW); digitalWrite(Left2,HIGH);
  digitalWrite(Right1,HIGH); digitalWrite(Right2,LOW);
  analogWrite(Left_Speed,180);
  analogWrite(Right_Speed,180);
}

void turnRight() {
  digitalWrite(Left1,HIGH); digitalWrite(Left2,LOW);
  digitalWrite(Right1,LOW); digitalWrite(Right2,HIGH);
  analogWrite(Left_Speed,180);
  analogWrite(Right_Speed,180);
}

void stopCar() {
  digitalWrite(Left1,LOW); digitalWrite(Left2,LOW);
  digitalWrite(Right1,LOW); digitalWrite(Right2,LOW);
  analogWrite(Left_Speed,0);
  analogWrite(Right_Speed,0);
}

void showOLED(String msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("RX:");
  display.println(msg);
  display.display();
}

void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(Left1,OUTPUT); pinMode(Left2,OUTPUT);
  pinMode(Right1,OUTPUT); pinMode(Right2,OUTPUT);
  pinMode(Left_Speed,OUTPUT);
  pinMode(Right_Speed,OUTPUT);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // NRF24
  radio.begin();
  radio.openReadingPipe(1,address);
  radio.setChannel(90);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&incoming, sizeof(incoming));

    char c = incoming.cmd;

    if      (c == 'F') moveForward();
    else if (c == 'B') moveBackward();
    else if (c == 'L') turnLeft();
    else if (c == 'R') turnRight();
    else if (c == 'S') stopCar();

    showOLED(String(c));
    Serial.println("Received: " + String(c));
  }
}
