#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>

/* ========= RADIO ========= */
RF24 radio(A3, A2);          // CE, CSN
const byte address[6] = "00001";

/* ========= OLED ========= */
Adafruit_SSD1306 display(128, 64, &Wire);

/* ========= MPU6050 ========= */
MPU6050 mpu;
int16_t ax, ay, az;

/* ========= DATA STRUCT ========= */
struct Payload {
  int x;
  int y;
};

/* ========= THRESHOLDS ========= */
const int TILT_TH = 8000;     // Adjust after testing
const int CENTER  = 512;
const int HIGH_V  = 700;
const int LOW_V   = 300;

/* ========= SETUP ========= */
void setup() {
  Serial.begin(9600);

  // I2C + MPU
  Wire.begin();
  mpu.initialize();

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Radio (TRANSMITTER)
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

/* ========= LOOP ========= */
void loop() {
  Payload data;

  // Default = STOP
  data.x = CENTER;
  data.y = CENTER;
  String dir = "STOP";

  // Read accelerometer
  mpu.getAcceleration(&ax, &ay, &az);

  // Tilt mapping (hand-tilt control)
  if (ay > TILT_TH) {
    data.y = HIGH_V;
    dir = "FORWARD";
  }
  else if (ay < -TILT_TH) {
    data.y = LOW_V;
    dir = "BACKWARD";
  }
  else if (ax > TILT_TH) {
    data.x = HIGH_V;
    dir = "RIGHT";
  }
  else if (ax < -TILT_TH) {
    data.x = LOW_V;
    dir = "LEFT";
  }

  // Send data
  radio.write(&data, sizeof(data));

  // OLED display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("ACCEL TX");
  display.print("AX: "); display.println(ax);
  display.print("AY: "); display.println(ay);
  display.print("DIR: ");
  display.println(dir);
  display.display();

  delay(100);
}
