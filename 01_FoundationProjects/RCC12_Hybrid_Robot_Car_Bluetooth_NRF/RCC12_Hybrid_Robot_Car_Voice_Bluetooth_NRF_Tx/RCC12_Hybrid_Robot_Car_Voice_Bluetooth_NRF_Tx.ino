#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- NRF24 ----------------
RF24 radio(A3, A2); // CE, CSN
const byte address[6] = "00001";

// ---------------- OLED ----------------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------------- DATA ----------------
char command = 'S';   // Default STOP

void setup() {
  Serial.begin(9600);   // HC-05 baud rate

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // NRF24
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {

  // ---------- READ BLUETOOTH ----------
  if (Serial.available()) {
    command = Serial.read();   // F B L R S
    radio.write(&command, sizeof(command));
  }

  // ---------- OLED ----------
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("TX MODE");
  display.print("CMD: ");
  display.println(command);
  display.display();

  delay(50);
}

