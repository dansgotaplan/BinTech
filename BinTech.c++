/*
  EcoLixeira Inteligente - Sistema de Incentivo à Reciclagem
  Hackathon Arduino - Tema: VIDAS, ESCOLAS E COMUNIDADE
  ODS 12 - Consumo e Produção Responsáveis
  
  Funcionalidades:
  - Identificação de usuário via botões
  - Detecção de itens com sensor PIR
  - Sistema de pontuação (5 pontos por item)
  - Display com feedback ao usuário
  - Comunicação serial para extração de dados
*/

#include <LiquidCrystal.h>

// ========== CONFIGURAÇÃO DE HARDWARE ==========
// Pinos do Display LCD (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Pinos dos botões
#define BTN_1 6
#define BTN_2 7
#define BTN_3 8
#define BTN_VOLTAR 9
#define BTN_ENCERRAR 10
#define BTN_CONFIRMAR A0

// Pinos dos sensores e atuadores
#define PIR_SENSOR A1    // Sensor de movimento infravermelho
#define BUZZER A2        // Buzzer para feedback sonoro

// ========== ESTRUTURA DE DADOS ==========
struct Estudante {
  int id;
  char nome[20];
  char sala[10];
  int pontos;
};

// Array de estudantes cadastrados (máximo 20 estudantes)
#define MAX_ESTUDANTES 20
Estudante estudantes[MAX_ESTUDANTES] = {
  {123, "Ana Silva", "7A", 0},
  {456, "Bruno Costa", "7B", 0},
  {789, "Carla Santos", "8A", 0},
  {101, "Diego Ferreira", "8B", 0},
  {202, "Elena Oliveira", "9A", 0}
  // Adicione mais estudantes conforme necessário
};

int totalEstudantes = 5; // Número atual de estudantes cadastrados

// ========== VARIÁVEIS GLOBAIS ==========
// Estados do sistema
enum EstadoSistema {
  AGUARDANDO_ID,
  INSERINDO_ID,
  USUARIO_LOGADO,
  CONTANDO_ITENS,
  FINALIZANDO
};

EstadoSistema estadoAtual = AGUARDANDO_ID;

// Variáveis para controle de ID
String idDigitado = "";
int usuarioAtualIndex = -1;
int itensContados = 0;

// Variáveis para controle de tempo
unsigned long ultimoTempo = 0;
unsigned long tempoDebounce = 200; // 200ms para debounce dos botões
unsigned long tempoTimeout = 30000; // 30 segundos de timeout

// Variáveis para controle do sensor PIR
bool sensorAnterior = false;
unsigned long tempoUltimaDeteccao = 0;
#define INTERVALO_DETECCAO 1000 // 1 segundo entre detecções

// ========== CONFIGURAÇÃO INICIAL ==========
void setup() {
  // Inicializar comunicação serial
  Serial.begin(9600);
  Serial.println("=== EcoLixeira Inteligente Iniciada ===");
  
  // Configurar pinos dos botões como entrada com pull-up interno
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_VOLTAR, INPUT_PULLUP);
  pinMode(BTN_ENCERRAR, INPUT_PULLUP);
  pinMode(BTN_CONFIRMAR, INPUT_PULLUP);
  
  // Configurar pinos dos sensores e atuadores
  pinMode(PIR_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  
  // Inicializar display LCD
  lcd.begin(16, 2);
  lcd.clear();
  
  // Mostrar mensagem de boas-vindas
  mostrarTelaInicial();
  
  // Fazer bipe de inicialização
  bip(2);
  
  Serial.println("Sistema pronto para uso!");
}

// ========== LOOP PRINCIPAL ==========
void loop() {
  // Verificar entrada dos botões
  verificarBotoes();
  
  // Verificar sensor PIR se usuário estiver logado
  if (estadoAtual == USUARIO_LOGADO || estadoAtual == CONTANDO_ITENS) {
    verificarSensorPIR();
  }
  
  // Verificar timeout
  verificarTimeout();
  
  // Pequeno delay para estabilidade
  delay(50);
}

