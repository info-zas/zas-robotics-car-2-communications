#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Bluetooth on pins 10 (RX), 11 (TX)
SoftwareSerial BT(10, 11);

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Motor pins
#define Left1      7
#define Left2      8
#define Left_Speed 6
#define Right1     3
#define Right2     4
#define Right_Speed 5

int speedVal = 180;   // Default speed

void showOLED(String msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("BT CMD:");
  display.println(msg);
  display.display();
}

// Motor functions (same as previous labs)
void moveForward() {
  digitalWrite(Left1, HIGH); digitalWrite(Left2, LOW);
  digitalWrite(Right1, HIGH); digitalWrite(Right2, LOW);
  analogWrite(Left_Speed, speedVal);
  analogWrite(Right_Speed, speedVal);
}

void moveBackward() {
  digitalWrite(Left1, LOW); digitalWrite(Left2, HIGH);
  digitalWrite(Right1, LOW); digitalWrite(Right2, HIGH);
  analogWrite(Left_Speed, speedVal);
  analogWrite(Right_Speed, speedVal);
}

void turnLeft() {
  digitalWrite(Left1, LOW); digitalWrite(Left2, HIGH);
  digitalWrite(Right1, HIGH); digitalWrite(Right2, LOW);
  analogWrite(Left_Speed, speedVal);
  analogWrite(Right_Speed, speedVal);
}

void turnRight() {
  digitalWrite(Left1, HIGH); digitalWrite(Left2, LOW);
  digitalWrite(Right1, LOW); digitalWrite(Right2, HIGH);
  analogWrite(Left_Speed, speedVal);
  analogWrite(Right_Speed, speedVal);
}

void stopCar() {
  digitalWrite(Left1, LOW); digitalWrite(Left2, LOW);
  digitalWrite(Right1, LOW); digitalWrite(Right2, LOW);
  analogWrite(Left_Speed, 0);
  analogWrite(Right_Speed, 0);
}

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  // Motor pins
  pinMode(Left1, OUTPUT);
  pinMode(Left2, OUTPUT);
  pinMode(Right1, OUTPUT);
  pinMode(Right2, OUTPUT);
  pinMode(Left_Speed, OUTPUT);
  pinMode(Right_Speed, OUTPUT);

  // OLED init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  if (BT.available()) {
    String cmd = BT.readStringUntil('\n');
    cmd.trim();

    // Commands
    if (cmd == "F") moveForward();
    else if (cmd == "B") moveBackward();
    else if (cmd == "L") turnLeft();
    else if (cmd == "R") turnRight();
    else if (cmd == "S") stopCar();

    // Speed command: e.g., "V200"
    if (cmd.startsWith("V")) {
      int newSpeed = cmd.substring(1).toInt();
      if (newSpeed >= 0 && newSpeed <= 255) {
        speedVal = newSpeed;
      }
    }

    // Display command
    showOLED(cmd);
    Serial.println("BT Command: " + cmd);
  }
}
