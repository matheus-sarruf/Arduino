#define INA 32
#define INB 33
#define INC 4
#define IND 18

#define PWM_FREQ 1000
#define PWM_RES  8

#define CH_INA 0
#define CH_INB 1
#define CH_INC 2
#define CH_IND 3

#if ESP_ARDUINO_VERSION_MAJOR >= 3
  #define PWM_ATTACH(pin, ch)       ledcAttach(pin, PWM_FREQ, PWM_RES)
  #define PWM_WRITE(pin, ch, duty)  ledcWrite(pin, duty)
#else
  #define PWM_ATTACH(pin, ch)       do { ledcSetup(ch, PWM_FREQ, PWM_RES); \
                                         ledcAttachPin(pin, ch); } while (0)
  #define PWM_WRITE(pin, ch, duty)  ledcWrite(ch, duty)
#endif

void motorEsq(int vel) {
  vel = constrain(vel, -255, 255);
  int duty = abs(vel);
  if (vel >= 0) {
    PWM_WRITE(INA, CH_INA, duty);
    PWM_WRITE(INB, CH_INB, 0);
  } else {
    PWM_WRITE(INA, CH_INA, 0);
    PWM_WRITE(INB, CH_INB, duty);
  }
}

void motorDir(int vel) {
  vel = constrain(vel, -255, 255);
  int duty = abs(vel);
  if (vel >= 0) {
    PWM_WRITE(INC, CH_INC, duty);
    PWM_WRITE(IND, CH_IND, 0);
  } else {
    PWM_WRITE(INC, CH_INC, 0);
    PWM_WRITE(IND, CH_IND, duty);
  }
}

void ajuda() {
  Serial.println();
  Serial.println("========= COMANDOS =========");
  Serial.println(" f <0-255>   frente");
  Serial.println(" t <0-255>   tras");
  Serial.println(" e <0-255>   girar esquerda");
  Serial.println(" d <0-255>   girar direita");
  Serial.println(" me <-255..255>  motor esq. individual");
  Serial.println(" md <-255..255>  motor dir. individual");
  Serial.println(" p           parar");
  Serial.println(" h           ajuda");
  Serial.println("============================");
}

void processarComando(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  int espaco = cmd.indexOf(' ');
  String op  = (espaco == -1) ? cmd : cmd.substring(0, espaco);
  String arg = (espaco == -1) ? ""  : cmd.substring(espaco + 1);
  op.toLowerCase();

  int vel = arg.toInt();

  if (op == "f") {
    motorEsq(vel);   motorDir(vel);
    Serial.printf(">> FRENTE vel=%d\n", vel);
  }
  else if (op == "t") {
    motorEsq(-vel);  motorDir(-vel);
    Serial.printf(">> TRAS vel=%d\n", vel);
  }
  else if (op == "e") {
    motorEsq(-vel);  motorDir(vel);
    Serial.printf(">> ESQUERDA vel=%d\n", vel);
  }
  else if (op == "d") {
    motorEsq(vel);   motorDir(-vel);
    Serial.printf(">> DIREITA vel=%d\n", vel);
  }
  else if (op == "me") {
    motorEsq(vel);
    Serial.printf(">> Motor ESQ vel=%d\n", vel);
  }
  else if (op == "md") {
    motorDir(vel);
    Serial.printf(">> Motor DIR vel=%d\n", vel);
  }
  else if (op == "p") {
    motorEsq(0);     motorDir(0);
    Serial.println(">> PARAR");
  }
  else if (op == "h" || op == "?") {
    ajuda();
  }
  else {
    Serial.println("Comando invalido. Digite 'h' para ajuda.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("=== ESP32 Ponte H + PWM iniciado ===");

  pinMode(INA, OUTPUT); digitalWrite(INA, LOW);
  pinMode(INB, OUTPUT); digitalWrite(INB, LOW);
  pinMode(INC, OUTPUT); digitalWrite(INC, LOW);
  pinMode(IND, OUTPUT); digitalWrite(IND, LOW);

  PWM_ATTACH(INA, CH_INA);
  PWM_ATTACH(INB, CH_INB);
  PWM_ATTACH(INC, CH_INC);
  PWM_ATTACH(IND, CH_IND);

  motorEsq(0);
  motorDir(0);

  ajuda();
}

String linha = "";

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (linha.length() > 0) {
        processarComando(linha);
        linha = "";
      }
    } else {
      linha += c;
      if (linha.length() > 64) linha = ""; // evita estouro
    }
  }
}