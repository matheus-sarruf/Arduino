#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "JOSE";
const char* password = "70701149";

#define INA D6
#define INB D7
#define ENA D8

// Servidor TCP para debug remoto
WiFiServer serverDebug(23);  // Porta 23 = Telnet padrão
WiFiClient clientDebug;

unsigned long previousMillis = 0;
const long interval = 2000;
int state = 0;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("Robo_Seguidor");
  ArduinoOTA.setPassword("3784");
  ArduinoOTA.begin();

  // Inicia o servidor de debug
  serverDebug.begin();
  serverDebug.setNoDelay(true);

  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(ENA, OUTPUT);
  digitalWrite(INA, LOW);
  digitalWrite(INB, LOW);
  analogWrite(ENA, 0);

  previousMillis = millis();
}

// Função que envia para Serial E para a rede ao mesmo tempo
void logMsg(String msg) {
  Serial.println(msg);          // Para o cabo USB (COM4)
  if (clientDebug && clientDebug.connected()) {
    clientDebug.println(msg);   // Para quem estiver conectado via rede
  }
}

void loop() {
  ArduinoOTA.handle();

  // Aceita novas conexões de debug
  if (serverDebug.hasClient()) {
    if (!clientDebug || !clientDebug.connected()) {
      clientDebug = serverDebug.available();
      clientDebug.println("=== Conectado ao Robo Seguidor ===");
    }
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    state++;
    if (state > 2) state = 0;

    if (state == 0) {
      logMsg("Motor para FRENTE");
      digitalWrite(INA, HIGH);
      digitalWrite(INB, LOW);
      analogWrite(ENA, 180);
    } else if (state == 1) {
      logMsg("Motor para TRAS");
      digitalWrite(INA, LOW);
      digitalWrite(INB, HIGH);
      analogWrite(ENA, 180);
    } else {
      logMsg("Motor PARADO");
      digitalWrite(INA, LOW);
      digitalWrite(INB, LOW);
      analogWrite(ENA, 0);
    }
  }
}