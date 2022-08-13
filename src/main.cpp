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
#define stepPeriod 30 //画面更新周期
float BLOCKSIZE = 21;
int BALLSIZE = 7;
long count = 0;
//加速度値
float acc_x = 0;
float acc_y = 0;
// float acc_z = 0;
//初期位置
float x = BLOCKSIZE * 1.2;
float y = BLOCKSIZE * 1.2;
//速度
float speed_x = 3;
float speed_y = -3;
float touch_level = 4;

int maze[MEIRO_HEIGHT][MEIRO_WIDTH];

void printBackground(int x, int y)
{
  //迷路を描画
  M5.Lcd.setTextSize(5);
  int h_start = max((int)(y / BLOCKSIZE - 1), 0);
  int w_start = max((int)(x / BLOCKSIZE - 1), 0);

  for (int j = h_start; j < h_start + 3; j++)
  {
    for (int i = w_start; i < w_start + 3; i++)
    {
      if (maze[j][i] == 1)
      {
        if ((i + j) % 2 == 0)
        {
          M5.Lcd.fillRect(i * BLOCKSIZE, j * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, ORANGE); // x-pos,y-pos,x-size,y-size,color //orange 0xFD20
        }
        else
        {
          M5.Lcd.fillRect(i * BLOCKSIZE, j * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, YELLOW);
        }
      }
      else
      {
        M5.Lcd.fillRect(i * BLOCKSIZE, j * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, BLACK);
      }
    }
  }
}

void moveBall()
{
  M5.Lcd.setCursor(0, 0);

  float isOk_x = 1;
  float isOk_y = 1;
  float h_start = (y / BLOCKSIZE - 1);
  float w_start = (x / BLOCKSIZE - 1);

  if (h_start < 0)
    h_start = 0;
  else if (h_start > MEIRO_HEIGHT - 3)
    h_start = MEIRO_HEIGHT - 3;
  if (w_start < 0)
    w_start = 0;
  else if (w_start > MEIRO_WIDTH - 3)
    w_start = MEIRO_WIDTH - 3;

  for (float j = h_start; j < h_start + 3; j += 0.5)
  {
    for (float i = w_start; i < w_start + 3; i += 0.5)
    {
      if (maze[(int)(j)][(int)(i)] == 1)
      {
        if ((j * BLOCKSIZE - y) * (j * BLOCKSIZE - y) + (i * BLOCKSIZE - x - acc_x * speed_x) * (i * BLOCKSIZE - x - acc_x * speed_x) < BLOCKSIZE * BLOCKSIZE / touch_level)
        {
          isOk_x = 0;
        }
        if ((j * BLOCKSIZE - y - acc_y * speed_y) * (j * BLOCKSIZE - y - acc_y * speed_y) + (i * BLOCKSIZE - x) * (i * BLOCKSIZE - x) < BLOCKSIZE * BLOCKSIZE / touch_level)
        {
          isOk_y = 0;
        }
      }
    }
  }
  x += (acc_x * speed_x) * isOk_x;
  y += (acc_y * speed_y) * isOk_y;
  if (count % 4 == 0)
  {
    Serial.print(acc_x * speed_x);
    Serial.print("  ");
    Serial.print(isOk_x);
    Serial.print("  ");
    Serial.print((acc_x * speed_x) * isOk_x);
    Serial.print("  ");
    Serial.print(acc_y * speed_y);
    Serial.print("  ");
    Serial.print(isOk_y);
    Serial.print("  ");
    Serial.println((acc_y * speed_y) * isOk_y);
  }

  if (x < 10)
    x = 10;
  else if (x > 300)
    x = 300;
  if (y < 10)
    y = 10;
  else if (y > 220)
    y = 220;

  //ボールの軌跡を消す
  printBackground(x, y);
  //ボールの位置がズレているのでoffset（現物合わせ）
  M5.Lcd.fillCircle((int)x + 1, (int)y + 1, BALLSIZE, GREEN); // x,y,r,color
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
          M5.Lcd.fillRect(i * BLOCKSIZE, j * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, ORANGE); // x-pos,y-pos,x-size,y-size,color //orange 0xFD20
        }
        else
        {
          M5.Lcd.fillRect(i * BLOCKSIZE, j * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, YELLOW);
        }
      }
    }
  }
}

void dispSpeed()
{
  m5.Lcd.setTextColor(RED);
  m5.Lcd.setCursor(300, 100);
  M5.Lcd.print("SPEED ");
  float sp = (abs(speed_x) + abs(speed_y)) / 2;

  M5.Lcd.print(sp);
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

unsigned long pre = 0;

void loop()
{
  M5.update(); //ボタンを読み取る
  while (micros() - pre < stepPeriod * 1000)
  {
  }
  pre = micros();

  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);

  int16_t axRaw, ayRaw, azRaw;

  axRaw = Wire.read() << 8 | Wire.read();
  ayRaw = Wire.read() << 8 | Wire.read();
  azRaw = Wire.read() << 8 | Wire.read();

  // 加速度値を分解能で割って加速度(G)に変換する
  //センサーの取り付け状態で向きを合わせる
  acc_x = azRaw / 16384.0;
  acc_y = -axRaw / 16384.0;
  // acc_z = azRaw / 16384.0;

  moveBall();

  if (count % 40 == 0)
  {
    initMPU6050();
  }
  count += 1;

  if (M5.BtnA.wasReleased() || M5.BtnB.pressedFor(1000, 200))
  {
    speed_x *= 1.111;
    speed_y *= 1.111;
    Serial.println(speed_x);
    dispSpeed();
    delay(200);
  }
  if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200))
  {
    speed_x *= 0.9;
    speed_y *= 0.9;
    Serial.println(speed_x);
    dispSpeed();
    delay(200);
  }
  if (M5.BtnC.wasReleased() || M5.BtnB.pressedFor(1000, 200))
  {
    initStage();
  }
}
