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

void setup() {
  Serial.begin(9600);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  pinMode(A0, INPUT); // Joystick X
  pinMode(A1, INPUT); // Joystick Y
}

void loop() {
  Payload data;
  data.x = analogRead(A0);
  data.y = analogRead(A1);

  radio.write(&data, sizeof(data));

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("TRANSMITTER");
  display.print("X: "); display.println(data.x);
  display.print("Y: "); display.println(data.y);
  display.display();

  delay(100);
}
