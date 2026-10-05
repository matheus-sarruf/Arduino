// ==================================================
//   ESP32 - Robo Seguidor AUTONOMO (LINHA PRETA)
//   + BOTAO DE EMERGENCIA no GPIO 33
//   + RECONEXAO WIFI AUTOMATICA
// ==================================================

#include <WiFi.h>
#include <ArduinoOTA.h>

// ---------- Credenciais WiFi ----------
const char* ssid = "JOSE";
const char* password = "70701149";

// ---------- Sensores ----------
#define S1 36
#define S2 39
#define S3 34
#define S4 35
#define S5 16

// ---------- Ponte H ----------
#define INA 32
#define IND 18

#define ENA 23
#define ENB 19

// ---------- Botao de emergencia ----------
#define BOTAO 33

// ---------- PWM ----------
#define PWM_FREQ 1000
#define PWM_RES 8

// ---------- Servidor TCP ----------
WiFiServer serverDebug(23);
#define MAX_CLIENTES 3
WiFiClient clientes[MAX_CLIENTES];

// ============ AJUSTES ============
int VEL_RETO = 150;
int VEL_CURVA_LENTA = 110;
int VEL_CURVA_RAPIDA = 180;
// =================================

String acaoAtual = "INICIANDO";
bool paradoPorBotao = false;

// Variavel para controle de reconexao WiFi
unsigned long ultimaTentativaWiFi = 0;

// ---------- Log ----------
void logMsg(const String& msg) {
  Serial.println(msg);
  for (int i = 0; i < MAX_CLIENTES; i++)
    if (clientes[i] && clientes[i].connected()) clientes[i].println(msg);
}

// ---------- Motores ----------
void setMotorEsq(int vel) {
  vel = constrain(vel, 0, 255);
  if (vel == 0) {
    digitalWrite(INA, LOW);
    ledcWrite(ENA, 0);
  } else {
    digitalWrite(INA, HIGH);
    ledcWrite(ENA, vel);
  }
}

void setMotorDir(int vel) {
  vel = constrain(vel, 0, 255);
  if (vel == 0) {
    digitalWrite(IND, LOW);
    ledcWrite(ENB, 0);
  } else {
    digitalWrite(IND, HIGH);
    ledcWrite(ENB, vel);
  }
}

void parar() {
  setMotorEsq(0);
  setMotorDir(0);
}

// ---------- Erro ----------
// ATENCAO: fita PRETA (0) sobre pista BRANCA (1)
int calcErro(int s1, int s2, int s3, int s4, int s5) {
  int soma = 0, ativos = 0;
  // Invertido para procurar 0 (Preto) em vez de 1 (Branco)
  if (s1 == 0) {
    soma += -3;
    ativos++;
  }
  if (s2 == 0) {
    soma += -1;
    ativos++;
  }
  if (s3 == 0) {
    soma += 0;
    ativos++;
  }
  if (s4 == 0) {
    soma += +1;
    ativos++;
  }
  if (s5 == 0) {
    soma += +3;
    ativos++;
  }
  if (ativos == 0) return 99;
  return soma / ativos;
}

