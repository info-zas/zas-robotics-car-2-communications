#include <IRremote.h>

#define IR_PIN 12

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_PIN);
  Serial.println("IR Test Ready");
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("Code: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.resume();
  }
}

