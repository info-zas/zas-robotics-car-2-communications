#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define JOY_X A0
#define JOY_Y A1

Adafruit_SSD1306 display(128, 64, &Wire);

int xValue, yValue;

String directionText(int x, int y) {
  int centerMin = 450;
  int centerMax = 550;

  if (y > 600) return "FORWARD";
  if (y < 400) return "BACKWARD";
  if (x > 600) return "RIGHT";
  if (x < 400) return "LEFT";

  return "CENTER";
}

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
}

void loop() {
  xValue = analogRead(JOY_X);
  yValue = analogRead(JOY_Y);

  String dir = directionText(xValue, yValue);

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("JOYSTICK VALUES");
  display.print("X: "); display.println(xValue);
  display.print("Y: "); display.println(yValue);

  display.println();
  display.print("DIR: ");
  display.println(dir);

  display.display();

  delay(50);
}