// ---------- Logica ----------
void seguirLinha(int s1, int s2, int s3, int s4, int s5) {
  int erro = calcErro(s1, s2, s3, s4, s5);

  if (erro == 99) {
    parar();
    acaoAtual = "PERDIDO -> PARADO";
    return;
  }
  if (erro <= -2) {
    setMotorEsq(0);
    setMotorDir(VEL_CURVA_RAPIDA);
    acaoAtual = "VIRANDO ESQ FORTE";
  } else if (erro == -1) {
    setMotorEsq(VEL_CURVA_LENTA);
    setMotorDir(VEL_RETO);
    acaoAtual = "VIRANDO ESQ leve";
  } else if (erro == 0) {
    setMotorEsq(VEL_RETO);
    setMotorDir(VEL_RETO);
    acaoAtual = "RETO";
  } else if (erro == 1) {
    setMotorEsq(VEL_RETO);
    setMotorDir(VEL_CURVA_LENTA);
    acaoAtual = "VIRANDO DIR leve";
  } else {
    setMotorEsq(VEL_CURVA_RAPIDA);
    setMotorDir(0);
    acaoAtual = "VIRANDO DIR FORTE";
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== Robo Seguidor AUTONOMO + BOTAO ===");

  pinMode(INA, OUTPUT);
  digitalWrite(INA, LOW);
  pinMode(IND, OUTPUT);
  digitalWrite(IND, LOW);
  ledcAttach(ENA, PWM_FREQ, PWM_RES);
  ledcAttach(ENB, PWM_FREQ, PWM_RES);
  parar();

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);

  // Botao com pull-up interno
  pinMode(BOTAO, INPUT_PULLUP);

  // --- WiFi (Tenta conectar por 5s, se falhar continua e tenta no loop) ---
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("Robo_Seguidor");
  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 5000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi OK. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi nao conectou ainda - vai tentar no loop");
  }

  // --- Configuracoes do OTA ---
  ArduinoOTA.setHostname("Robo_Seguidor");
  ArduinoOTA.setPassword("3784");
  ArduinoOTA.begin();

  // --- Servidor TCP ---
  serverDebug.begin();
  serverDebug.setNoDelay(true);

  Serial.println("Iniciando em 2s...");
  delay(2000);
  Serial.println(">>> SEGUINDO <<<\n");
}

// ---------- Loop ----------
unsigned long ultimoLog = 0;

void loop() {
  // Mantem o OTA ativo
  ArduinoOTA.handle();

  // ---------- RECONEXAO WIFI AUTOMATICA ----------
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - ultimaTentativaWiFi > 5000) {  // Tenta a cada 5 segundos
      ultimaTentativaWiFi = millis();
      Serial.println("WiFi caiu ou desconectado. Tentando reconectar...");
      WiFi.disconnect();
      WiFi.reconnect();
    }
  }

  // ---------- Servidor TCP Debug ----------
  if (serverDebug.hasClient()) {
    for (int i = 0; i < MAX_CLIENTES; i++) {
      if (!clientes[i] || !clientes[i].connected()) {
        if (clientes[i]) clientes[i].stop();
        clientes[i] = serverDebug.available();
        clientes[i].setNoDelay(true);
        clientes[i].println("=== Conectado ao Robo Seguidor ===");
        break;
      }
    }
  }

  // ---------- Leitura dos Sensores ----------
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s4 = digitalRead(S4);
  int s5 = digitalRead(S5);

  // ---------- Botao toggle com debounce ----------
  static bool estadoBotao = false;
  static bool leituraAnterior = false;
  static unsigned long ultimaMudanca = 0;
  const unsigned long DEBOUNCE_MS = 50;

  bool leitura = (digitalRead(BOTAO) == LOW);

  if (leitura != leituraAnterior) ultimaMudanca = millis();
  leituraAnterior = leitura;

  if ((millis() - ultimaMudanca) > DEBOUNCE_MS && leitura != estadoBotao) {
    estadoBotao = leitura;
    if (estadoBotao) {  // so na BORDA de aperto
      paradoPorBotao = !paradoPorBotao;
      if (paradoPorBotao) logMsg(">>> PARADO <<<");
      else logMsg(">>> RETOMANDO <<<");
    }
  }

  // ---------- Logica de Movimento ----------
  if (paradoPorBotao) {
    parar();
    acaoAtual = "PARADO (aperte de novo)";
  } else {
    seguirLinha(s1, s2, s3, s4, s5);
  }

  // ---------- Log Serial e TCP ----------
  if (millis() - ultimoLog > 150) {
    ultimoLog = millis();
    int erro = calcErro(s1, s2, s3, s4, s5);
    char buf[220];
    snprintf(buf, sizeof(buf),
             "S: %d %d %d %d %d  | erro=%+3d | %s",
             s1, s2, s3, s4, s5, erro, acaoAtual.c_str());
    logMsg(buf);
  }
}