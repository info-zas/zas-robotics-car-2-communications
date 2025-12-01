#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

RF24 radio(9,10);
const byte address[6] = "TOUCH";

Adafruit_SSD1306 display(128,64,&Wire,-1);

#define Left1 7
#define Left2 8
#define Left_Speed 6
#define Right1 3
#define Right2 4
#define Right_Speed 5

struct Payload {
  bool touched;
};

Payload incoming;

void motorForward() {
  digitalWrite(Left1,HIGH); digitalWrite(Left2,LOW);
  digitalWrite(Right1,HIGH); digitalWrite(Right2,LOW);
  analogWrite(Left_Speed,200);
  analogWrite(Right_Speed,200);
}

void motorStop() {
  digitalWrite(Left1,LOW); digitalWrite(Left2,LOW);
  digitalWrite(Right1,LOW); digitalWrite(Right2,LOW);
  analogWrite(Left_Speed,0);
  analogWrite(Right_Speed,0);
}

void show(String msg) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("CAR RX");
  display.println(msg);
  display.display();
}

void setup() {
  pinMode(Left1,OUTPUT);
  pinMode(Left2,OUTPUT);
  pinMode(Right1,OUTPUT);
  pinMode(Right2,OUTPUT);
  pinMode(Left_Speed,OUTPUT);
  pinMode(Right_Speed,OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.clearDisplay();

  radio.begin();
  radio.openReadingPipe(1,address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&incoming, sizeof(incoming));

    if (incoming.touched) {
      motorForward();
      show("FORWARD");
    } else {
      motorStop();
      show("STOP");
    }
  }
}
