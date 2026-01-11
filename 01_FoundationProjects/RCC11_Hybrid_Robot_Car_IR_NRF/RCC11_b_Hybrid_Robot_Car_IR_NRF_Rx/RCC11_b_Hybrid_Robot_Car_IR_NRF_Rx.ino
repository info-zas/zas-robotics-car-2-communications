#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ---------------- NRF24 ----------------
RF24 radio(A3, A2); // CE, CSN
const byte address[6] = "00001";

// ---------------- MOTOR PINS ----------------
#define IN1 4
#define IN2 3
#define ENA 5

#define IN3 7
#define IN4 8
#define ENB 6

char command = 'S';

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);

  // Motors
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  stopMotors();

  // NRF24
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

// ---------------- LOOP ----------------
void loop() {

  if (radio.available()) {
    radio.read(&command, sizeof(command));
    handleCommand(command);

    Serial.print("RX CMD: ");
    Serial.println(command);
  }
}

// ---------------- COMMAND HANDLER ----------------
void handleCommand(char c) {
  switch (c) {
    case 'F': forward(); break;
    case 'B': backward(); break;
    case 'L': left(); break;
    case 'R': right(); break;
    default: stopMotors(); break;
  }
}

// ---------------- MOTOR FUNCTIONS ----------------
void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); analogWrite(ENA, 200);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); analogWrite(ENB, 200);
}

void backward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); analogWrite(ENA, 200);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); analogWrite(ENB, 200);
}

void left() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); analogWrite(ENA, 200);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); analogWrite(ENB, 200);
}

void right() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); analogWrite(ENA, 200);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); analogWrite(ENB, 200);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); analogWrite(ENA, 0);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); analogWrite(ENB, 0);
}

