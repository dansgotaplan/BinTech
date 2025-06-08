//Tentativa no. 3
//Tempo gasto: 42min Taroba Σ;3

//==== MÁQUINA DE ESTADO (SM) ====
//1. Os estados são usados como switch no void loop(), pra identificar em que ponto da operação o usuário está.
//2. Cada estado tem funções específicas, favor indicar.
//3. "IDLE" = Fazendo nada, mostrando mensagem inicial.
//4. "LENDO" = Recebendo o código do usuário.
//5. "ATIVO" = Contando itens e atribuindo pontos.
//6. "FINAL" = Computando e exibindo resultados finais.

//==== ANOTAÇÕES ====
//1. Checar se as portas estão certinhas, e determinar as portas dos botões (BT1, BT2, BT3, BT4, SIM, NAO) e do buzzer (BUZZER_PIN).
//Nenhum desses acima está definido no código, mas estão chamados com esse nome.
//2. Ver o que fazer com o TRIG, ECHO, LED.
//3. Implementar as funções nos cases do switch
//4. Implementar o roteador no IDLE (pra transicionar pro codigo já lendo)
//5. Implementar o editor no LENDO

#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,4,5,6,7);
#define TRIG 11;
#define ECHO 10;
#define LED 12;

enum State{"IDLE", "LENDO", "ATIVO", "FINAL"};

struct Aluno {
    String codigo;
    String nome;
    String turma;
    int pontos;
};
Aluno aluno[] {
    {"12341", "Kauã Y", "3° TDS A", 0},
    {"11424", "V Daniel", "3° TDS A", 0},
    {"12234", "Kaio A", "3° TDS A", 0},
    {"11122", "Ingridy N", "3° TDS A", 0},
    {"12212", "João G", "3° TDS A", 0},
    {"22143", "Lorenna P", "3° TDS A", 0},
    {"22341", "Eduardo N", "3° TDS A", 0}
};

//==== FUNÇÕES GLOBAIS ====

void default() { //Volta as variáveis pro default
    String codigoInput = "";
    int indexAlunoAtual = -1; //Nenhum
    int pontosSessao = 0;
    State estado = "IDLE";
}

void beep(int tempo) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(tempo);
    digitalWrite(BUZZER_PIN, LOW);
}

//==== FUNÇÕES IDLE ====
void exibirMensagemInicial() {
    lcd.clear;
    lcd.setCursor(0,0);
    lcd.print("BinTech");
    lcd.setCursor(0,1);
    lcd.print("Inserir código");
}

//==== FUNÇÕES LENDO ====

//==== FUNÇÕES ATIVO ====

//==== FUNÇÕES FINAL ====

//==== SETUP E LOOP ==== (manter no final)
void setup() {
    Serial.begin(9600);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(LED, OUTPUT);
    
    lcd.begin(16,2);
    exibirMensagemInicial();
    default();
}

void loop() {
    switch(estado) {
        case "IDLE":
            exibirMensagemInicial();
            break;
        case "LENDO:
            //codeblock
            break;
        case "ATIVO":
            //codeblock
            break;
        case "FINAL":
            //codeblock
            break;
    }
}
