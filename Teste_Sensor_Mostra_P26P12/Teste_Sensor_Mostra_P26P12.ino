#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// ============================================================
// DIAGNOSTICO - D6 no GPIO 0 (analogico), D3 no GPIO 23 (digital)
// ============================================================

const int numSensores = 16;
const int pinosSensores[numSensores] = {
  39, 36, 34, 35, 32, 33, 25, 26,  // Modulo 1 (Esquerda)
  27, 14, 23, 13, 4, 0, 2, 15      // Modulo 2 (Direita) - D6 agora no GPIO 0
};

// So o GPIO 23 e digital agora
bool ehDigital(int pino) {
  return (pino == 23);
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("CHOSO");
  Serial.println("Bluetooth: CHOSO");
  Serial.println("=== DIAGNOSTICO DOS SENSORES ===");

  for (int i = 0; i < numSensores; i++) {
    pinMode(pinosSensores[i], INPUT);
  }

  delay(2000);
  SerialBT.println("=== DIAGNOSTICO ===");
  SerialBT.println("S1-S8 = Modulo 1 (Esq)");
  SerialBT.println("S9-S16 = Modulo 2 (Dir)");
  SerialBT.println("S11 (GP23) = DIGITAL");
  SerialBT.println("Os outros 15 = ANALOGICOS");
  SerialBT.println();
}

void loop() {
  String msg = "";

  // --- Linha 1: Cabecalho ---
  msg += "S:  ";
  for (int i = 0; i < numSensores; i++) {
    if (i == 8) msg += "| ";
    msg += "S";
    if (i < 9) msg += " ";
    msg += " ";
  }
  SerialBT.println(msg);

  // --- Linha 2: Tipo (A/D) ---
  msg = "T:  ";
  for (int i = 0; i < numSensores; i++) {
    if (i == 8) msg += "| ";
    if (ehDigital(pinosSensores[i])) msg += "D";
    else msg += "A";
    msg += "  ";
  }
  SerialBT.println(msg);

  // --- Linha 3: Valores ---
  msg = "V: ";
  for (int i = 0; i < numSensores; i++) {
    if (i == 8) msg += "| ";

    if (ehDigital(pinosSensores[i])) {
      // Leitura digital (0 ou 1)
      int v = digitalRead(pinosSensores[i]);
      msg += String(v);
      msg += "  ";
    } else {
      // Leitura analogica (0 a 4095)
      int v = analogRead(pinosSensores[i]);
      msg += String(v);
      int tamanho = String(v).length();
      for (int j = tamanho; j < 5; j++) msg += " ";
    }
  }
  SerialBT.println(msg);
  SerialBT.println();

  delay(500);
}