// ========== FUNÇÕES DE CONTROLE DE BOTÕES ==========
void verificarBotoes() {
  static unsigned long ultimoBotao = 0;
  
  // Debounce simples
  if (millis() - ultimoBotao < tempoDebounce) {
    return;
  }
  
  // Verificar cada botão
  if (digitalRead(BTN_1) == LOW) {
    processarBotao('1');
    ultimoBotao = millis();
  }
  else if (digitalRead(BTN_2) == LOW) {
    processarBotao('2');
    ultimoBotao = millis();
  }
  else if (digitalRead(BTN_3) == LOW) {
    processarBotao('3');
    ultimoBotao = millis();
  }
  else if (digitalRead(BTN_VOLTAR) == LOW) {
    processarBotao('V');
    ultimoBotao = millis();
  }
  else if (digitalRead(BTN_ENCERRAR) == LOW) {
    processarBotao('E');
    ultimoBotao = millis();
  }
  else if (digitalRead(BTN_CONFIRMAR) == LOW) {
    processarBotao('C');
    ultimoBotao = millis();
  }
}

// Processar ação do botão baseado no estado atual
void processarBotao(char botao) {
  bip(1); // Feedback sonoro
  
  switch (estadoAtual) {
    case AGUARDANDO_ID:
      if (botao >= '1' && botao <= '3') {
        iniciarInsercaoID();
        idDigitado = String(botao);
        atualizarDisplayID();
      }
      break;
      
    case INSERINDO_ID:
      if (botao >= '1' && botao <= '3') {
        if (idDigitado.length() < 3) { // Máximo 3 dígitos
          idDigitado += String(botao);
          atualizarDisplayID();
        }
      }
      else if (botao == 'V') { // Voltar - apagar último dígito
        if (idDigitado.length() > 0) {
          idDigitado = idDigitado.substring(0, idDigitado.length() - 1);
          if (idDigitado.length() == 0) {
            voltarTelaInicial();
          } else {
            atualizarDisplayID();
          }
        }
      }
      else if (botao == 'C') { // Confirmar ID
        verificarUsuario();
      }
      break;
      
    case USUARIO_LOGADO:
    case CONTANDO_ITENS:
      if (botao == 'E') { // Encerrar sessão
        finalizarSessao();
      }
      break;
      
    case FINALIZANDO:
      // Qualquer botão volta para tela inicial
      voltarTelaInicial();
      break;
  }
  
  ultimoTempo = millis(); // Resetar timeout
}

// ========== FUNÇÕES DE CONTROLE DO SENSOR PIR ==========
void verificarSensorPIR() {
  bool sensorAtual = digitalRead(PIR_SENSOR);
  
  // Detectar transição de LOW para HIGH (movimento detectado)
  if (sensorAtual && !sensorAnterior) {
    // Verificar se passou tempo suficiente desde última detecção
    if (millis() - tempoUltimaDeteccao > INTERVALO_DETECCAO) {
      itemDetectado();
      tempoUltimaDeteccao = millis();
    }
  }
  
  sensorAnterior = sensorAtual;
}

// Processar item detectado pelo sensor
void itemDetectado() {
  itensContados++;
  estudantes[usuarioAtualIndex].pontos += 5; // 5 pontos por item
  
  estadoAtual = CONTANDO_ITENS;
  
  // Feedback sonoro
  bip(1);
  
  // Atualizar display
  atualizarDisplayContagem();
  
  // Log serial
  Serial.print("Item detectado! Usuário: ");
  Serial.print(estudantes[usuarioAtualIndex].nome);
  Serial.print(" - Itens: ");
  Serial.print(itensContados);
  Serial.print(" - Pontos totais: ");
  Serial.println(estudantes[usuarioAtualIndex].pontos);
  
  ultimoTempo = millis(); // Resetar timeout
}

