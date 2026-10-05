#define BUZZER 4

#define DO4 262
#define RE4 294
#define MI4 330
#define FA4 349
#define SOL4 392
#define LA4 440
#define SI4 494
#define DO5 523
#define RE5 587
#define MI5 659
#define FA5 698
#define SOL5 784
#define LA5 880
#define SI5 988
#define DO6 1047

#define MINIMA 800
#define SEMINIMA 400
#define COLCHEIA 200
#define SEMICOLCHEIA 100

void nota(int frequencia, int duracao_ms);
void tocarSuperMarioOverworldCompleto();

void setup() {
  ledcAttach(BUZZER, 1000, 8);
  Serial.begin(115200);
  Serial.println("Sistema iniciado. Preparando loop do Mario...");
}

void loop() {
  Serial.println("Iniciando a musica...");
  tocarSuperMarioOverworldCompleto();
  
  Serial.println("Musica concluida. Reiniciando em 2 segundos...");
  delay(2000);
}

void nota(int frequencia, int duracao_ms) {
  if (frequencia == 0) {
    ledcWriteTone(BUZZER, 0);
    delay(duracao_ms);
    return;
  }
  ledcWriteTone(BUZZER, frequencia);
  delay(duracao_ms);
  ledcWriteTone(BUZZER, 0);
  delay(20);
}

void tocarSuperMarioOverworldCompleto() {
  nota(MI5, COLCHEIA); nota(MI5, COLCHEIA); delay(COLCHEIA);
  nota(MI5, COLCHEIA); delay(COLCHEIA);
  nota(DO5, COLCHEIA); nota(MI5, COLCHEIA);
  nota(SOL5, SEMINIMA); delay(SEMINIMA);
  nota(SOL4, SEMINIMA); delay(SEMINIMA);

  for (int i = 0; i < 2; i++) {
    nota(DO5, SEMINIMA); delay(COLCHEIA);
    nota(SOL4, SEMINIMA); delay(COLCHEIA);
    nota(MI4, SEMINIMA); delay(COLCHEIA);
    nota(LA4, SEMINIMA); nota(SI4, SEMINIMA); nota(LA4, SEMINIMA);
    nota(SOL4, COLCHEIA); nota(MI5, COLCHEIA); nota(SOL5, COLCHEIA);
    nota(LA5, SEMINIMA); nota(FA5, COLCHEIA); nota(SOL5, COLCHEIA);
    delay(COLCHEIA); nota(MI5, SEMINIMA); nota(DO5, COLCHEIA);
    nota(RE5, COLCHEIA); nota(SI4, COLCHEIA); delay(SEMINIMA);
  }

  for (int i = 0; i < 2; i++) {
    delay(SEMINIMA); nota(SOL5, COLCHEIA); nota(FA5, COLCHEIA);
    nota(MI5, COLCHEIA); nota(RE5, COLCHEIA); nota(MI5, SEMINIMA);
    delay(COLCHEIA); nota(LA4, COLCHEIA); nota(DO5, COLCHEIA);
    nota(LA4, COLCHEIA); nota(DO5, COLCHEIA); nota(RE5, COLCHEIA);
    
    delay(SEMINIMA); nota(SOL5, COLCHEIA); nota(FA5, COLCHEIA);
    nota(MI5, COLCHEIA); nota(RE5, COLCHEIA); nota(MI5, SEMINIMA);
    nota(DO6, COLCHEIA); delay(COLCHEIA); nota(DO6, COLCHEIA);
    nota(DO6, SEMINIMA); delay(SEMINIMA);
  }

  for (int i = 0; i < 2; i++) {
    nota(DO5, COLCHEIA); nota(DO5, SEMINIMA); nota(DO5, COLCHEIA);
    delay(COLCHEIA); nota(DO5, COLCHEIA); nota(RE5, SEMINIMA);
    nota(MI5, COLCHEIA); nota(DO5, SEMINIMA); nota(LA4, COLCHEIA);
    nota(SOL4, SEMINIMA); delay(SEMINIMA);

    nota(DO5, COLCHEIA); nota(DO5, SEMINIMA); nota(DO5, COLCHEIA);
    delay(COLCHEIA); nota(DO5, COLCHEIA); nota(RE5, COLCHEIA);
    nota(MI5, COLCHEIA); delay(SEMINIMA);
  }
}