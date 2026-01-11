#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= IR =================
#define IR_PIN 12

// ================= MOTOR PINS =================
#define Left1 4
#define Left2 3
#define Left_Speed 5

#define Right1 7
#define Right2 8
#define Right_Speed 6

// ================= IR CODES =================
#define IR_FORWARD  0xE718FF00
#define IR_BACKWARD 0xAD52FF00
#define IR_LEFT     0xF708FF00
#define IR_RIGHT    0xA55AFF00
#define IR_STOP     0xE31CFF00

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(OLED_RESET);

// ================= SETUP =================
void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_PIN);

  pinMode(Left1, OUTPUT);
  pinMode(Left2, OUTPUT);
  pinMode(Left_Speed, OUTPUT);
  pinMode(Right1, OUTPUT);
  pinMode(Right2, OUTPUT);
  pinMode(Right_Speed, OUTPUT);

  analogWrite(Left_Speed, 150);
  analogWrite(Right_Speed, 150);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("IR READY");
  display.display();

  stopMotors();
}

// ================= LOOP =================
void loop() {

  if (IrReceiver.decode()) {

    uint32_t code = IrReceiver.decodedIRData.decodedRawData;

    Serial.print("IR Code: 0x");
    Serial.println(code, HEX);

    if (code == IR_FORWARD) {
      forward();
      showOLED("FORWARD");
    }
    else if (code == IR_BACKWARD) {
      backward();
      showOLED("BACKWARD");
    }
    else if (code == IR_LEFT) {
      left();
      showOLED("LEFT");
    }
    else if (code == IR_RIGHT) {
      right();
      showOLED("RIGHT");
    }
    else if (code == IR_STOP) {
      stopMotors();
      showOLED("STOP");
    }

    IrReceiver.resume();
  }
}

// ================= OLED FUNCTION =================
void showOLED(const char* msg) {
  display.clearDisplay();
  display.setCursor(0, 00);
  display.println(msg);
  display.display();
}

// ================= MOTOR FUNCTIONS =================
void forward() {
  digitalWrite(Left1, HIGH);
  digitalWrite(Left2, LOW);
  digitalWrite(Right1, HIGH);
  digitalWrite(Right2, LOW);
}

void backward() {
  digitalWrite(Left1, LOW);
  digitalWrite(Left2, HIGH);
  digitalWrite(Right1, LOW);
  digitalWrite(Right2, HIGH);
}

void left() {
  digitalWrite(Left1, LOW);
  digitalWrite(Left2, HIGH);
  digitalWrite(Right1, HIGH);
  digitalWrite(Right2, LOW);
}

void right() {
  digitalWrite(Left1, HIGH);
  digitalWrite(Left2, LOW);
  digitalWrite(Right1, LOW);
  digitalWrite(Right2, HIGH);
}

void stopMotors() {
  digitalWrite(Left1, LOW);
  digitalWrite(Left2, LOW);
  digitalWrite(Right1, LOW);
  digitalWrite(Right2, LOW);
}
