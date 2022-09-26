#include <Wire.h>
#define MS 10

void setup() {
  // omajinai begin
  Serial.begin(115200); //シリアル通信のデータ転送レートを設定しポート開放
  Serial.println("--- Started ---");
  
  Wire.begin();         //I2C通信開始
  // センサ開始動作 
  Wire.beginTransmission(0x68); //アドレス0x68指定でMPU-6050を選択、送信処理開始
  Wire.write(0x6B);             //MPU6050_PWR_MGMT_1レジスタのアドレス指定
  Wire.write(0x00);             //0を書き込むことでセンサ動作開始
  Wire.endTransmission();       //スレーブデバイスに対する送信を完了
  // omajinai end
}

float cur_gx = 0, pre_gx = 0, dx_deg = 0, x_deg = 0;

void loop() {
  
  // omajinai begin
  // スタートアドレスの設定とデータの要求処理
  Wire.beginTransmission(0x68);     //アドレス0x68指定でMPU-6050を選択、送信処理開始
  Wire.write(0x3B);                 //ACCEL_XOUT_Hレジスタのアドレス指定
  Wire.endTransmission(false);      //false設定で接続維持
  Wire.requestFrom(0x68, 14, true); //MPU-6050に対して14byte分データを要求、I2Cバス開放
  
  int16_t ax, ay, az, gx, gy, gz, Temp; //符号付き整数を格納する16ビットの変数型の設定

  //シフト演算と論理和で16bitのデータを変数に格納
  //ax～gzまで、16bit(2byte) × 7 = 14byte
  ax = Wire.read() << 8 | Wire.read();    //x軸の加速度の読み取り 16bit
  ay = Wire.read() << 8 | Wire.read();    //y軸の加速度の読み取り 16bit
  az = Wire.read() << 8 | Wire.read();    //z軸の加速度の読み取り 16bit
  Temp  = Wire.read() << 8 | Wire.read(); //温度の読み取り 16bit
  gx = Wire.read() << 8 | Wire.read();    //x軸の角速度の読み取り 16bit
  gy = Wire.read() << 8 | Wire.read();    //y軸の角速度の読み取り 16bit
  gz = Wire.read() << 8 | Wire.read();    //z軸の角速度の読み取り 16bit
  // omajinai end
  
  cur_gx = gx / (float)131.0;
  dx_deg = (cur_gx - pre_gx) * (1.0 / MS);
  x_deg += dx_deg;

  //データの表示 1LSBの値はデータシートに記載
  //AFS_SEL設定 = 0, ±2g, 16384LSB/g
  //FS_SEL設定  = 0, ±250deg/s, 131LSB/deg/s 
  //Serial.print(ax/16384.0); Serial.print(" g,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  //Serial.print(ay/16384.0); Serial.print(" g,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  //Serial.print(az/16384.0); Serial.print(" g,  ");     //1LSBを加速度(G)に換算してシリアルモニタに表示
  Serial.print(cur_gx); Serial.print(" deg/s,  ");   //1LSBを角速度(deg/s)に換算してシリアルモニタに表示
  Serial.print(pre_gx); Serial.print(" deg/s,  ");
  Serial.print(x_deg); Serial.print(" deg,  ");
  Serial.print(dx_deg); Serial.print(" deg \n");
  //Serial.print(gy/131.0); Serial.print(" deg/s,  ");   //1LSBを角速度(deg/s)に換算してシリアルモニタに表示
  //Serial.print(gz/131.0); Serial.println(" deg/s,  "); //1LSBを角速度(deg/s)に換算してシリアルモニタに表示

  pre_gx = cur_gx;

  delay(MS);
}
