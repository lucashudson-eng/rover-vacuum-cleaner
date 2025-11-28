// Pinos do TB6612FNG
#define PWMA 6
#define AIN2 7
#define AIN1 8
#define STBY 9
#define BIN1 10
#define BIN2 11
#define PWMB 12

void setup() {
  // Configura os pinos como saída
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Ativa o driver
  digitalWrite(STBY, HIGH);
}

void loop() {
  // Motores para frente
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMA, 255);
  analogWrite(PWMB, 255);
  delay(1000);

  // Parar
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  delay(1000);

  // Motores para trás
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, 255);
  analogWrite(PWMB, 255);
  delay(1000);

  // Parar
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  delay(1000);
}
