// 1
const int LED1 = 8;
const int LED2 = 9;  
const int LED3 = 10;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  digitalWrite(LED1, HIGH); digitalWrite(LED2, LOW);  digitalWrite(LED3, LOW);
  delay(300);

  digitalWrite(LED1, LOW);  digitalWrite(LED2, HIGH); digitalWrite(LED3, LOW);
  delay(300);

  digitalWrite(LED1, LOW);  digitalWrite(LED2, LOW);  digitalWrite(LED3, HIGH);
  delay(300);
}


//2
void setup() {
  DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2); //00000111
}

void loop() {
  PORTB = (1 << PB0);//00000001
  PORTB = (1 << PB1);//00000010
  PORTB = (1 << PB2);//00000100
  PORTB = 0x00;//00000000
}
