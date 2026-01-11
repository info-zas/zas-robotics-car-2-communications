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
  //backward();
  //delay(2000);
  //left();
  //delay(500);
  right();
  delay(1000);
  
}

void forward(){  //forword
digitalWrite(Left1, HIGH);  
digitalWrite(Left2, LOW);  
digitalWrite(Right1, HIGH);  
digitalWrite(Right2, LOW); 
}

void backward(){  //forword
digitalWrite(Left1, LOW);  
digitalWrite(Left2, HIGH);  
digitalWrite(Right1, LOW);  
digitalWrite(Right2, HIGH); 
}

void left(){ //turnLeft
digitalWrite(Left1, LOW);  
digitalWrite(Left2, HIGH);  
digitalWrite(Right1, HIGH);  
digitalWrite(Right2, LOW); 
}

void right(){ //turnLeft
digitalWrite(Left1, HIGH);  
digitalWrite(Left2, LOW);  
digitalWrite(Right1, LOW);  
digitalWrite(Right2, HIGH); 
}

