#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ========= RADIO ========= */
RF24 radio(A3, A2);          // CE, CSN
const byte address[6] = "00001";

/* ========= OLED ========= */
Adafruit_SSD1306 display(128, 64, &Wire);

/* ========= BUTTON PINS ========= */
#define BTN_UP     7
#define BTN_DOWN   6
#define BTN_LEFT   5
#define BTN_RIGHT  4

/* ========= DATA STRUCT ========= */
struct Payload {
  int x;
  int y;
};

/* ========= SETUP ========= */
void setup() {
  Serial.begin(9600);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Radio (TRANSMITTER)
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  // Button pins (using internal pull-ups)
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
}

/* ========= LOOP ========= */
void loop() {
  Payload data;

  // Default = STOP (center position)
  data.x = 512;
  data.y = 512;

  String dir = "STOP";

  // Button logic (active LOW)
  if (digitalRead(BTN_UP) == LOW) {
    data.y = 700;          // FORWARD
    dir = "FORWARD";
  }
  else if (digitalRead(BTN_DOWN) == LOW) {
    data.y = 300;          // BACKWARD
    dir = "BACKWARD";
  }
  else if (digitalRead(BTN_LEFT) == LOW) {
    data.x = 300;          // LEFT
    dir = "LEFT";
  }
  else if (digitalRead(BTN_RIGHT) == LOW) {
    data.x = 700;          // RIGHT
    dir = "RIGHT";
  }

  // Send data
  radio.write(&data, sizeof(data));

  // OLED display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("TRANSMITTER");
  display.print("DIR: ");
  display.println(dir);
  display.display();

  delay(100);
}
