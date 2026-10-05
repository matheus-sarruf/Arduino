const int led1 = 0;
const int led2 = 4;

const int intervalo = 500;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}

void loop() {
  digitalWrite(led1, HIGH);
  digitalWrite(led2, LOW);
  delay(intervalo); 

  digitalWrite(led1, LOW);
  digitalWrite(led2, HIGH);
  delay(intervalo);
}