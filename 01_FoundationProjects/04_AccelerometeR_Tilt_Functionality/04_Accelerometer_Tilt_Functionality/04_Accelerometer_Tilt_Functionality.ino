#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>

MPU6050 mpu;

Adafruit_SSD1306 display(128, 64, &Wire);

int16_t ax, ay, az;

String getDirection(int x, int y) {
  int threshold = 8000; // adjust based on test

  if (y > threshold) return "FORWARD";
  if (y < -threshold) return "BACKWARD";
  if (x > threshold) return "RIGHT";
  if (x < -threshold) return "LEFT";

  return "LEVEL";
}

void setup() {
  Serial.begin(9600);

  Wire.begin();
  mpu.initialize();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
  mpu.getAcceleration(&ax, &ay, &az);

  String direction = getDirection(ax, ay);

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("ACCELEROMETER");
  display.print("AX: "); display.println(ax);
  display.print("AY: "); display.println(ay);
  display.println();
  display.print("DIR: ");
  display.println(direction);

  display.display();

  delay(100);
}
