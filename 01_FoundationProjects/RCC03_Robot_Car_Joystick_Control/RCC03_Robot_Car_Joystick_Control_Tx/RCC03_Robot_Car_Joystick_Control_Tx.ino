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

  // Joystick pins
  pinMode(A0, INPUT);   // X-axis
  pinMode(A1, INPUT);   // Y-axis
}

/* ========= LOOP ========= */
void loop() {
  Payload data;

  // Read joystick
  data.x = analogRead(A0);
  data.y = analogRead(A1);

  // Send data
  radio.write(&data, sizeof(data));

  // OLED display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("TRANSMITTER");
  display.print("X: "); display.println(data.x);
  display.print("Y: "); display.println(data.y);
  display.display();

  delay(100);
}

