// ------------- Remote Transmitter -------------
// Inputs: 4 buttons, joystick (A0/A1), MPU6050 accelerometer
// Output: ControlPayload over NRF24L01 + status on OLED

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050_light.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// NRF24L01 pins
RF24 radio(9, 10);              // CE, CSN
const byte address[6] = "CAR01";

// Joystick pins
#define JOY_X A0
#define JOY_Y A1

// Button pins (active LOW with INPUT_PULLUP)
#define BTN1 2   // e.g. Forward
#define BTN2 3   // Backward
#define BTN3 4   // Left
#define BTN4 5   // Right
#define MODE_BTN 6  // Cycle between modes

// Accelerometer (MPU6050)
MPU6050 mpu(Wire);

// Modes
enum ControlMode : uint8_t {
  MODE_BUTTONS = 0,
  MODE_TILT    = 1,
  MODE_JOYSTICK = 2
};

volatile ControlMode currentMode = MODE_BUTTONS;

// Shared payload struct
struct ControlPayload {
  int16_t joyX;
  int16_t joyY;
  int16_t accelX;
  int16_t accelY;
  uint8_t buttons;   // bit0=BTN1, bit1=BTN2, bit2=BTN3, bit3=BTN4
  uint8_t mode;      // see ControlMode
};

unsigned long lastModePress = 0;
const unsigned long MODE_DEBOUNCE = 300; // ms

void showStatus(const ControlPayload &p) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.println("REMOTE TX");
  display.print("Mode: ");
  if (p.mode == MODE_BUTTONS)   display.println("Buttons");
  else if (p.mode == MODE_TILT) display.println("Tilt");
  else if (p.mode == MODE_JOYSTICK) display.println("Joystick");

  display.print("Joy X: "); display.println(p.joyX);
  display.print("Joy Y: "); display.println(p.joyY);
  display.print("Ax: ");  display.println(p.accelX);
  display.print("Ay: ");  display.println(p.accelY);

  display.print("Btns: ");
  display.println(p.buttons, BIN);

  display.display();
}

void setupButtons() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  pinMode(MODE_BTN, INPUT_PULLUP);
}

void checkModeButton() {
  if (digitalRead(MODE_BTN) == LOW) {
    unsigned long now = millis();
    if (now - lastModePress > MODE_DEBOUNCE) {
      lastModePress = now;
      currentMode = static_cast<ControlMode>((currentMode + 1) % 3);
    }
  }
}

uint8_t readButtons() {
  uint8_t mask = 0;
  if (digitalRead(BTN1) == LOW) mask |= 0x01;
  if (digitalRead(BTN2) == LOW) mask |= 0x02;
  if (digitalRead(BTN3) == LOW) mask |= 0x04;
  if (digitalRead(BTN4) == LOW) mask |= 0x08;
  return mask;
}

void setup() {
  Serial.begin(9600);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // OLED failure: just continue without display
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Buttons & joystick
  setupButtons();
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);

  // MPU6050
  Wire.begin();
  mpu.begin();
  mpu.calcOffsets();  // quick calibration (can be done once and saved)

  // NRF24
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  checkModeButton();

  // Read joystick
  int joyX = analogRead(JOY_X); // 0–1023
  int joyY = analogRead(JOY_Y);

  // Read accel
  mpu.update();
  int16_t ax = mpu.getAccX() * 1000;  // scale to int
  int16_t ay = mpu.getAccY() * 1000;

  // Read buttons
  uint8_t btnMask = readButtons();

  ControlPayload payload;
  payload.joyX = joyX;
  payload.joyY = joyY;
  payload.accelX = ax;
  payload.accelY = ay;
  payload.buttons = btnMask;
  payload.mode = currentMode;

  // Send
  radio.write(&payload, sizeof(payload));

  // Show on OLED
  showStatus(payload);

  delay(80);
}
