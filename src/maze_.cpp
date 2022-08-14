// https://daeudaeu.com/c_create_maze/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze_.h"
#include <Arduino.h>

/* 迷路の各マス表す値の定義 */
#define PATH 0
#define WALL 1

/* 方向を表す値の定義 */
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

/* 迷路の各マスの情報を格納する配列 */
static int maze[MEIRO_HEIGHT][MEIRO_WIDTH] = {};

int mazeDepth = 0;
int mazeDepthMax = 0;
int maxDepthGoal_xy[2];
void initRand(void)
{
  /* 現在の時刻を取得 */
  long rnd = random(3000);
  printf("random= %d\n", rnd);

  /* 現在の時刻を乱数の種として乱数の生成系列を変更 */
  srand((unsigned int)rnd);
}

void printMaze()
{
  int i, j;

  for (j = 0; j < MEIRO_HEIGHT; j++)
  {
    for (i = 0; i < MEIRO_WIDTH; i++)
    {
      /* 配列の値に応じて記号/文字を表示 */
      if (maze[j][i] == PATH)
      {
        printf(" ");
      }
      if (maze[j][i] == WALL)
      {
        printf("#");
      }
    }
    printf("\n");
  }
  printf("\n");
}

int returnMaze(int j, int i)
{
  return maze[j][i];
}
int returnGoal(int i)
{
  return maxDepthGoal_xy[i];
}

void dig(int i, int j)
{
  /* どの方向を掘ろうとしたかを覚えておく変数 */
  int up, down, left, right;

  up = 0;
  down = 0;
  left = 0;
  right = 0;

  /* 全方向試すまでループ */
  while (up == 0 || down == 0 || left == 0 || right == 0)
  {
    /* 0 - 3 の乱数を取得 */
    int d = rand() % 4;

    switch (d)
    {
    case UP:
      /* 上方向が掘れるなら掘る */
      if (j - 2 >= 0 && j - 2 < MEIRO_HEIGHT)
      {
        if (maze[j - 2][i] == WALL)
        {
          mazeDepth += 1;
          maze[j - 2][i] = PATH;
          maze[j - 1][i] = PATH;
          dig(i, j - 2);
        }
      }
      up++;
      break;
    case DOWN:
      /* 下方向が掘れるなら掘る */
      if (j + 2 >= 0 && j + 2 < MEIRO_HEIGHT)
      {
        if (maze[j + 2][i] == WALL)
        {
          mazeDepth += 1;
          maze[j + 2][i] = PATH;
          maze[j + 1][i] = PATH;
          dig(i, j + 2);
        }
      }
      down++;
      break;
    case LEFT:
      /* 左方向が掘れるなら掘る */
      if (i - 2 >= 0 && i - 2 < MEIRO_WIDTH)
      {
        if (maze[j][i - 2] == WALL)
        {
          mazeDepth += 1;
          maze[j][i - 2] = PATH;
          maze[j][i - 1] = PATH;
          dig(i - 2, j);
        }
      }
      left++;
      break;
    case RIGHT:
      /* 右方向が掘れるなら掘る */
      if (i + 2 >= 0 && i + 2 < MEIRO_WIDTH)
      {
        if (maze[j][i + 2] == WALL)
        {
          mazeDepth += 1;
          maze[j][i + 2] = PATH;
          maze[j][i + 1] = PATH;
          dig(i + 2, j);
        }
      }
      right++;
      break;
    }
  }
  if (mazeDepthMax < mazeDepth)
  {
    mazeDepthMax = mazeDepth;
    maxDepthGoal_xy[0] = i;
    maxDepthGoal_xy[1] = j;
  }
  mazeDepth -= 1;
  Serial.println("Max Depth");
  Serial.print(mazeDepth);
  Serial.print(" ");
  Serial.print(maxDepthGoal_xy[0]);
  Serial.print(" ");
  Serial.println(maxDepthGoal_xy[1]);
}

void createMaze()
{
  int i, j;

  /* 全マス壁にする */
  for (j = 0; j < MEIRO_HEIGHT; j++)
  {
    for (i = 0; i < MEIRO_WIDTH; i++)
    {
      maze[j][i] = WALL;
    }
  }

  /* 開始点を左上にする */
  i = 1;
  j = 1;

  /* i, j を通路に設定 */
  maze[j][i] = PATH;

  /* マス(i,j)を起点に穴を掘る */
  dig(i, j);
}
