#define BUZZER D6

#define DO4  262
#define RE4  294
#define MI4  330
#define FA4  349
#define SOL4 392
#define LA4  440
#define SI4  494
#define DO5  523
#define RE5  587
#define MI5  659
#define FA5  698
#define SOL5 784
#define LA5  880
#define SI5  988
#define DO6  1047

#define MINIMA      800
#define SEMINIMA    400
#define COLCHEIA    200
#define SEMICOLCHEIA 100

void setup() {
  pinMode(BUZZER, OUTPUT);
  Serial.begin(115200);
  Serial.println("Tocando Super Mario...");
  tocarSuperMario();

  Serial.println("Fim da musica. Entrando em modo espera.");
}

void loop() {
  delay(1000);
}

void nota(int frequencia, int duracao_ms) {
  if (frequencia == 0) {
    delay(duracao_ms);
    return;
  }
  long periodo_us = 1000000L / frequencia;
  long meio = periodo_us / 2;
  long ciclos = (long)duracao_ms * 1000L / periodo_us;

  for (long i = 0; i < ciclos; i++) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(meio);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(meio);
  }
  delay(30);
}

void tocarSuperMario() {
  nota(MI5, COLCHEIA); nota(MI5, COLCHEIA); delay(COLCHEIA);
  nota(MI5, COLCHEIA); delay(COLCHEIA);
  nota(DO5, COLCHEIA); nota(MI5, COLCHEIA);
  nota(SOL5, SEMINIMA); delay(SEMINIMA);
  nota(SOL4, SEMINIMA); delay(SEMINIMA);

  nota(DO5, SEMINIMA); delay(COLCHEIA);
  nota(SOL4, SEMINIMA); delay(COLCHEIA);
  nota(MI4, SEMINIMA); delay(COLCHEIA);
  nota(LA4, SEMINIMA); nota(SI4, SEMINIMA);
  nota(LA4, SEMINIMA); nota(SOL4, COLCHEIA);
  nota(MI5, COLCHEIA); nota(SOL5, COLCHEIA);
  nota(LA5, SEMINIMA); nota(FA5, COLCHEIA);
  nota(SOL5, COLCHEIA); delay(COLCHEIA);
  nota(MI5, SEMINIMA); nota(DO5, COLCHEIA);
  nota(RE5, COLCHEIA); nota(SI4, COLCHEIA);
  delay(SEMINIMA);

  nota(MI5, COLCHEIA); nota(MI5, COLCHEIA); delay(COLCHEIA);
  nota(MI5, COLCHEIA); delay(COLCHEIA);
  nota(DO5, COLCHEIA); nota(MI5, COLCHEIA);
  nota(SOL5, SEMINIMA); delay(SEMINIMA);
  nota(SOL4, SEMINIMA); delay(SEMINIMA);

  nota(DO5, SEMINIMA); delay(COLCHEIA);
  nota(SOL4, SEMINIMA); delay(COLCHEIA);
  nota(MI4, SEMINIMA); delay(COLCHEIA);
  nota(LA4, SEMINIMA); nota(SI4, SEMINIMA);
  nota(LA4, SEMINIMA); nota(SOL4, COLCHEIA);
  nota(MI5, COLCHEIA); nota(SOL5, COLCHEIA);
  nota(LA5, SEMINIMA); nota(FA5, COLCHEIA);
  nota(SOL5, COLCHEIA); delay(COLCHEIA);
  nota(MI5, SEMINIMA); nota(DO5, COLCHEIA);
  nota(RE5, COLCHEIA); nota(SI4, COLCHEIA);
  delay(SEMINIMA);

  delay(SEMINIMA);
  nota(RE5, COLCHEIA); nota(RE5, COLCHEIA);
  nota(RE5, SEMINIMA); delay(COLCHEIA);
  nota(RE5, COLCHEIA); nota(MI5, SEMINIMA);
  nota(DO5, SEMINIMA); delay(COLCHEIA);
  nota(SOL4, SEMINIMA); nota(SOL4, SEMINIMA);

  delay(SEMINIMA);
  nota(DO5, SEMINIMA); nota(SI4, COLCHEIA);
  nota(SOL4, COLCHEIA); nota(MI4, COLCHEIA);
  nota(SOL4, COLCHEIA); nota(DO5, SEMINIMA);
  nota(SI4, COLCHEIA); nota(SOL4, COLCHEIA);
  nota(MI4, COLCHEIA); nota(SOL4, COLCHEIA);
  delay(COLCHEIA);

  nota(DO5, MINIMA);
  delay(COLCHEIA);
}