
/* Parameter */
int tDelay = 1000;
int rclkPin = 11;
int srclkPin = 9;
int dsPin = 12;
int serialSpeed = 9600;

byte leds = 0;

void setup(){

  Serial.begin(serialSpeed);

  pinMode(rclkPin, OUTPUT);
  pinMode(dsPin, OUTPUT);
  pinMode(srclkPin, OUTPUT);
}

void onAllLED() {
  digitalWrite(rclkPin, LOW);
  shiftOut(dsPin, srclkPin, LSBFIRST, B11111111);
  digitalWrite(rclkPin, HIGH); 
}

void offAllLED() {
  digitalWrite(rclkPin, LOW);
  shiftOut(dsPin, srclkPin, LSBFIRST, B00000000);
  digitalWrite(rclkPin, HIGH);
}

void onLED(int position) {
  if(position == 8) {
    onAllLED();
  } else if(position == 9) {
    offAllLED();    
  } else {
    position %= 8;
    byte bitPosition;
    bitSet(bitPosition, position);
    digitalWrite(rclkPin, LOW);
    shiftOut(dsPin, srclkPin, LSBFIRST, bitPosition);
    digitalWrite(rclkPin, HIGH);
    //0delay(tDelay);
  }
  
}

void loop(){

  char position;

  if(Serial.available() > 0) {
    position = Serial.read() - '0';
    Serial.print(position);
    if(position >= 0) {
      Serial.print(position);
      onLED(position);
      delay(tDelay);
    }
  }
}
