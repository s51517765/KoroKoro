#include <M5Stack.h> //0.3.9
#include <Arduino.h>
#include <Wire.h>
#include "maze_.h"

//加速度センサ
// https://shizenkarasuzon.hatenablog.com/entry/2019/02/16/162647
//  MPU-6050のアドレス、レジスタ設定値
#define MPU6050_WHO_AM_I 0x75   // Read Only
#define MPU6050_PWR_MGMT_1 0x6B // Read and Write
#define MPU_ADDRESS 0x68
int blockSize = 20;
int ballSize = 7;

//加速度値
float acc_x = 0;
float acc_y = 0;
// float acc_z = 0;
//初期位置
float x = blockSize * 1.5;
float y = blockSize * 1.5;
//速度
float speed_x = 10;
float speed_y = -10;

int maze[11][13];

void printLCD()
{
  M5.Lcd.setCursor(0, 0);
  // M5.Lcd.setTextSize(3);
  // m5.Lcd.clear();
  // M5.Lcd.println(acc_x);
  // M5.Lcd.println(acc_y);
  // M5.Lcd.println(x);
  // M5.Lcd.println(y);

  // x += acc_x * speed_x;
  // y += acc_y * speed_y;
  float isOk_x = 1;
  float isOk_y = 1;
  for (int j = 0; j < MEIRO_HEIGHT; j++)
  {
    for (int i = 0; i < MEIRO_WIDTH; i++)
    {
      if (maze[j][i] == 1)
      {
        if ((j * blockSize - y) * (j * blockSize - y) + (i * blockSize - x - acc_x * speed_x) * (i * blockSize - x - acc_x * speed_x) < blockSize * blockSize / 1.6)
        {
          isOk_x = 0;
        }
        if ((j * blockSize - y - acc_y * speed_y) * (j * blockSize - y - acc_y * speed_y) + (i * blockSize - x) * (i * blockSize - x) < blockSize * blockSize / 1.6)
        {
          isOk_y = 0;
        }
      }
    }
  }
  Serial.print(isOk_x, acc_x * speed_x);
  Serial.print("  ");
  Serial.println(isOk_y, acc_y * speed_y);
  x += (acc_x * speed_x) * isOk_x;
  y += (acc_y * speed_y) * isOk_y;

  x = max((int)x, 10);
  y = max((int)y, 10);
  x = min((int)x, 300);
  y = min((int)y, 220);

  //ボールの位置がズレているのでoffset（現物合わせ）
  M5.Lcd.fillCircle((int)x + 7, (int)y + 7, ballSize, GREEN); // x,y,r,color
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

void initStage()
{
  M5.Lcd.setTextSize(3);
  m5.Lcd.clear();
  //迷路配列を取得
  for (int j = 0; j < MEIRO_HEIGHT; j++)
  {
    for (int i = 0; i < MEIRO_WIDTH; i++)
    {
      maze[j][i] = returnMaze(j, i);
    }
  }
  //迷路を描画
  M5.Lcd.setTextSize(5);
  for (int j = 0; j < MEIRO_HEIGHT; j++)
  {
    for (int i = 0; i < MEIRO_WIDTH; i++)
    {
      if (maze[j][i] == 1)
      {
        if ((i + j) % 2 == 0)
        {
          M5.Lcd.fillRect(i * blockSize, j * blockSize, blockSize, blockSize, ORANGE); // x-pos,y-pos,x-size,y-size,color //orange 0xFD20
        }
        else
        {
          M5.Lcd.fillRect(i * blockSize, j * blockSize, blockSize, blockSize, YELLOW);
        }
      }
    }
  }
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
  m5.Lcd.setCursor(0, 0);
  initRand();
  createMaze();
  printMaze();
  initStage();
}

long count = 0;
unsigned long pre = 0;
void loop()
{
  while (micros() - pre < 60 * 1000)
  {
  }
  pre = micros();

  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);
  /* while (Wire.available() < 14)
   {
   }
   */
  int16_t axRaw, ayRaw, azRaw;

  axRaw = Wire.read() << 8 | Wire.read();
  ayRaw = Wire.read() << 8 | Wire.read();
  azRaw = Wire.read() << 8 | Wire.read();

  // 加速度値を分解能で割って加速度(G)に変換する
  //センサーの取り付け状態で向きを合わせる
  acc_x = azRaw / 16384.0;
  acc_y = -axRaw / 16384.0;
  // acc_z = azRaw / 16384.0;

  /*
    Serial.print(acc_x);
    Serial.print(" , ");
    Serial.print(acc_y);
    Serial.println("");
  */
  printLCD();
  // initStage();

  if (count % 40 == 0)
  {
    initMPU6050();
    // initStage();
  }
  count += 1;

  if (M5.BtnA.wasPressed())
  {
    speed_x -= 5;
    speed_y -= 5;
    Serial.println(speed_x);
  }
  if (M5.BtnC.wasPressed())
  {
    speed_x += 5;
    speed_y += 5;
    Serial.println(speed_x);
  }
  if (M5.BtnB.wasPressed())
  {
    initStage();
  }
}
