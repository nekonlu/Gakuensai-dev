// Arduino入門編㉖ UART通信について！
// https://burariweb.info
// LED点灯テスト①(送信側スケッチ)
 
const int LED_PIN = 13;

void setup(){
 
  Serial.begin(9600);          // シリアル通信の開始(ボーレート9600bps)
  pinMode(LED_PIN, OUTPUT);
 
}
 
void loop(){
 
  digitalWrite(LED_PIN, HIGH);
  Serial.write(1);              // "1"を送信
  delay(500);
 
  digitalWrite(LED_PIN, LOW);
  Serial.write(0);              // "0"を送信
  delay(500);
 
}
