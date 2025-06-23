#ifndef SNAKE_H
#define SNAKE_H

#include "Arduino.h"

#define LED_IN_ROW 8
#define ROW_ON HIGH
#define ROW_OFF LOW
#define COL_ON LOW
#define COL_OFF HIGH

class Snake {
private:
  const int rows[LED_IN_ROW] = { 37, 32, 29, 7, 22, 28, 23, 26 }, cols[LED_IN_ROW] = { 33, 24, 53, 36, 27, 35, 31, 30 };

  struct Point {
    int x;
    int y;
  };

  Point snake[LED_IN_ROW * LED_IN_ROW];
  int length;
  Point apple;

  int dx, dy;

  void PlaceApple();
  bool IsCollision(int x, int y);
  bool IsOpposite(int x, int y);
  void RenderFrame();
  bool IsOccupied(int x, int y);
  bool IsAppleAt(int x, int y);

public:
  Snake();
  void Start();
  void Stop();
  bool Next(int dirX, int dirY);  // 방향: dx, dy
  int GetLength();
  void Refresh();
  int GetDX();  // 진행 방향
  int GetDY();
  int GetHeadX();  // 머리 좌표
  int GetHeadY();
  int GetAppleX();
  int GetAppleY();
};

#endif