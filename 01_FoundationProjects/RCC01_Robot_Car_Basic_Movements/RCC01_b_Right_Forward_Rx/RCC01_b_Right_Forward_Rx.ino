#define Left1 4
#define Left2 3
#define Left_Speed 5

#define Right1 7
#define Right2 8
#define Right_Speed 6

void setup() {
  pinMode(Left1, OUTPUT);
  pinMode(Left2,OUTPUT);
  pinMode(Left_Speed,OUTPUT); //pwm

  pinMode(Right1, OUTPUT);
  pinMode(Right2,OUTPUT);
  pinMode(Right_Speed,OUTPUT); //pwm

}

void loop() {
  analogWrite(Left_Speed,150);
  analogWrite(Right_Speed,150);

  forward();
  delay(2000);
  
}

void forward(){  //forword
digitalWrite(Left1, HIGH);  
digitalWrite(Left2, LOW);  
digitalWrite(Right1, HIGH);  
digitalWrite(Right2, LOW); 
}

