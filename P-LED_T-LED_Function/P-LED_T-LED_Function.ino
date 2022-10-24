#include <Wire.h>
#include <math.h>

int MS = 100;
int t_rclkPin = 3, t_srclkPin = 4, t_dsPin = 2;     
int p_rclkPin = 11, p_srclkPin = 10, p_dsPin = 12;     

byte leds = 0;

// P-LED: 0
// T-LED: 1
void offAllLED(int led);
void onLED(int position);

void setup() {
  pinMode(t_rclkPin, OUTPUT);  
  pinMode(t_dsPin, OUTPUT);   
  pinMode(t_srclkPin, OUTPUT);  

  pinMode(p_rclkPin, OUTPUT);  
  pinMode(p_dsPin, OUTPUT);   
  pinMode(p_srclkPin, OUTPUT);  
}

void loop() 
{
  for(int i = 0; i < 16; i++) {
    onLED(i);
    delay(100);
    offAllLED(i / 8);
  }
}

void offAllLED(int led) {

  if(led == 0) {
    digitalWrite(p_rclkPin, LOW);
    shiftOut(p_dsPin, p_srclkPin, LSBFIRST, B00000000);
    digitalWrite(p_rclkPin, HIGH);
  } else if(led == 1) {
    digitalWrite(t_rclkPin, LOW);
    shiftOut(t_dsPin, t_srclkPin, LSBFIRST, B00000000);
    digitalWrite(t_rclkPin, HIGH);    
  }
}

void onLED(int position) {

  if(position <= 7) {
    byte bitPosition;
    bitSet(bitPosition, position);
    digitalWrite(p_rclkPin, LOW);
    shiftOut(p_dsPin, p_srclkPin, LSBFIRST, bitPosition);
    digitalWrite(p_rclkPin, HIGH);
  }
  
  if(position >= 8 && position <= 15) {
    byte bitPosition; 
    bitSet(bitPosition, position % 8);
    digitalWrite(t_rclkPin, LOW);
    shiftOut(t_dsPin, t_srclkPin, LSBFIRST, bitPosition);
    digitalWrite(t_rclkPin, HIGH);
  }
  
}
