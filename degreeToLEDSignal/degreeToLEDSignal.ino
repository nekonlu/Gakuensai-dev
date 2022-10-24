#include <Wire.h>
#include <math.h>
#define MS 100

int rclkPin = 11;   // (11) ST_CP [RCLK] on 74HC595
int srclkPin = 10;   // (9)  SH_CP [SRCLK] on 74HC595
int dsPin = 12;     // (12) DS [SER] on 74HC595

int GT = 20;      // GyroThreshold ジャイロセンサーとPosition LEDの閾値

void onAllLED();
void offAllLED();
void onLED(int position);
void signalToLED(double X_deg, double Y_deg);

void setup() {

  pinMode(rclkPin, OUTPUT);   
  pinMode(dsPin, OUTPUT);     
  pinMode(srclkPin, OUTPUT);  

	// omajinai begin
	Serial.begin(9600);				//シリアル通信のデータ転送レートを設定しポート開放
	Serial.println("--- Started ---");
  
	Wire.begin();					//I2C通信開始
	// センサ開始動作 
	Wire.beginTransmission(0x68);	//アドレス0x68指定でMPU-6050を選択、送信処理開始
	Wire.write(0x6B);				//MPU6050_PWR_MGMT_1レジスタのアドレス指定
	Wire.write(0x00);				//0を書き込むことでセンサ動作開始
	Wire.endTransmission();			//スレーブデバイスに対する送信を完了
	// omajinai end
}

double pre_gx = 0;

int signal;

void loop() {
  
  // omajinai begin
  // スタートアドレスの設定とデータの要求処理
  Wire.beginTransmission(0x68);     //アドレス0x68指定でMPU-6050を選択、送信処理開始
  Wire.write(0x3B);                 //ACCEL_XOUT_Hレジスタのアドレス指定
  Wire.endTransmission(false);      //false設定で接続維持
  Wire.requestFrom(0x68, 14, true); //MPU-6050に対して14byte分データを要求、I2Cバス開放
  
  int16_t axInt, ayInt, azInt, gx, gy, gz, Temp; //符号付き整数を格納する16ビットの変数型の設定

  //シフト演算と論理和で16bitのデータを変数に格納
  //ax～gzまで、16bit(2byte) × 7 = 14byte
  axInt = Wire.read() << 8 | Wire.read();    //x軸の加速度の読み取り 16bit
  ayInt = Wire.read() << 8 | Wire.read();    //y軸の加速度の読み取り 16bit
  azInt = Wire.read() << 8 | Wire.read();    //z軸の加速度の読み取り 16bit
  Temp  = Wire.read() << 8 | Wire.read(); //温度の読み取り 16bit
  gx = Wire.read() << 8 | Wire.read();    //x軸の角速度の読み取り 16bit
  gy = Wire.read() << 8 | Wire.read();    //y軸の角速度の読み取り 16bit
  gz = Wire.read() << 8 | Wire.read();    //z軸の角速度の読み取り 16bit
  // omajinai end
  
  double ax, ay, az, axCoefficient, ayCoefficient, azCoefficient, rawX_deg, rawY_deg;
  double X_deg, Y_deg;

  // 係数
  axCoefficient = 16384.0;
  ayCoefficient = 16384.0;
  azCoefficient = 13200.0;
  
  ax = axInt / axCoefficient;
  ay = ayInt / ayCoefficient;
  az = azInt / azCoefficient;
  
  rawX_deg = atan2(ay, 1);
  rawY_deg = atan2(ax, 1);

  // 加速度から算出したX軸Y軸の角度 (degree)
  X_deg = rawX_deg * 115.38;
  Y_deg = rawY_deg * 115.38 - 6;
  

  //データの表示 1LSBの値はデータシートに記載
  //AFS_SEL設定 = 0, ±2g, 16384LSB/g
  //FS_SEL設定  = 0, ±250deg/s, 131LSB/deg/s 
  // Serial.print(ax); Serial.print(" g,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  // Serial.print(ay); Serial.print(" g,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  // Serial.print(az); Serial.print(" g,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  // Serial.print(X_deg); Serial.print("deg,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  // Serial.print(Y_deg); Serial.print("deg,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  //Serial.print(gx/131.0); Serial.print(" deg/s,  ");   //1LSBを角速度(deg/s)に換算してシリアルモニタに表示
  //Serial.print(gy/131.0); Serial.print(" deg/s,  ");   //1LSBを角速度(deg/s)に換算してシリアルモニタに表示
  //Serial.print(gz/131.0); Serial.println(" deg/s,  "); //1LSBを角速度(deg/s)に換算してシリアルモニタに表示

  
  signalToLED(X_deg, Y_deg);

  delay(MS);
}

// Function Define

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

void toSignal(double X_deg, double Y_deg) {
  if(-GT <= X_deg && X_deg <= GT) { 
    X_deg = 0;
  }

  if(-GT <= Y_deg && Y_deg <= GT) { 
    Y_deg = 0;
    
  }

  //Serial.print(X_deg); Serial.print("deg,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  //Serial.print(Y_deg); Serial.print("deg,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示

  if(X_deg == 0 && Y_deg <= -GT) {
    signal = 0;
  } else if(X_deg >= GT && Y_deg <= -GT) {
    signal = 1;
  } else if(X_deg >= GT && Y_deg == 0) {
    signal = 2;
  } else if(X_deg >= GT && Y_deg >= GT) {
    signal = 3;
  } else if(X_deg == 0 && Y_deg >= GT) {
    signal = 4;
  } else if(X_deg <= -GT && Y_deg >= GT){
    signal = 5;
  } else if(X_deg <= -GT && Y_deg == 0) {
    signal = 6;
  } else if(X_deg <= -GT && Y_deg <= -GT) {
    signal = 7;
  } else {
    signal = -1;
  }

  if(signal >= 0) {
    onLED(signal);
  } else {
    offAllLED();
  }
}