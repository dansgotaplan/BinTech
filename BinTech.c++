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

// vv Alunos vv
struct Aluno {
  String codigo;
  String nome;
  String turma;
  int pontos;
};
Aluno alunos[7] = {
  {"12341", "Kauã", "3° TDS A", 0},
  {"11424", "Daniel", "3° TDS A", 0},
  {"12234", "Kaio", "3° TDS A", 0},
  {"11122", "Ingridy", "3° TDS A", 0},
  {"12212", "Joãozinho", "3° TDS A", 0},
  {"22143", "Lorenna", "3° TDS A", 0},
  {"22341", "Norinha", "3° TDS A", 0}
};

// vv Variáveis vv
String codigoInput = "";
int alunoAtual = -1; //Isso aqui é o índice do aluno na lista (^^). -1 é nenhum aluno selecionado
int pontosSessao = 0; //Pontos na sessão atual
bool sessaoAtiva = False;


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
