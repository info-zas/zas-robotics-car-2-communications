#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// IR Receiver pin
#define IR_PIN 11

// Motor pins
#define Left1      7
#define Left2      8
#define Left_Speed 6
#define Right1     3
#define Right2     4
#define Right_Speed 5

// OLED
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Create IR receiver object
IRrecv irrecv(IR_PIN);
decode_results results;

// IR Remote button codes (NEC format example)
#define IR_UP     0x00FF18E7
#define IR_DOWN   0x00FF4AB5
#define IR_LEFT   0x00FF10EF
#define IR_RIGHT  0x00FF5AA5
#define IR_OK     0x00FF38C7

void showDisplay(String msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(msg);
  display.display();
}

// Motor control helpers
void moveForward() {
  digitalWrite(Left1, HIGH);
  digitalWrite(Left2, LOW);
  digitalWrite(Right1, HIGH);
  digitalWrite(Right2, LOW);
  analogWrite(Left_Speed, 200);
  analogWrite(Right_Speed, 200);
  showDisplay("FORWARD");
}

void moveBackward() {
  digitalWrite(Left1, LOW);
  digitalWrite(Left2, HIGH);
  digitalWrite(Right1, LOW);
  digitalWrite(Right2, HIGH);
  analogWrite(Left_Speed, 200);
  analogWrite(Right_Speed, 200);
  showDisplay("BACK");
}

void turnLeft() {
  digitalWrite(Left1, LOW);
  digitalWrite(Left2, HIGH);
  digitalWrite(Right1, HIGH);
  digitalWrite(Right2, LOW);
  analogWrite(Left_Speed, 180);
  analogWrite(Right_Speed, 180);
  showDisplay("LEFT");
}

void turnRight() {
  digitalWrite(Left1, HIGH);
  digitalWrite(Left2, LOW);
  digitalWrite(Right1, LOW);
  digitalWrite(Right2, HIGH);
  analogWrite(Left_Speed, 180);
  analogWrite(Right_Speed, 180);
  showDisplay("RIGHT");
}

void stopCar() {
  digitalWrite(Left1, LOW);
  digitalWrite(Left2, LOW);
  digitalWrite(Right1, LOW);
  digitalWrite(Right2, LOW);
  analogWrite(Left_Speed, 0);
  analogWrite(Right_Speed, 0);
  showDisplay("STOP");
}

void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(Left1, OUTPUT);
  pinMode(Left2, OUTPUT);
  pinMode(Right1, OUTPUT);
  pinMode(Right2, OUTPUT);
  pinMode(Left_Speed, OUTPUT);
  pinMode(Right_Speed, OUTPUT);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // IR receiver
  irrecv.enableIRIn();
  showDisplay("IR READY");
}

void loop() {

  if (irrecv.decode(&results)) {
    unsigned long code = results.value;
    Serial.println(code, HEX);

    if      (code == IR_UP)    moveForward();
    else if (code == IR_DOWN)  moveBackward();
    else if (code == IR_LEFT)  turnLeft();
    else if (code == IR_RIGHT) turnRight();
    else if (code == IR_OK)    stopCar();

    irrecv.resume();
  }
}
