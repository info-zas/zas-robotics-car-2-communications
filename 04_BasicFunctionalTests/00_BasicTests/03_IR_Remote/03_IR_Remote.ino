#include <IRremote.hpp>
#define RightA 5
#define RightB 6
#define LeftA 9
#define LeftB 10

#define Left1 7
#define Left2 8
#define Left_Speed 6

#define Right1 3
#define Right2 4
#define Right_Speed 5


#define irPin 12

IRrecv irrecv(irPin);
decode_results results;

void forward(int speed) {
  analogWrite(RightA, speed);   
  analogWrite(RightB, LOW);  
  analogWrite(LeftA, speed); 
  analogWrite(LeftB, LOW);
}

void backward(int speed) {
  analogWrite(RightA, LOW);   
  analogWrite(RightB, speed);  
  analogWrite(LeftA, LOW); 
  analogWrite(LeftB, speed);
}

void left(int speed) {
  analogWrite(RightA, speed);   
  analogWrite(RightB, LOW);  
  analogWrite(LeftA, LOW); 
  analogWrite(LeftB, LOW);
}

void right(int speed) {
  analogWrite(RightA, LOW);   
  analogWrite(RightB, LOW);  
  analogWrite(LeftA, speed); 
  analogWrite(LeftB, LOW);
}

void stopCar() {
  analogWrite(RightA, LOW);   
  analogWrite(RightB, LOW);  
  analogWrite(LeftA, LOW); 
  analogWrite(LeftB, LOW);
}

void setup() {
  pinMode(RightA, OUTPUT); 
  pinMode(RightB, OUTPUT); 
  pinMode(LeftA, OUTPUT); 
  pinMode(LeftB, OUTPUT); 
  IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);
  Serial.begin(9600);
}

void loop(){

if (IrReceiver.decode()) 
  {
   unsigned long val = IrReceiver.decodedIRData.decodedRawData;
    Serial.println(val);
    
    if(val == 2907897600){
      forward(100);
    }

    if(val == 3810328320){
      stopCar();
    }

    if(val == 3877175040){
      backward(100);
    }

    IrReceiver.resume();
}

}
