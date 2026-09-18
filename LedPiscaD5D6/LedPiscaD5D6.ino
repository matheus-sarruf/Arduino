const int AO = AO;

void setup() {
  // Configura os pinos como saída digital
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  
  // Garante que ambos os LEDs iniciem desligados
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
}

const int intervalo = 500;

void loop() {
  digitalWrite(D5, HIGH);
  digitalWrite(D6, LOW);
  delay(intervalo); 

  digitalWrite(D5, LOW);
  digitalWrite(D6, HIGH);
  delay(intervalo);
}
