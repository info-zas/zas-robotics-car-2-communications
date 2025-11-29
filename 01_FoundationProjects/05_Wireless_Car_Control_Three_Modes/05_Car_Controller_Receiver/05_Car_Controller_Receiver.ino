// ------------- Car Receiver -------------
// Receives ControlPayload and drives motors via TB6612
// Shows current mode + movement on OLED

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// NRF24
RF24 radio(9, 10);              // CE, CSN
const byte address[6] = "CAR01";

// Motor pins (as you specified)
#define Left1       7
#define Left2       8
#define Left_Speed  6
#define Right1      3
#define Right2      4
#define Right_Speed 5

// Modes (must match transmitter)
enum ControlMode : uint8_t {
  MODE_BUTTONS = 0,
  MODE_TILT    = 1,
  MODE_JOYSTICK = 2
};

// Same struct as TX
struct ControlPayload {
  int16_t joyX;
  int16_t joyY;
  int16_t accelX;
  int16_t accelY;
  uint8_t buttons;
  uint8_t mode;
};

ControlPayload payload;
String lastAction = "STOP";

void setupMotors() {
  pinMode(Left1, OUTPUT);
  pinMode(Left2, OUTPUT);
  pinMode(Right1, OUTPUT);
  pinMode(Right2, OUTPUT);
  pinMode(Left_Speed, OUTPUT);
  pinMode(Right_Speed, OUTPUT);
}

void setMotors(int leftSpeed, int rightSpeed) {
  // leftSpeed/rightSpeed: -255..255
  // Left motor
  if (leftSpeed > 0) {
    digitalWrite(Left1, HIGH);
    digitalWrite(Left2, LOW);
  } else if (leftSpeed < 0) {
    digitalWrite(Left1, LOW);
    digitalWrite(Left2, HIGH);
  } else {
    digitalWrite(Left1, LOW);
    digitalWrite(Left2, LOW);
  }
  analogWrite(Left_Speed, min(abs(leftSpeed), 255));

  // Right motor
  if (rightSpeed > 0) {
    digitalWrite(Right1, HIGH);
    digitalWrite(Right2, LOW);
  } else if (rightSpeed < 0) {
    digitalWrite(Right1, LOW);
    digitalWrite(Right2, HIGH);
  } else {
    digitalWrite(Right1, LOW);
    digitalWrite(Right2, LOW);
  }
  analogWrite(Right_Speed, min(abs(rightSpeed), 255));
}

void stopMotors() {
  setMotors(0, 0);
  lastAction = "STOP";
}

void moveForward(int speed = 200) {
  setMotors(speed, speed);
  lastAction = "FORWARD";
}

void moveBackward(int speed = 200) {
  setMotors(-speed, -speed);
  lastAction = "BACKWARD";
}

void turnLeft(int speed = 180) {
  setMotors(-speed, speed);
  lastAction = "LEFT";
}

void turnRight(int speed = 180) {
  setMotors(speed, -speed);
  lastAction = "RIGHT";
}

void showStatus(const ControlPayload &p) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.println("CAR RX");
  display.print("Mode: ");
  if (p.mode == MODE_BUTTONS)   display.println("Buttons");
  else if (p.mode == MODE_TILT) display.println("Tilt");
  else if (p.mode == MODE_JOYSTICK) display.println("Joystick");

  display.print("JoyX: "); display.println(p.joyX);
  display.print("JoyY: "); display.println(p.joyY);
  display.print("Ax: ");  display.println(p.accelX);
  display.print("Ay: ");  display.println(p.accelY);

  display.print("Act: ");
  display.println(lastAction);

  display.display();
}

void handleButtons(const ControlPayload &p) {
  // bit0=BTN1, bit1=BTN2, bit2=BTN3, bit3=BTN4
  if (p.buttons & 0x01) {        // BTN1
    moveForward();
  } else if (p.buttons & 0x02) { // BTN2
    moveBackward();
  } else if (p.buttons & 0x04) { // BTN3
    turnLeft();
  } else if (p.buttons & 0x08) { // BTN4
    turnRight();
  } else {
    stopMotors();
  }
}

void handleJoystick(const ControlPayload &p) {
  const int center = 512;
  const int threshold = 120;

  int dx = p.joyX - center;
  int dy = p.joyY - center;

  if (abs(dx) < threshold && abs(dy) < threshold) {
    stopMotors();
    return;
  }

  if (abs(dy) > abs(dx)) {
    if (dy < 0) moveForward();   // assuming up is smaller value
    else        moveBackward();
  } else {
    if (dx < 0) turnLeft();
    else        turnRight();
  }
}

void handleTilt(const ControlPayload &p) {
  int16_t ax = p.accelX;
  int16_t ay = p.accelY;
  const int tiltThreshold = 2000; // adjust based on test

  if (abs(ax) < tiltThreshold && abs(ay) < tiltThreshold) {
    stopMotors();
    return;
  }

  if (abs(ay) > abs(ax)) {
    if (ay > 0) moveForward();
    else        moveBackward();
  } else {
    if (ax > 0) turnRight();
    else        turnLeft();
  }
}

void setup() {
  Serial.begin(9600);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // continue even if OLED not working
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  // Motors
  setupMotors();
  stopMotors();

  // NRF24
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&payload, sizeof(payload));

    switch (payload.mode) {
      case MODE_BUTTONS:
        handleButtons(payload);
        break;
      case MODE_JOYSTICK:
        handleJoystick(payload);
        break;
      case MODE_TILT:
        handleTilt(payload);
        break;
      default:
        stopMotors();
        break;
    }

    showStatus(payload);
  }
}
