#include <M5Stack.h> //0.3.9

#include <Arduino.h>
#include <Wire.h>

//加速度センサ
// https://shizenkarasuzon.hatenablog.com/entry/2019/02/16/162647
//  MPU-6050のアドレス、レジスタ設定値
#define MPU6050_WHO_AM_I 0x75   // Read Only
#define MPU6050_PWR_MGMT_1 0x6B // Read and Write
#define MPU_ADDRESS 0x68

//加速度値
float acc_x = 0;
float acc_y = 0;
// float acc_z = 0;
float x = 0;
float y = 0;

float speed_x = 30;
float speed_y = -30;
void printLCD()
{
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(3);
  m5.Lcd.clear();
  M5.Lcd.println(acc_x);
  M5.Lcd.println(acc_y);
  M5.Lcd.println(x);
  M5.Lcd.println(y);

  M5.Lcd.setTextSize(5);

  x += acc_x * speed_x;
  y += acc_y * speed_y;
  x = max((int)x, 10);
  y = max((int)y, 10);
  x = min((int)x, 300);
  y = min((int)y, 230);

  M5.Lcd.fillCircle((int)x, (int)y, 10, GREEN); // x,y,r,color
  // M5.Lcd.fillRect(100, 100, 20, 20, YELLOW);
}

void initMPU6050()
{
  // 初回の読み出し
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(MPU6050_WHO_AM_I); // MPU6050_PWR_MGMT_1
  Wire.write(0x00);
  Wire.endTransmission();

  // 動作モードの読み出し
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(MPU6050_PWR_MGMT_1); // MPU6050_PWR_MGMT_1レジスタの設定
  Wire.write(0x00);
  Wire.endTransmission();
}
void setup()
{
  Wire.begin();

  // PCとの通信を開始
  Serial.begin(115200); // 115200bps
  initMPU6050();

  M5.begin(true, false, true);
  M5.Power.begin();
  // LCDに表示
  m5.Lcd.fillScreen(BLACK);
  m5.Lcd.setTextColor(YELLOW);
  m5.Lcd.setTextSize(5);
  m5.Lcd.setCursor(0, 0);
}
long count = 0;
void loop()
{
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);
  while (Wire.available() < 14)
  {
  }
  int16_t axRaw, ayRaw, azRaw;

  axRaw = Wire.read() << 8 | Wire.read();
  ayRaw = Wire.read() << 8 | Wire.read();
  azRaw = Wire.read() << 8 | Wire.read();

  // 加速度値を分解能で割って加速度(G)に変換する
  //センサーの取り付け状態で向きを合わせる
  acc_x = azRaw / 16384.0;
  acc_y = -axRaw / 16384.0;
  // acc_z = azRaw / 16384.0;

  Serial.print(acc_x);
  Serial.print(" , ");
  Serial.print(acc_y);

  Serial.println("");
  printLCD();
  delay(100);
  if (count % 20 == 0)
    initMPU6050();
  count += 1;
}