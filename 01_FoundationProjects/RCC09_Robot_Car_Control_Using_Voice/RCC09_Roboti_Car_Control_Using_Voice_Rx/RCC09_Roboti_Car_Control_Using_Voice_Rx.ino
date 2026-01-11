// ================= MOTOR PINS =================
#define IN1 4   // Left motor forward
#define IN2 3   // Left motor backward
#define ENA 5   // Left motor PWM

#define IN3 7   // Right motor forward
#define IN4 8   // Right motor backward
#define ENB 6   // Right motor PWM

char command;

// ================= SETUP =================
void setup() {
  Serial.begin(9600);   // HC-05 default baud rate

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopMotors();
}

// ================= LOOP =================
void loop() {
  if (Serial.available()) {
    command = Serial.read();
    Serial.println(command);

    switch (command) {
      case 'F': forward();  break;
      case 'B': backward(); break;
      case 'L': left();     break;
      case 'R': right();    break;
      case 'S': stopMotors(); break;
    }
  }
}

// ================= MOTOR FUNCTIONS =================
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 200);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 200);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 200);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 200);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