// ========== FUNÇÕES DE GERENCIAMENTO DE USUÁRIO ==========
void verificarUsuario() {
  int id = idDigitado.toInt();
  usuarioAtualIndex = -1;
  
  // Buscar usuário no array
  for (int i = 0; i < totalEstudantes; i++) {
    if (estudantes[i].id == id) {
      usuarioAtualIndex = i;
      break;
    }
  }
  
  if (usuarioAtualIndex != -1) {
    // Usuário encontrado
    estadoAtual = USUARIO_LOGADO;
    itensContados = 0;
    mostrarUsuarioLogado();
    
    Serial.print("Usuário logado: ");
    Serial.print(estudantes[usuarioAtualIndex].nome);
    Serial.print(" (ID: ");
    Serial.print(estudantes[usuarioAtualIndex].id);
    Serial.println(")");
  } else {
    // Usuário não encontrado
    mostrarUsuarioNaoEncontrado();
    delay(2000);
    voltarTelaInicial();
  }
}

void finalizarSessao() {
  estadoAtual = FINALIZANDO;
  mostrarFinalizacao();
  
  // Log da sessão completa
  Serial.println("=== SESSÃO FINALIZADA ===");
  Serial.print("Usuário: ");
  Serial.println(estudantes[usuarioAtualIndex].nome);
  Serial.print("Sala: ");
  Serial.println(estudantes[usuarioAtualIndex].sala);
  Serial.print("Itens reciclados nesta sessão: ");
  Serial.println(itensContados);
  Serial.print("Pontos ganhos nesta sessão: ");
  Serial.println(itensContados * 5);
  Serial.print("Total de pontos do usuário: ");
  Serial.println(estudantes[usuarioAtualIndex].pontos);
  Serial.println("==========================");
  
  bip(3); // Bipe de finalização
  
  // Resetar variáveis
  idDigitado = "";
  usuarioAtualIndex = -1;
  itensContados = 0;
}

// ========== FUNÇÕES DE TIMEOUT ==========
void verificarTimeout() {
  if (millis() - ultimoTempo > tempoTimeout) {
    if (estadoAtual != AGUARDANDO_ID && estadoAtual != FINALIZANDO) {
      Serial.println("Timeout - voltando para tela inicial");
      voltarTelaInicial();
    }
  }
}

// ========== FUNÇÕES DE DISPLAY ==========
void mostrarTelaInicial() {
  estadoAtual = AGUARDANDO_ID;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EcoLixeira Smart");
  lcd.setCursor(0, 1);
  lcd.print("Digite seu ID...");
  ultimoTempo = millis();
}

void iniciarInsercaoID() {
  estadoAtual = INSERINDO_ID;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite ID:");
}

void atualizarDisplayID() {
  lcd.setCursor(0, 1);
  lcd.print("ID: ");
  lcd.print(idDigitado);
  
  // Limpar resto da linha
  for (int i = idDigitado.length() + 4; i < 16; i++) {
    lcd.print(" ");
  }
}

void mostrarUsuarioLogado() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ola, ");
  lcd.print(estudantes[usuarioAtualIndex].nome);
  lcd.setCursor(0, 1);
  lcd.print("Pode reciclar!");
}

void atualizarDisplayContagem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Itens: ");
  lcd.print(itensContados);
  lcd.print(" Pts: ");
  lcd.print(estudantes[usuarioAtualIndex].pontos);
  lcd.setCursor(0, 1);
  lcd.print("Continue ou [E]");
}

void mostrarUsuarioNaoEncontrado() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ID nao");
  lcd.setCursor(0, 1);
  lcd.print("encontrado!");
}

void mostrarFinalizacao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Finalizado!");
  lcd.setCursor(0, 1);
  lcd.print("Obrigado!");
  
  // Mostrar por 3 segundos e depois voltar
  delay(3000);
  voltarTelaInicial();
}

void voltarTelaInicial() {
  idDigitado = "";
  usuarioAtualIndex = -1;
  itensContados = 0;
  mostrarTelaInicial();
}

// ========== FUNÇÕES DE FEEDBACK SONORO ==========
void bip(int quantidade) {
  for (int i = 0; i < quantidade; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    if (i < quantidade - 1) {
      delay(100);
    }
  }
}

