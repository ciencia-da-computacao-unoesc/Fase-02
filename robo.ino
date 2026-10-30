/*
 * PROJETO: Robô Autônomo e Bluetooth (Dual Mode)
 * PLATAFORMA: Arduino Uno / Nano
 * COMPONENTES: L298N, 2x Motores TT, HC-SR04, Servo 9g, Sensor IR Frontal, HC-05, LCD 16x2 I2C
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// =========================================================================
// 1. DEFINIÇÃO DE PINOS
// =========================================================================

// Ponte H L298N (Pinos 5 e 6 usam Timer0, evitando conflito com a Servo.h no Timer1)
const int PINO_ENA = 5;  // PWM Velocidade Motor Esquerdo
const int PINO_IN1 = 4;  // Direção Motor Esquerdo
const int PINO_IN2 = 7;  // Direção Motor Esquerdo
const int PINO_IN3 = 8;  // Direção Motor Direito
const int PINO_IN4 = 12; // Direção Motor Direito
const int PINO_ENB = 6;  // PWM Velocidade Motor Direito

// Sensores e Atuadores
const int PINO_SERVO = 9;
const int PINO_TRIG  = 10;
const int PINO_ECHO  = 11;
const int PINO_IR    = 2; // Sensor Infravermelho (LOW = Obstáculo detectado)

// Bluetooth HC-05 (RX = Pino 3, TX = Pino 13)
const int PINO_BT_RX = 3;
const int PINO_BT_TX = 13;

// =========================================================================
// 2. CONSTANTES DE CALIBRAÇÃO E AJUSTE FINO
// =========================================================================
const int VELOCIDADE_RETA  = 170; // 0 a 255 (Ajuste conforme a tensão da sua bateria)
const int VELOCIDADE_CURVA = 200; // Mais força para vencer o atrito no giro
const int DISTANCIA_MINIMA = 25;  // Distância em cm para acionar o desvio

// Ângulos do Servo Motor (Ajuste se o seu servo ficar levemente desalinhado)
const int ANGULO_CENTRO   = 90;
const int ANGULO_DIREITA  = 20;
const int ANGULO_ESQUERDA = 160;

// =========================================================================
// 3. OBJETOS E VARIÁVEIS GLOBAIS
// =========================================================================
LiquidCrystal_I2C lcd(0x27, 16, 2); // Se não exibir texto, teste o endereço 0x3F
Servo radarServo;
SoftwareSerial bluetooth(PINO_BT_RX, PINO_BT_TX);

bool modoAutonomo = false; // Inicia em Modo Manual por segurança
char ultimoComandoBT = 'S';
unsigned long ultimaAtualizacaoDisplay = 0;

// =========================================================================
// 4. CONFIGURAÇÃO INICIAL (SETUP)
// =========================================================================
void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  // Configuração dos pinos da Ponte H
  pinMode(PINO_ENA, OUTPUT);
  pinMode(PINO_ENB, OUTPUT);
  pinMode(PINO_IN1, OUTPUT);
  pinMode(PINO_IN2, OUTPUT);
  pinMode(PINO_IN3, OUTPUT);
  pinMode(PINO_IN4, OUTPUT);

  // Configuração dos Sensores
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  pinMode(PINO_IR, INPUT);

  // Garante motores parados na inicialização
  pararMotores();

  // Inicializa e centraliza o Servo
  radarServo.attach(PINO_SERVO);
  radarServo.write(ANGULO_CENTRO);

  // Inicializa o Display LCD I2C
  lcd.init();
  lcd.backlight();
  atualizarDisplay("SISTEMA PRONTO", "MODO: MANUAL");
  delay(1000);
}

// =========================================================================
// 5. LOOP PRINCIPAL
// =========================================================================
void loop() {
  // 1. Verifica constantemente se chegaram comandos via Bluetooth
  verificarBluetooth();

  // 2. Executa a máquina de estados conforme o modo selecionado
  if (modoAutonomo) {
    executarModoAutonomo();
  } else {
    executarModoManual();
  }
}

// =========================================================================
// 6. LEITURA BLUETOOTH E CONTROLE MANUAL
// =========================================================================
void verificarBluetooth() {
  while (bluetooth.available() > 0) {
    char cmd = bluetooth.read();

    // Ignora quebras de linha enviadas por alguns aplicativos
    if (cmd == '\n' || cmd == '\r') continue;

    // Comandos de troca de modo
    if (cmd == 'A' || cmd == 'a' || cmd == 'X') {
      modoAutonomo = true;
      pararMotores();
      radarServo.write(ANGULO_CENTRO);
      atualizarDisplay("MODO: AUTONOMO", "Iniciando...");
    } 
    else if (cmd == 'M' || cmd == 'm' || cmd == 'x') {
      modoAutonomo = false;
      ultimoComandoBT = 'S';
      pararMotores();
      radarServo.write(ANGULO_CENTRO);
      atualizarDisplay("MODO: MANUAL", "Aguardando cmd");
    } 
    else if (!modoAutonomo) {
      // Se estiver no modo manual, registra o comando de direção
      ultimoComandoBT = toupper(cmd);
    }
  }
}

void executarModoManual() {
  bool irDetectou = (digitalRead(PINO_IR) == LOW);

  switch (ultimoComandoBT) {
    case 'F':
      // Proteção opcional: impede colisão frontal se o IR detectar algo colado ao chassi
      if (irDetectou) {
        pararMotores();
        atualizarDisplayPeriodico("MODO: MANUAL", "BLOQUEIO IR!");
      } else {
        moverFrente(VELOCIDADE_RETA);
        atualizarDisplayPeriodico("MODO: MANUAL", "DIR: FRENTE");
      }
      break;

    case 'B':
      moverTras(VELOCIDADE_RETA);
      atualizarDisplayPeriodico("MODO: MANUAL", "DIR: RE");
      break;

    case 'L':
      girarEsquerda(VELOCIDADE_CURVA);
      atualizarDisplayPeriodico("MODO: MANUAL", "DIR: ESQUERDA");
      break;

    case 'R':
      girarDireita(VELOCIDADE_CURVA);
      atualizarDisplayPeriodico("MODO: MANUAL", "DIR: DIREITA");
      break;

    case 'S':
    default:
      pararMotores();
      atualizarDisplayPeriodico("MODO: MANUAL", "PARADO");
      break;
  }
}

// =========================================================================
// 7. LÓGICA DE NAVEGAÇÃO AUTÔNOMA (RADAR + SENSOR IR)
// =========================================================================
void executarModoAutonomo() {
  int distanciaFrente = lerDistanciaCM();
  bool obstaculoIR = (digitalRead(PINO_IR) == LOW); // Maioria dos sensores IR ativa em LOW

  // Verifica se há bloqueio pelo Ultrassônico OU pelo Sensor IR Frontal
  if (distanciaFrente <= DISTANCIA_MINIMA || obstaculoIR) {
    pararMotores();

    if (obstaculoIR) {
      atualizarDisplay("ALERTA IR!", "Obstaculo Perto");
    } else {
      atualizarDisplay("OBSTACULO A:", String(distanciaFrente) + " cm");
    }

    if (!aguardarComEscutaBT(250)) return; // Se mudou para Manual no celular, aborta manobra

    // Dá uma leve ré para abrir espaço de manobra
    atualizarDisplay("MANOBRA:", "Recuando...");
    moverTras(VELOCIDADE_RETA);
    if (!aguardarComEscutaBT(350)) return;
    pararMotores();
    if (!aguardarComEscutaBT(200)) return;

    // Varredura do Radar: Olha para a Direita
    atualizarDisplay("RADAR:", "Olhando Dir...");
    radarServo.write(ANGULO_DIREITA);
    if (!aguardarComEscutaBT(400)) return;
    int distDireita = lerDistanciaCM();

    // Varredura do Radar: Olha para a Esquerda
    atualizarDisplay("RADAR:", "Olhando Esq...");
    radarServo.write(ANGULO_ESQUERDA);
    if (!aguardarComEscutaBT(450)) return;
    int distEsquerda = lerDistanciaCM();

    // Retorna o Servo para o centro
    radarServo.write(ANGULO_CENTRO);
    if (!aguardarComEscutaBT(250)) return;

    // Tomada de Decisão baseada nas leituras
    if (distDireita < 15 && distEsquerda < 15) {
      // Beco sem saída: Gira 180 graus
      atualizarDisplay("BECO SEM SAIDA", "Meia volta...");
      girarDireita(VELOCIDADE_CURVA);
      if (!aguardarComEscutaBT(700)) return;
    } 
    else if (distDireita >= distEsquerda) {
      // Caminho da direita está mais livre
      atualizarDisplay("DESVIANDO:", ">>> Direita");
      girarDireita(VELOCIDADE_CURVA);
      if (!aguardarComEscutaBT(400)) return;
    } 
    else {
      // Caminho da esquerda está mais livre
      atualizarDisplay("DESVIANDO:", "<<< Esquerda");
      girarEsquerda(VELOCIDADE_CURVA);
      if (!aguardarComEscutaBT(400)) return;
    }

    pararMotores();
    if (!aguardarComEscutaBT(150)) return;

  } else {
    // Caminho livre: segue em frente e exibe a distância em tempo real
    moverFrente(VELOCIDADE_RETA);
    atualizarDisplayPeriodico("AUTO | LIVRE", "Dist: " + String(distanciaFrente) + " cm");
  }
}

// =========================================================================
// 8. FUNÇÕES DE LEITURA DE SENSORES E UTILITÁRIOS
// =========================================================================
int lerDistanciaCM() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  // Timeout de 25000 us (~4 metros) evita que o código trave se o eco se perder
  long duracao = pulseIn(PINO_ECHO, HIGH, 25000);

  if (duracao == 0) {
    return 250; // Se não houve eco dentro do limite, assume caminho livre (250 cm)
  }
  return duracao * 0.034 / 2;
}

// Função de delay inteligente: espera o tempo da manobra mas continua escutando o Bluetooth
bool aguardarComEscutaBT(unsigned long tempoMS) {
  unsigned long inicio = millis();
  while (millis() - inicio < tempoMS) {
    verificarBluetooth();
    if (!modoAutonomo) {
      pararMotores();
      return false; // Interrompe imediatamente se o usuário assumiu o controle manual
    }
  }
  return true;
}

void atualizarDisplay(String linha1, String linha2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  lcd.setCursor(0, 1);
  lcd.print(linha2);
}

void atualizarDisplayPeriodico(String linha1, String linha2) {
  if (millis() - ultimaAtualizacaoDisplay >= 300) {
    ultimaAtualizacaoDisplay = millis();
    atualizarDisplay(linha1, linha2);
  }
}

// =========================================================================
// 9. FUNÇÕES DE CONTROLE DOS MOTORES (PONTE H L298N)
// =========================================================================
void moverFrente(int velocidade) {
  analogWrite(PINO_ENA, velocidade);
  analogWrite(PINO_ENB, velocidade);
  digitalWrite(PINO_IN1, HIGH);
  digitalWrite(PINO_IN2, LOW);
  digitalWrite(PINO_IN3, HIGH);
  digitalWrite(PINO_IN4, LOW);
}

void moverTras(int velocidade) {
  analogWrite(PINO_ENA, velocidade);
  analogWrite(PINO_ENB, velocidade);
  digitalWrite(PINO_IN1, LOW);
  digitalWrite(PINO_IN2, HIGH);
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, HIGH);
}

void girarEsquerda(int velocidade) {
  analogWrite(PINO_ENA, velocidade);
  analogWrite(PINO_ENB, velocidade);
  // Motor Esquerdo recua, Motor Direito avança
  digitalWrite(PINO_IN1, LOW);
  digitalWrite(PINO_IN2, HIGH);
  digitalWrite(PINO_IN3, HIGH);
  digitalWrite(PINO_IN4, LOW);
}

void girarDireita(int velocidade) {
  analogWrite(PINO_ENA, velocidade);
  analogWrite(PINO_ENB, velocidade);
  // Motor Esquerdo avança, Motor Direito recua
  digitalWrite(PINO_IN1, HIGH);
  digitalWrite(PINO_IN2, LOW);
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, HIGH);
}

void pararMotores() {
  analogWrite(PINO_ENA, 0);
  analogWrite(PINO_ENB, 0);
  digitalWrite(PINO_IN1, LOW);
  digitalWrite(PINO_IN2, LOW);
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, LOW);
}
