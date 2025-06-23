#include "snake.h"

Snake::Snake() {
  for (int i = 0; i < LED_IN_ROW; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], ROW_OFF);

    pinMode(cols[i], OUTPUT);
    digitalWrite(cols[i], COL_OFF);
  }

  Start();
}

void Snake::Start() {
  length = 1;
  snake[0] = { LED_IN_ROW / 2, LED_IN_ROW / 2 };  // 중심에서 시작
  dx = 1;
  dy = 0;
  PlaceApple();
}

void Snake::Stop() {
  length = dy = 0;
  dx = 1;
  apple.x = apple.y = -1;
}

// 사과 배치
void Snake::PlaceApple() {
  while (true) {
    apple.x = random(0, LED_IN_ROW);
    apple.y = random(0, LED_IN_ROW);
    bool overlap = false;
    for (int i = 0; i < length; i++) {
      if (snake[i].x == apple.x && snake[i].y == apple.y) {
        overlap = true;
        break;
      }
    }
    if (!overlap) break;
  }
}

// 충돌
bool Snake::IsCollision(int x, int y) {
  if (x < 0 || x >= LED_IN_ROW || y < 0 || y >= LED_IN_ROW)
    return true;

  for (int i = 0; i < length; i++) {
    if (snake[i].x == x && snake[i].y == y)
      return true;
  }
  return false;
}

// 반대 방향 체크
bool Snake::IsOpposite(int dirX, int dirY) {
  return (dx == -dirX && dy == -dirY);
}

// 이동
bool Snake::Next(int dirX, int dirY) {
  if (dirX && dirY) {
    if (dirX != dx) dirY = 0;  // X축이 새 입력
    else dirX = 0;             // Y축이 새 입력
  }

  if ((dirX || dirY) && !IsOpposite(dirX, dirY)) {
    dx = dirX;
    dy = dirY;
  }

  int newX = snake[0].x + dx;
  int newY = snake[0].y + dy;

  if (IsCollision(newX, newY)) return false;  // 게임 종료

  for (int i = length; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0] = { newX, newY };

  if (newX == apple.x && newY == apple.y) {
    length++;
    PlaceApple();
  }

  return true;
}

void Snake::Refresh() {
  RenderFrame();
}

void Snake::RenderFrame() {
  for (int r = 0; r < LED_IN_ROW; r++) {

    digitalWrite(rows[r], ROW_ON);

    for (int c = 0; c < LED_IN_ROW; c++) {
      bool on = false;

      if (apple.x == c && apple.y == r) on = true;  // 사과
      else if (IsOccupied(c, r)) on = true;         // 뱀 몸통

      digitalWrite(cols[c], on ? COL_ON : COL_OFF);
    }

    delayMicroseconds(1500);  // 1.5 ms

    for (int c = 0; c < LED_IN_ROW; c++)
      digitalWrite(cols[c], COL_OFF);

    digitalWrite(rows[r], ROW_OFF);
  }
}

// 스네이크 몸통 체크
bool Snake::IsOccupied(int x, int y) {
  for (int i = 0; i < length; i++) {
    if (snake[i].x == x && snake[i].y == y)
      return true;
  }
  return false;
}

// 사과 위치
bool Snake::IsAppleAt(int x, int y) {
  return (apple.x == x && apple.y == y);
}

int Snake::GetLength() {
  return length;
}

int Snake::GetDX()    { return dx; }
int Snake::GetDY()    { return dy; }
int Snake::GetHeadX() { return snake[0].x; }
int Snake::GetHeadY() { return snake[0].y; }
int Snake::GetAppleX(){ return apple.x; }
int Snake::GetAppleY(){ return apple.y; }
