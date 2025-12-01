#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define TOUCH_PIN 4

RF24 radio(9,10);
const byte address[6] = "TOUCH";

Adafruit_SSD1306 display(128,64,&Wire,-1);

struct Payload {
  bool touched;
};

void show(String msg) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("TOUCH TX");
  display.println(msg);
  display.display();
}

void setup() {
  pinMode(TOUCH_PIN, INPUT);

  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.clearDisplay();

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
}

void loop() {
  Payload p;
  p.touched = digitalRead(TOUCH_PIN);

  radio.write(&p, sizeof(p));

  if (p.touched) show("TOUCHED");
  else show("NO TOUCH");

  delay(70);
}
