#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// ========== PINOS DOS SENSORES ==========
const int numSensores = 16;
const int pinosSensores[numSensores] = {
  39, 36, 34, 35, 32, 33, 25, 26,  // Módulo 1 (Esquerda)
  27, 14, 16, 13, 4, 0, 2, 15      // Módulo 2 (Direita) - D3 agora no GPIO 16
};

// ========== PINOS DOS MOTORES ==========
// STBY não é mais controlado por GPIO (ligado direto no 3.3V)
#define PWMA 18
#define AIN1 5
#define AIN2 17
#define PWMB 19
#define BIN1 21
#define BIN2 22

// ========== CALIBRAÇÃO ==========
int minVal[numSensores];
int maxVal[numSensores];
int normalizado[numSensores];

// ========== PID ==========
float Kp = 0.10;
float Ki = 0.0;
float Kd = 2.0;

// ========== VELOCIDADE ==========
int velocidadeBase = 130;

// ========== VARIÁVEIS DE CONTROLE ==========
float erro = 0, erroAnterior = 0, integral = 0, derivada = 0, correcao = 0;
int posicao = 0, ultimaPosicao = 0;

// ========== ESTADOS ==========
bool seguindo = false;
bool debugAtivo = true;
unsigned long ultimoDebug = 0;

// ========== BUFFER BLUETOOTH ==========
String bufferBT = "";

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  SerialBT.begin("CHOSO");
  Serial.println("Bluetooth: CHOSO");

  for (int i = 0; i < numSensores; i++) {
    pinMode(pinosSensores[i], INPUT);
    minVal[i] = 4095;
    maxVal[i] = 0;
  }

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  ledcAttach(PWMA, 5000, 8);
  ledcAttach(PWMB, 5000, 8);
  parar();

  Serial.println("Calibrando em 3 segundos...");
  SerialBT.println(">> CALIBRACAO em 3s. Mova o robo sobre a linha e o fundo!");
  delay(3000);
  calibrar();

  imprimirAjuda();
}

// ============================================================
// CALIBRAÇÃO
// ============================================================
void calibrar() {
  Serial.println("CALIBRANDO (8s)...");
  SerialBT.println(">> CALIBRANDO (8s)...");

  for (int i = 0; i < numSensores; i++) {
    minVal[i] = 4095;
    maxVal[i] = 0;
  }

  unsigned long inicio = millis();
  while (millis() - inicio < 8000) {
    for (int i = 0; i < numSensores; i++) {
      int v = analogRead(pinosSensores[i]);
      if (v < minVal[i]) minVal[i] = v;
      if (v > maxVal[i]) maxVal[i] = v;
    }
    delay(5);
  }

  SerialBT.println(">> Calibracao OK!");
  Serial.println("Calibracao OK!");
}

// ============================================================
// CONTROLE DOS MOTORES
// ============================================================
void setMotor(int motor, int speed) {
  if (speed > 255) speed = 255;
  if (speed < -255) speed = -255;

  if (motor == 1) {
    if (speed > 0) { digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); }
    else if (speed < 0) { digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); }
    else { digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW); }
    ledcWrite(PWMA, abs(speed));
  } else {
    if (speed > 0) { digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); }
    else if (speed < 0) { digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); }
    else { digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW); }
    ledcWrite(PWMB, abs(speed));
  }
}

void parar() {
  setMotor(1, 0);
  setMotor(2, 0);
}

// ============================================================
// LEITURA DOS SENSORES
// ============================================================
void lerSensores() {
  for (int i = 0; i < numSensores; i++) {
    int v = analogRead(pinosSensores[i]);
    if (maxVal[i] - minVal[i] == 0) {
      normalizado[i] = 0;
    } else {
      normalizado[i] = (v - minVal[i]) * 1000 / (maxVal[i] - minVal[i]);
    }
    if (normalizado[i] < 0) normalizado[i] = 0;
    if (normalizado[i] > 1000) normalizado[i] = 1000;
  }
}

// Retorna posição da linha (-7500 a +7500) ou 9999 se perdeu
int calcularPosicao() {
  long somaPesos = 0;
  long somaValores = 0;

  for (int i = 0; i < numSensores; i++) {
    int peso = (i * 1000) - 7500;
    somaPesos += (long)peso * normalizado[i];
    somaValores += normalizado[i];
  }

  if (somaValores < 500) return 9999;
  return somaPesos / somaValores;
}

