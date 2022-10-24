#include <Wire.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int MS = 100;

// Shift Resister Pin
int t_rclkPin = 3, t_srclkPin = 4, t_dsPin = 2;     
int p_rclkPin = 11, p_srclkPin = 10, p_dsPin = 12;   

// Gyro Sensor Pin & Constant
int GT = 20;  // GyroThreshold: ジャイロセンサーとPosition LEDの閾値
int p_signal;   // Gyro SensorからP-LEDに送る信号, 
int t_signal = 8 + (rand() % 8);   // ランダムに光らせるT-LEDに送る信号
double ax, ay, az, axCoefficient, ayCoefficient, azCoefficient, rawX_deg, rawY_deg;
double X_deg, Y_deg;

// Button Signal
int button = 1;

// Send Arduino Value
int STATE = 2;
double TIME = 30;
int POINT = 0;

// Define Function
void offAllLED(int led);    // P-LED: 0, T-LED: 1
void onLED(int position);   // P-LED: 0~7, T-LED: 8~15
void signalToLED(double X_deg, double Y_deg);
void getDeg();
void updateSignal();

void setup() {
  
  srand((unsigned int)time(NULL));

  pinMode(t_rclkPin, OUTPUT);  
  pinMode(t_dsPin, OUTPUT);   
  pinMode(t_srclkPin, OUTPUT);  

  pinMode(p_rclkPin, OUTPUT);  
  pinMode(p_dsPin, OUTPUT);   
  pinMode(p_srclkPin, OUTPUT);  

  // おまじないスタート
	Serial.begin(9600);				//シリアル通信のデータ転送レートを設定しポート開放
  
	Wire.begin();					//I2C通信開始
	// センサ開始動作 
	Wire.beginTransmission(0x68);	//アドレス0x68指定でMPU-6050を選択、送信処理開始
	Wire.write(0x6B);				//MPU6050_PWR_MGMT_1レジスタのアドレス指定
	Wire.write(0x00);				//0を書き込むことでセンサ動作開始
	Wire.endTransmission();			//スレーブデバイスに対する送信を完了
	// おまじないエンド
}

/*
　　　　　　　　　　||￣￣￣￣￣￣￣￣￣￣||
　　　　　　　　　　||ジャイロセンサ     ||
　　　　　　　　　　||　ズレたら　　 ｡　　　∧_∧　　いいですね。
　　　　　　　　　　||　　　修正！　　＼　(ﾟДﾟ ,,)
　　　　　　　　　　||＿＿＿＿＿＿＿＿⊂⊂　}
　　∧ ∧　　　 ∧ ∧　　　 ∧ ∧　　　 |￣￣￣￣|
　 (　　∧ ∧　(　　 ∧ ∧　(　　∧ ∧　|　　　　　　|
～（＿(　　∧ ∧ __(　　∧ ∧__(　　 ∧ ∧￣￣￣
　　～（＿(　　∧ ∧＿(　　∧ ∧＿(　　 ∧ ∧　　は～い、先生。
　　　　～（＿(　　 ,,)～（＿(　　 ,,)～（＿(　　 ,,)
　　　　　　～（＿__ﾉ　　～（＿__ ﾉ　　～（＿__ﾉ
*/

void loop() 
{
  if(STATE == 0) {

  } else if(STATE == 1) {
    
  } else if(STATE == 2) {
    // While Game
    getDeg();
    signalToLED(X_deg, Y_deg);
    TIME -= 0.1;

    if(p_signal == t_signal % 8) {
      updateSignal();
      POINT++;
    } else {
      onLED(t_signal);
    }

    if(TIME <= 0.0) {
      STATE = 0;
      TIME = 30.0;      
      POINT = 0;      
    }
  }

  Serial.print(STATE); Serial.print(",");
  Serial.print(TIME); Serial.print(",");
  Serial.println(POINT);

  delay(MS);
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

void signalToLED(double X_deg, double Y_deg) {
  if(-GT <= X_deg && X_deg <= GT) { 
    X_deg = 0;
  }

  if(-(GT / 2) <= Y_deg && Y_deg <= (GT / 2)) { 
    Y_deg = 0;
    
  }

  //Serial.print(X_deg); Serial.print("deg,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  //Serial.print(Y_deg); Serial.print("deg,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示

  if(X_deg == 0 && Y_deg <= -GT) {
    p_signal = 0;
  } else if(X_deg >= GT / 2 && Y_deg <= -(GT / 2)) {
    p_signal = 1;
  } else if(X_deg >= GT && Y_deg == 0) {
    p_signal = 2;
  } else if(X_deg >= GT / 2 && Y_deg >= GT / 2) {
    p_signal = 3;
  } else if(X_deg == 0 && Y_deg >= GT) {
    p_signal = 4;
  } else if(X_deg <= -(GT / 2) && Y_deg >= GT / 2){
    p_signal = 5;
  } else if(X_deg <= -GT && Y_deg == 0) {
    p_signal = 6;
  } else if(X_deg <= -(GT / 2) && Y_deg <= -(GT / 2)) {
    p_signal = 7;
  } else {
    p_signal = -1;
  }

  if(p_signal >= 0) {
    onLED(p_signal);
  } else {
    offAllLED(0);
  }
}

void getDeg() {
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
}

void updateSignal() {
  t_signal = 8 + (rand() % 8);
}









