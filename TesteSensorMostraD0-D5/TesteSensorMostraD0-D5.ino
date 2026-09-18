#define S1 D0
#define S2 D1
#define S3 D2
#define S4 D3
#define S5 D4

#define S6 D5

#define MD D6
#define ME D7

void setup() {
  Serial.begin(115200);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);

  pinMode(S6, INPUT);

  pinMode(MD, OUTPUT);
  pinMode(ME, OUTPUT);
}

void loop() {
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s4 = digitalRead(S4);
  int s5 = digitalRead(S5);
  int s6 = digitalRead(S6);

  char buffer[80];
  snprintf(buffer, sizeof(buffer), "S1=%d | S2=%d | S3=%d | S4=%d | S5=%d | S6=%d", s1, s2, s3, s4, s5, s6);
  Serial.println(buffer);

}