// ============================================================
// COMANDOS BLUETOOTH
// ============================================================
void imprimirAjuda() {
  SerialBT.println("==== COMANDOS ====");
  SerialBT.println("g = Seguir linha");
  SerialBT.println("s = Parar");
  SerialBT.println("c = Calibrar novamente");
  SerialBT.println("w/a/d/x = Teste manual motores");
  SerialBT.println("p<v> = Kp (ex: p0.15)");
  SerialBT.println("i<v> = Ki (ex: i0.001)");
  SerialBT.println("k<v> = Kd (ex: k2.5)");
  SerialBT.println("v<v> = Velocidade base (ex: v150)");
  SerialBT.println("show = Ver config atual");
  SerialBT.println("debug = Liga/desliga debug");
  SerialBT.println("h = Esta ajuda");
}

void processarComando(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  char c = cmd.charAt(0);
  String valor = cmd.substring(1);
  valor.trim();

  switch (c) {
    case 'g': seguindo = true; SerialBT.println(">> SEGUINDO LINHA"); break;
    case 's': seguindo = false; parar(); SerialBT.println(">> PARADO"); break;
    case 'c': parar(); seguindo = false; delay(300); calibrar(); break;
    case 'w': setMotor(1, 150); setMotor(2, 150); SerialBT.println(">> FRENTE"); break;
    case 'a': setMotor(1, -150); setMotor(2, 150); SerialBT.println(">> ESQ"); break;
    case 'd': setMotor(1, 150); setMotor(2, -150); SerialBT.println(">> DIR"); break;
    case 'x': setMotor(1, -150); setMotor(2, -150); SerialBT.println(">> RE"); break;

    case 'p':
      if (valor.length() > 0) Kp = valor.toFloat();
      SerialBT.print(">> Kp = "); SerialBT.println(Kp, 3);
      break;
    case 'i':
      if (valor.length() > 0) Ki = valor.toFloat();
      SerialBT.print(">> Ki = "); SerialBT.println(Ki, 4);
      break;
    case 'k':
      if (valor.length() > 0) Kd = valor.toFloat();
      SerialBT.print(">> Kd = "); SerialBT.println(Kd, 3);
      break;
    case 'v':
      if (valor.length() > 0) velocidadeBase = valor.toInt();
      SerialBT.print(">> Velocidade = "); SerialBT.println(velocidadeBase);
      break;

    case 'h': imprimirAjuda(); break;

    default:
      if (cmd == "show") {
        SerialBT.print("Kp="); SerialBT.print(Kp, 3);
        SerialBT.print(" Ki="); SerialBT.print(Ki, 4);
        SerialBT.print(" Kd="); SerialBT.print(Kd, 3);
        SerialBT.print(" Vel="); SerialBT.println(velocidadeBase);
      } else if (cmd == "debug") {
        debugAtivo = !debugAtivo;
        SerialBT.print(">> Debug "); SerialBT.println(debugAtivo ? "ON" : "OFF");
      } else {
        SerialBT.println(">> Desconhecido. Digite 'h'.");
      }
  }
}

// ============================================================
// LOOP PRINCIPAL
// ============================================================
void loop() {
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n' || c == '\r') {
      if (bufferBT.length() > 0) {
        processarComando(bufferBT);
        bufferBT = "";
      }
    } else {
      bufferBT += c;
      if (bufferBT.length() > 30) bufferBT = "";
    }
  }

  lerSensores();

  if (seguindo) {
    posicao = calcularPosicao();

    if (posicao == 9999) {
      if (ultimaPosicao > 0) {
        setMotor(1, velocidadeBase);
        setMotor(2, -velocidadeBase / 2);
      } else {
        setMotor(1, -velocidadeBase / 2);
        setMotor(2, velocidadeBase);
      }
    } else {
      erro = posicao;
      integral += erro;
      if (integral > 10000) integral = 10000;
      if (integral < -10000) integral = -10000;

      derivada = erro - erroAnterior;
      erroAnterior = erro;

      correcao = Kp * erro + Ki * integral + Kd * derivada;
      ultimaPosicao = posicao;

      int velEsq = velocidadeBase + (int)correcao;
      int velDir = velocidadeBase - (int)correcao;

      setMotor(1, velEsq);
      setMotor(2, velDir);
    }
  }

  if (debugAtivo && millis() - ultimoDebug > 200) {
    ultimoDebug = millis();
    String msg = "P:" + String(posicao) + " E:" + String(erro, 0) + " C:" + String(correcao, 0) + " | ";
    for (int i = 0; i < numSensores; i++) {
      if (normalizado[i] > 500) msg += "1";
      else msg += "0";
      if (i == 7) msg += "|";
    }
    SerialBT.println(msg);
  }

  delay(5);
}