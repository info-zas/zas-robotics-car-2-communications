#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ========= RADIO ========= */
RF24 radio(A3, A2);        // CE, CSN
const byte address[6] = "00001";

/* ========= OLED ========= */
Adafruit_SSD1306 display(128, 64, &Wire);

/* ========= MOTOR PINS ========= */
#define Left1 4
#define Left2 3
#define Left_Speed 5

#define Right1 7
#define Right2 8
#define Right_Speed 6

/* ========= DATA STRUCT ========= */
struct Payload {
  int x;
  int y;
};

Payload incoming;

/* ========= THRESHOLDS ========= */
const int CENTER_MIN = 450;
const int CENTER_MAX = 550;
const int SPEED = 150;

/* ========= SETUP ========= */
void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(Left1, OUTPUT);
  pinMode(Left2, OUTPUT);
  pinMode(Left_Speed, OUTPUT);
  pinMode(Right1, OUTPUT);
  pinMode(Right2, OUTPUT);
  pinMode(Right_Speed, OUTPUT);

  analogWrite(Left_Speed, SPEED);
  analogWrite(Right_Speed, SPEED);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Radio
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

/* ========= LOOP ========= */
void loop() {

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("CAR RECEIVER");

  if (radio.available()) {
    radio.read(&incoming, sizeof(incoming));

    display.print("X: "); display.println(incoming.x);
    display.print("Y: "); display.println(incoming.y);

    controlCar(incoming.x, incoming.y);
  } 
  //else {
  //  stopCar();
  //  display.println("No Signal");
  //}

  display.display();
  delay(100);
}

/* ========= CAR CONTROL LOGIC ========= */
void controlCar(int x, int y) {

  if (y > 600) {
    forward();
    display.println("FORWARD");
  }
  else if (y < 400) {
    backward();
    display.println("BACKWARD");
  }
  else if (x > 600) {
    right();
    display.println("RIGHT");
  }
  else if (x < 400) {
    left();
    display.println("LEFT");
  }
  else {
    stopCar();
    display.println("STOP");
  }
}

/* ========= MOTOR FUNCTIONS ========= */
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

void stopCar() {
  digitalWrite(Left1, LOW);
  digitalWrite(Left2, LOW);
  digitalWrite(Right1, LOW);
  digitalWrite(Right2, LOW);
}

