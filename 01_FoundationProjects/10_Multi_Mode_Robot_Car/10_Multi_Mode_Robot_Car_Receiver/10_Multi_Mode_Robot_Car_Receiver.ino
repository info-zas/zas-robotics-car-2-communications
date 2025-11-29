/*******************************************************
 * LAB SESSION 15 – MULTI-MODE ROBOT CAR (FULL CODE)
 * Modes:
 * 1. JOYSTICK      (NRF24)
 * 2. TILT          (NRF24)
 * 3. IR REMOTE     (IR Receiver)
 * 4. BLUETOOTH     (HC-05)
 * 5. OBSTACLE      (Ultrasonic Auto Mode)
 *******************************************************/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>
#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= DISPLAY ======================
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ================= BLUETOOTH ====================
SoftwareSerial BT(10, 11); // RX, TX

// ================= NRF24 ========================
RF24 radio(9, 10);          // CE, CSN
const byte address[6] = "MOD15";

// ================= IR ===========================
#define IR_PIN 6
IRrecv irrecv(IR_PIN);
decode_results results;

// ================= ULTRASONIC ===================
#define TRIG 2
#define ECHO 3

// ================= MOTOR PINS ===================
#define Left1 7
#define Left2 8
#define Left_Speed 6
#define Right1 3
#define Right2 4
#define Right_Speed 5

// ================= MODES ========================
enum Mode {
  MODE_JOYSTICK = 1,
  MODE_TILT,
  MODE_IR,
  MODE_BLUETOOTH,
  MODE_OBSTACLE
};

int currentMode = MODE_JOYSTICK;

// ================= NRF24 Payload =================
struct Payload {
  int16_t joyX;
  int16_t joyY;
  int16_t accelX;
  int16_t accelY;
  uint8_t buttons;
  uint8_t mode;
};

Payload incoming;

// =================================================
// OLED Helper
// =================================================
void show(String title, String msg = "") {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println(title);
  display.setTextSize(1);
  display.println(msg);
  display.display();
}

// =================================================
// Motor Control
// =================================================
void setMotors(int leftSpeed, int rightSpeed) {

  // Left
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
  analogWrite(Left_Speed, abs(leftSpeed));

  // Right
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
  analogWrite(Right_Speed, abs(rightSpeed));
}

void stopCar()       { setMotors(0, 0); }
void moveForward()   { setMotors(200, 200); }
void moveBackward()  { setMotors(-200,-200); }
void turnLeft()      { setMotors(-150,150); }
void turnRight()     { setMotors(150,-150); }

// =================================================
// MODE 1: Joystick Mode (NRF24)
// =================================================
void modeJoystick() {
  if (radio.available()) {
    radio.read(&incoming, sizeof(incoming));

    int dx = incoming.joyX - 512;
    int dy = incoming.joyY - 512;
    int threshold = 120;

    if (abs(dx) < threshold && abs(dy) < threshold) {
      stopCar();
      show("Joystick","STOP");
      return;
    }

    // Determine dominant direction
    if (abs(dy) > abs(dx)) {
      if (dy < 0) { moveForward(); show("Joystick","FORWARD"); }
      else        { moveBackward(); show("Joystick","BACK"); }
    } else {
      if (dx < 0) { turnLeft(); show("Joystick","LEFT"); }
      else        { turnRight(); show("Joystick","RIGHT"); }
    }
  }
}

// =================================================
// MODE 2: Tilt Mode (NRF24)
// =================================================
void modeTilt() {
  if (radio.available()) {
    radio.read(&incoming, sizeof(incoming));

    int ax = incoming.accelX;
    int ay = incoming.accelY;
    int threshold = 2000;

    if (abs(ax) < threshold && abs(ay) < threshold) {
      stopCar();
      show("Tilt","LEVEL");
      return;
    }

    if (abs(ay) > abs(ax)) {
      if (ay > 0) { moveForward(); show("Tilt","FORWARD"); }
      else        { moveBackward(); show("Tilt","BACK"); }
    } else {
      if (ax > 0) { turnRight(); show("Tilt","RIGHT"); }
      else        { turnLeft(); show("Tilt","LEFT"); }
    }
  }
}

// =================================================
// MODE 3: IR Remote Mode
// =================================================
void modeIR() {
  if (irrecv.decode(&results)) {
    unsigned long code = results.value;

    if      (code == 0x00FF18E7) { moveForward(); show("IR","FORWARD"); }
    else if (code == 0x00FF4AB5) { moveBackward(); show("IR","BACK"); }
    else if (code == 0x00FF10EF) { turnLeft(); show("IR","LEFT"); }
    else if (code == 0x00FF5AA5) { turnRight(); show("IR","RIGHT"); }
    else if (code == 0x00FF38C7) { stopCar(); show("IR","STOP"); }

    irrecv.resume();
  }
}

// =================================================
// MODE 4: Bluetooth Mode
// =================================================
void modeBluetooth() {
  if (BT.available()) {
    char c = BT.read();

    if      (c=='F') { moveForward(); show("BT","FORWARD"); }
    else if (c=='B') { moveBackward(); show("BT","BACK"); }
    else if (c=='L') { turnLeft(); show("BT","LEFT"); }
    else if (c=='R') { turnRight(); show("BT","RIGHT"); }
    else if (c=='S') { stopCar(); show("BT","STOP"); }
  }
}

// =================================================
// MODE 5: Obstacle Avoidance Mode
// =================================================
long getDistance() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  return duration * 0.034 / 2; // cm
}

void modeObstacle() {
  long d = getDistance();

  if (d < 20) {
    stopCar();
    delay(200);
    turnLeft();
    show("Obstacle","AVOID");
    delay(400);
  } else {
    moveForward();
    show("Obstacle","FORWARD");
  }
}

// =================================================
// SETUP
// =================================================
void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  // Motor pins
  pinMode(Left1,OUTPUT);
  pinMode(Left2,OUTPUT);
  pinMode(Right1,OUTPUT);
  pinMode(Right2,OUTPUT);
  pinMode(Left_Speed,OUTPUT);
  pinMode(Right_Speed,OUTPUT);

  // IR
  irrecv.enableIRIn();

  // Ultrasonic
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.clearDisplay();

  // NRF24
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  show("MODE 1","Joystick");
}

// =================================================
// LOOP
// =================================================
void loop() {

  // Change mode: send '1' to '5' from Serial Monitor
  if (Serial.available()) {
    char m = Serial.read();
    if (m >= '1' && m <= '5') {
      currentMode = m - '0';
      show("MODE", String(currentMode));
      delay(300);
    }
  }

  switch(currentMode) {
    case MODE_JOYSTICK:   modeJoystick();   break;
    case MODE_TILT:       modeTilt();       break;
    case MODE_IR:         modeIR();         break;
    case MODE_BLUETOOTH:  modeBluetooth();  break;
    case MODE_OBSTACLE:   modeObstacle();   break;
  }
}