// ========== FUNÇÕES DE COMUNICAÇÃO SERIAL ==========
void serialEvent() {
  // Função chamada automaticamente quando há dados na serial
  while (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    
    if (comando == "EXPORT_DATA") {
      exportarDados();
    }
    else if (comando == "RESET_POINTS") {
      resetarPontos();
    }
    else if (comando == "STATUS") {
      mostrarStatus();
    }
    else if (comando.startsWith("ADD_STUDENT:")) {
      // Formato: ADD_STUDENT:ID,NOME,SALA
      adicionarEstudante(comando);
    }
  }
}

// Exportar todos os dados dos estudantes via serial
void exportarDados() {
  Serial.println("=== EXPORTAÇÃO DE DADOS ===");
  Serial.println("ID,NOME,SALA,PONTOS");
  
  for (int i = 0; i < totalEstudantes; i++) {
    Serial.print(estudantes[i].id);
    Serial.print(",");
    Serial.print(estudantes[i].nome);
    Serial.print(",");
    Serial.print(estudantes[i].sala);
    Serial.print(",");
    Serial.println(estudantes[i].pontos);
  }
  
  Serial.println("=== FIM DA EXPORTAÇÃO ===");
}

// Resetar pontos de todos os estudantes
void resetarPontos() {
  for (int i = 0; i < totalEstudantes; i++) {
    estudantes[i].pontos = 0;
  }
  Serial.println("Pontos resetados para todos os estudantes!");
}

// Mostrar status atual do sistema
void mostrarStatus() {
  Serial.println("=== STATUS DO SISTEMA ===");
  Serial.print("Estado atual: ");
  
  switch (estadoAtual) {
    case AGUARDANDO_ID:
      Serial.println("Aguardando ID");
      break;
    case INSERINDO_ID:
      Serial.println("Inserindo ID");
      break;
    case USUARIO_LOGADO:
      Serial.println("Usuario logado");
      break;
    case CONTANDO_ITENS:
      Serial.println("Contando itens");
      break;
    case FINALIZANDO:
      Serial.println("Finalizando");
      break;
  }
  
  Serial.print("Total de estudantes: ");
  Serial.println(totalEstudantes);
  
  if (usuarioAtualIndex != -1) {
    Serial.print("Usuario atual: ");
    Serial.println(estudantes[usuarioAtualIndex].nome);
    Serial.print("Itens nesta sessao: ");
    Serial.println(itensContados);
  }
  
  Serial.println("=========================");
}

// Adicionar novo estudante via serial
void adicionarEstudante(String comando) {
  if (totalEstudantes >= MAX_ESTUDANTES) {
    Serial.println("ERRO: Máximo de estudantes atingido!");
    return;
  }
  
  // Extrair dados do comando
  int primeira_virgula = comando.indexOf(',');
  int segunda_virgula = comando.indexOf(',', primeira_virgula + 1);
  
  if (primeira_virgula == -1 || segunda_virgula == -1) {
    Serial.println("ERRO: Formato inválido. Use: ADD_STUDENT:ID,NOME,SALA");
    return;
  }
  
  String idStr = comando.substring(12, primeira_virgula); // Após "ADD_STUDENT:"
  String nome = comando.substring(primeira_virgula + 1, segunda_virgula);
  String sala = comando.substring(segunda_virgula + 1);
  
  int id = idStr.toInt();
  
  // Verificar se ID já existe
  for (int i = 0; i < totalEstudantes; i++) {
    if (estudantes[i].id == id) {
      Serial.println("ERRO: ID já existe!");
      return;
    }
  }
  
  // Adicionar novo estudante
  estudantes[totalEstudantes].id = id;
  nome.toCharArray(estudantes[totalEstudantes].nome, 20);
  sala.toCharArray(estudantes[totalEstudantes].sala, 10);
  estudantes[totalEstudantes].pontos = 0;
  
  totalEstudantes++;
  
  Serial.print("Estudante adicionado: ");
  Serial.print(nome);
  Serial.print(" (ID: ");
  Serial.print(id);
  Serial.println(")");
}