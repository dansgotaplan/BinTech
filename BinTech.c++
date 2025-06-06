// #include <LiquidCrystal.h>
// 
// // Pinos do display: rs, en, d4, d5, d6, d7
// LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// int sensorPIR = 10;
// int estadoMovimento = 0;
// 
// 
// void setup() {
//   lcd.begin(16, 2);
//   pinMode(sensorPIR, INPUT);
//   lcd.clear();
//   lcd.print("Iniciando...");
//   delay(2000);
//   lcd.clear();
// }
// 
// 
// void loop() {
//   estadoMovimento = digitalRead(sensorPIR);
// 
//   if (estadoMovimento == HIGH) {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Movimento");
//     lcd.setCursor(0, 1);
//     lcd.print("detectado!");
//     delay(1000);  // tempo da mensagem
//   } else {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Aguardando...");
//     delay(1000);  // para evitar atualizações constantes
//   }
// }


#define TRIG 11
#define ECHO 10
#define LED 12

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  lcd.clear();
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  long duracao;
  float distancia;

  digitalWrite(TRIG, LOW);
  delayMicroseconds(0.1);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(1);
  digitalWrite(TRIG, LOW);

  duracao = pulseIn(ECHO, HIGH);
  distancia = duracao * 0.034 / 2;

  Serial.print("Distância: ");
  Serial.print(distancia);
  Serial.println(" cm");

  if (distancia < 30) {
    digitalWrite(LED, HIGH); // Acende o LED
    
  } else {
    digitalWrite(LED, LOW); // Apaga o LED
  }

  delay(200);
}