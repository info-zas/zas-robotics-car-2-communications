/************************************************************
 * LAB 15 – MULTI-MODE ROBOT CONTROLLER (TRANSMITTER)
 *
 * Sends over NRF24:
 *   - Joystick X/Y
 *   - Accelerometer X/Y
 *   - Button bitmask
 *   - Selected mode (1–5)
 *
 * Modes:
 * 1 = Joystick
 * 2 = Tilt
 * 3 = IR (local to robot)
 * 4 = Bluetooth (local to robot)
 * 5 = Obstacle (robot autonomous)
 ************************************************************/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- DISPLAY ----------------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------------- NRF24 -------------------
RF24 radio(9, 10);   // CE, CSN
const byte address[6] = "MOD15";

// ---------------- ACCELEROMETER -----------
MPU6050 mpu;

// ---------------- JOYSTICK ----------------
#define JOYX A0
#define JOYY A1

// ---------------- BUTTONS -----------------
#define BTN1 2
#define BTN2 3
#define BTN3 4
#define BTN4 5

// ---------------- MODE BUTTON --------------
#define MODE_BTN 7
int currentMode = 1;

// ---------------- PAYLOAD ------------------
struct Payload {
  int16_t joyX;
  int16_t joyY;
  int16_t accelX;
  int16_t accelY;
  uint8_t buttons;
  uint8_t mode;
};

Payload outgoing;

// OLED helper
void show(String line1, String line2 = "") {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2); display.println(line1);
  display.setTextSize(1); display.println(line2);
  display.display();
}

// Read buttons into bitmask
uint8_t readButtons() {
  uint8_t b = 0;
  if (!digitalRead(BTN1)) b |= 0x01;
  if (!digitalRead(BTN2)) b |= 0x02;
  if (!digitalRead(BTN3)) b |= 0x04;
  if (!digitalRead(BTN4)) b |= 0x08;
  return b;
}

// Cycle modes when mode button pressed
void checkModeButton() {
  static unsigned long lastPress = 0;
  if (!digitalRead(MODE_BTN) && millis() - lastPress > 300) {
    currentMode++;
    if (currentMode > 5) currentMode = 1;
    lastPress = millis();
    show("MODE", String(currentMode));
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  pinMode(MODE_BTN, INPUT_PULLUP);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  show("REMOTE", "Booting...");

  // NRF24
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();

  // Accelerometer
  Wire.begin();
  mpu.initialize();
  delay(200);

  show("REMOTE", "Ready");
}

void loop() {

  checkModeButton();

  // Joystick
  outgoing.joyX = analogRead(JOYX);
  outgoing.joyY = analogRead(JOYY);

  // Accelerometer
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  outgoing.accelX = ax;
  outgoing.accelY = ay;

  // Buttons
  outgoing.buttons = readButtons();

  // Mode
  outgoing.mode = currentMode;

  // Send payload
  radio.write(&outgoing, sizeof(outgoing));

  // Debug OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("Mode: "); display.println(currentMode);
  display.print("JX: "); display.println(outgoing.joyX);
  display.print("JY: "); display.println(outgoing.joyY);
  display.print("AX: "); display.println(outgoing.accelX);
  display.print("AY: "); display.println(outgoing.accelY);
  display.print("Buttons: "); display.println(outgoing.buttons, BIN);
  display.display();

  delay(70); // smooth sending
}
