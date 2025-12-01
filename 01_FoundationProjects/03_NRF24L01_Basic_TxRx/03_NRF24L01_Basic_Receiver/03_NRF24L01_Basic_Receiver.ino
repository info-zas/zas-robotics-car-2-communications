#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RF24 radio(A3, A2); // CE, CSN
const byte address[6] = "00001";

Adafruit_SSD1306 display(128, 64, &Wire);

struct Payload {
  int x;
  int y;
};

Payload incoming;

void setup() {
  Serial.begin(9600);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Radio
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("RECEIVER");

  if (radio.available()) {
    radio.read(&incoming, sizeof(incoming));

    display.print("X: "); display.println(incoming.x);
    display.print("Y: "); display.println(incoming.y);
  } 
  else {
    display.println("Waiting...");
  }

  display.display();
  delay(50);
}
