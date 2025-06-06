#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // mesmo mapeamento da imagem
int sensorPIR = 8;
int estadoMovimento = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(sensorPIR, INPUT);
  lcd.clear();
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  estadoMovimento = digitalRead(sensorPIR);

  if (estadoMovimento == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Movimento");
    lcd.setCursor(0, 1);
    lcd.print("detectado!");
    delay(2000);  // tempo da mensagem
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Aguardando...");
    delay(1000);  // para evitar atualizações constantes
  }
}