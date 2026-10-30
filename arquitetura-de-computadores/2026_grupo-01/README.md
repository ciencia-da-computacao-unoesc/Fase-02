# 🤖 Robô Autônomo & Bluetooth com Arduino

Projeto de desenvolvimento de um robô móvel autônomo e controlável via Bluetooth desenvolvido com a plataforma Arduino. O robô opera em dois modos distintos: **Navegação Autônoma** com desvio de obstáculos através de radar ultrassônico móvel (servo motor) e redundância infravermelha frontal, e **Modo Manual** controlado via smartphone com telemetria em display.

---

## 👥 Integrantes do Grupo
HANS WAGNER BETINARDI <br>
LUCIMAR BOTH <br>
MIRIAN RODRIGUES <br>
NATAN OGLIARI <br>
NELSO GABRIEL ALBA <br>
---

## 🚀 Funcionalidades

- **Modo Autônomo com Radar 180°:**
  - Varredura de ambiente com o sensor ultrassônico HC-SR04 montado sobre o servo SG90.
  - Análise de rotas à esquerda e à direita para escolha da rota.
  - Manobra automática de recuo e giro de 180° em situações de beco sem saída.
- **Camada de Proteção Infravermelha (IR):**
  - Detecção imediata de obstáculos em proximidade crítica (< 10 cm) na dianteira do chassi.
- **Modo Manual via Bluetooth (HC-05):**
  - Controle remoto total através do aplicativo *Arduino Bluetooth Control* (Frente, Ré, Esquerda, Direita e Parar).
- **Interface Visual:**
  - Exibição de telemetria em tempo real (modo ativo, leituras de distância em centímetros, nível de bateria) em display LCD 16x2 I2C.

---

## 🛠️ Componentes e Hardware Utilizado

- **Placa Controladora:** Arduino Uno R3(ATmega328P)
- **Atuadores de Tração:** 2x Motores DC com Caixa de Redução (3-6V, eixo duplo) + Rodas emborrachadas
- **Driver de Potência:** Módulo Ponte H Dupla L298N
- **Sistema de Varredura:**
  - Sensor de Distância Ultrassônico HC-SR04
  - Micro Servo Motor 9g (SG90)
- **Sensor de Proximidade:** 1x Sensor de Obstáculo Infravermelho Reflexivo (IR)
- **Comunicação Sem Fio:** Módulo Bluetooth HC-05
- **Display:** Display LCD 16x2 com Módulo I2C
- **Alimentação:** Suporte com 2 baterias.

---

## ⚡ Esquema de Conexões (Pinagem)

| Componente | Pino do Módulo | Pino no Arduino | Função / Tipo |
| :--- | :--- | :--- | :--- |
| **Driver L298N** | `ENA` / `ENB` | **D5** / **D6** | Controle de velocidade PWM (Timer0) |
| **Driver L298N** | `IN1`, `IN2` | **D4**, **D7** | Direção do Motor Esquerdo |
| **Driver L298N** | `IN3`, `IN4` | **D8**, **D12** | Direção do Motor Direito |
| **Servo SG90** | Sinal (Laranja) | **D9** | Controle do ângulo do radar (Timer1) |
| **Sensor HC-SR04** | `Trig` / `Echo` | **D10** / **D11** | Disparo e leitura do pulso ultrassônico |
| **Sensor IR** | `OUT` / `D0` | **D2** | Interrupção / leitura digital de proximidade |
| **Módulo HC-05** | `TXD` / `RXD` | **D3** / **D13** | Comunicação Serial via `SoftwareSerial` |
| **Display I2C** | `SDA` / `SCL` | **A4** / **A5** | Barramento de dados I2C |

---

## 📱 Mapeamento de Comandos (App Bluetooth)

O aplicativo [**Arduino Bluetooth Control**](https://play.google.com/store/apps/details?id=com.broxcode.arduinobluetoothfree&hl=pt) deve ser configurado com a seguinte tabela de envio de caracteres:

| Botão / Função | Caractere Enviado | Descrição da Ação |
| :---: | :---: | :--- |
| **Frente** | `F` | Movimenta os dois motores para frente |
| **Ré** | `B` | Movimenta os dois motores para trás |
| **Esquerda** | `L` | Gira o robô sobre o próprio eixo à esquerda |
| **Direita** | `R` | Gira o robô sobre o próprio eixo à direita |
| **Parar** | `S` | Para imediatamente ambos os motores |
| **Modo Autônomo** | `A` ou `X` | Habilita a tomada de decisão autônoma |
| **Modo Manual** | `M` ou `x` | Desativa o modo autônomo e aguarda comandos manuais |
