#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <IRremote.h>

// ---------------- IR ----------------
#define IR_PIN 12

#define IR_FORWARD  0xE718FF00
#define IR_BACKWARD 0xAD52FF00
#define IR_LEFT     0xF708FF00
#define IR_RIGHT    0xA55AFF00
#define IR_STOP     0xE31CFF00

// ---------------- NRF24 ----------------
RF24 radio(A3, A2); // CE, CSN
const byte address[6] = "00001";

char command = 'S';

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);

  // IR
  IrReceiver.begin(IR_PIN);

  // NRF24
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

// ---------------- LOOP ----------------
void loop() {

  if (IrReceiver.decode()) {

    uint32_t code = IrReceiver.decodedIRData.decodedRawData;

    if (code == IR_FORWARD)      command = 'F';
    else if (code == IR_BACKWARD) command = 'B';
    else if (code == IR_LEFT)     command = 'L';
    else if (code == IR_RIGHT)    command = 'R';
    else if (code == IR_STOP)     command = 'S';

    radio.write(&command, sizeof(command));

    Serial.print("IR → CMD: ");
    Serial.println(command);

    IrReceiver.resume();
  }
}

