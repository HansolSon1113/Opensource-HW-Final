#include "buzz.h"
#include "led.h"
#include "cds.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "snake.h"
#include "joystick.h"
#include "timer.h"
#include <WiFiEsp.h>


#define BUZZER_PIN 10
#define LOCK_LED_PIN 12
#define PLAY_LED_PIN 13
#define CDS_PIN A0
#define BTN_PIN 2
#define JOYSTICK_X_PIN A14
#define JOYSTICK_Y_PIN A15
#define BALL_PIN 9

#define Wifi Serial1
#define Bluetooth Serial2
#define JOYSTICK_DBNC 200
#define LIGHT_THRESHOLD 5
#define STEP_MS 1000
#define REFRESH_MS 15

enum GameState { PLAYING,
                 PAUSED,
                 LOBBY };
enum MENU { RESUME,
            TO_LOBBY };

Buzzer buzzer(BUZZER_PIN);
LED lockLED(LOCK_LED_PIN);
LED playLED(PLAY_LED_PIN);
CDS cds(CDS_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Snake snake;
Joystick joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);
WiFiEspClient client;

const char *ssid = "HansoliPhone";
const char *pwd = "6729Aa++1";
char *server = "api.thingspeak.com";
String apiKey = "AFH5C5BXIQITEV9Q";

volatile bool btnEdge = false;
volatile unsigned long btnTime = 0;

bool menu = false;
MENU selection = RESUME;
GameState gameState = LOBBY;
GameState beforeLock = LOBBY;
bool lockGame = false;
bool tempLocked = false;
int pendX = 0, pendY = 0;

static unsigned long lastLCD = 0;
static unsigned long lastStep = 0;
static unsigned long lastRef = 0;

unsigned long playStart = 0;
unsigned long playTime = 0;
unsigned long screenTime = 0;
uint8_t darkCnt = 0, poseCnt = 0;

char btMsg[17] = "";
int btLen = 0;

void Lobby() {
  if (millis() - lastLCD < 200) return;
  lastLCD = millis();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Arduino Snake GM");
  lcd.setCursor(3, 1);
  lcd.print("->START<-");
}

void ToLobby() {
  menu = false;
  selection = RESUME;
  gameState = LOBBY;
  beforeLock = LOBBY;
  btLen = 0;
  btMsg[0] = '\0';

  snake.Stop();
  lastLCD = 0;
  Lobby();
}

void LockGame() {
  if (!tempLocked) {
    if (gameState == PLAYING) {
      beforeLock = PLAYING;
    }
  }
  tempLocked = true;

  lockLED.On();
  playLED.Off();
  buzzer.Play();
  gameState = PAUSED;
  menu = false;
  lastLCD = 0;
}

void UnLockGame() {
  lockLED.Off();
  buzzer.Stop();
  tempLocked = false;

  if (!lockGame && beforeLock == PLAYING && !menu) {
    gameState = PLAYING;
    lastStep = millis();
    pendX = pendY = 0;
    playLED.On();
  } else {
    gameState = LOBBY;
    playLED.Off();
    lcd.clear();
    lastLCD = 0;
    if (menu) {
      MenuHandler();
    } else {
      Lobby();
    }
  }

  lastLCD = 0;
}

void MenuHandler() {
  static unsigned long lastMove = 0;

  playLED.Off();

  int dx = joystick.ReadX();
  if (millis() - lastMove > JOYSTICK_DBNC) {
    if (dx == -1) selection = RESUME;
    else if (dx == 1) selection = TO_LOBBY;
    if (dx) lastMove = millis();
  }

  if (millis() - lastLCD < 500) return;
  lastLCD = millis();

  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("MENU");
  lcd.setCursor(0, 1);
  if (selection == RESUME) lcd.print("->RESUME<- LOBBY");
  else lcd.print(" RESUME ->LOBBY<-");
}

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), BtnInterrupt, FALLING);
  pinMode(BALL_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  Serial.begin(9600);
  Wifi.begin(9600);
  Bluetooth.begin(9600);

  WiFi.init(&Wifi);

  Serial.print("WIFI: Connecting to \'");
  Serial.print(ssid);
  Serial.println("\'");

  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWIFI: AP-info");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Lobby();
}

void LoopStartPlaying() {
  menu = false;
  tempLocked = false;
  lockGame = false;
  selection = RESUME;

  gameState = PLAYING;
  snake.Start();
  playStart = millis();
  lastStep = millis() + 500;  // 0.5초 뒤부터 이동 시작

  playLED.On();
}

void loop() {
  Serial.print("CDS=");
  Serial.print(cds.Read());
  Serial.print("  BALL=");
  Serial.print(digitalRead(BALL_PIN));
  Serial.print("  lockGame=");
  Serial.println(lockGame);

  unsigned long btStart = millis();
  while (Bluetooth.available()) {
    char c = Bluetooth.read();
    Serial.print("BT: ");
    Serial.println(c);
    if (c == '0') {
      lockGame = true;
      btLen = 0;
      btMsg[0] = '\0';
    } else if (c == '1') {
      lockGame = false;
      btLen = 0;
      btMsg[0] = '\0';
    } else if (lockGame && btLen < 16) {
      btMsg[btLen++] = c;
      btMsg[btLen] = '\0';
      lastLCD = 0;
    }

    if (millis() - btStart > 5) break;  // 5 ms 이상이면 탈출
  }

  darkCnt = (cds.Read() < LIGHT_THRESHOLD) ? min(darkCnt + 1, 3) : max(darkCnt - 1, 0);
  poseCnt = (digitalRead(BALL_PIN) == LOW) ? min(poseCnt + 1, 3) : max(poseCnt - 1, 0);
  bool sensorLocked = gameState != LOBBY && (darkCnt >= 3 || poseCnt >= 3);

  bool isLocked = lockGame || sensorLocked;

  static bool prevLocked = false;
  if (isLocked && !prevLocked) {
    LockGame();
  } else if (!isLocked && prevLocked) {
    UnLockGame();
  }
  prevLocked = isLocked;

  static uint8_t prevLen = 0;
  if (isLocked) {
    if (millis() - lastLCD > 300 || btLen != prevLen) {
      lastLCD = millis();
      prevLen = btLen;

      lcd.clear();
      lcd.setCursor(0, 0);
      if (lockGame) {
        lcd.print("LOCKED BY APP!");
        lcd.setCursor(0, 1);
        lcd.print(btMsg);
      } else {
        lcd.print("ALERT!");
        lcd.setCursor(0, 1);
        lcd.print("Fix Light/Pose");
      }
    }
  }

  if ((gameState == PLAYING || gameState == PAUSED) && millis() - lastRef >= REFRESH_MS) {
    lastRef = millis();
    snake.Refresh();
  }

  if (btnEdge) {
    btnEdge = false;
    switch (gameState) {
      case LOBBY:
        LoopStartPlaying();
        break;

      case PLAYING:  // 메뉴 진입
        gameState = PAUSED;
        menu = true;
        selection = RESUME;
        lastLCD = 0;
        tempLocked = false;
        break;

      case PAUSED:
        if (menu) {
          if (selection == RESUME) {
            gameState = PLAYING;
            menu = false;
            selection = RESUME;
            lastStep = millis();
          } else {
            ToLobby();
          }
        }
        break;
    }
  }

  int jx = joystick.ReadX();
  int jy = joystick.ReadY();
  if (!menu && jx || jy) {  // 0 이 아니면 새 입력
    pendX = jx;
    pendY = jy;
  }

  if (gameState == PLAYING && millis() - lastStep >= STEP_MS) {
    lastStep = millis();
    Serial.print(pendX);
    Serial.print("|");
    Serial.println(pendY);
    if (!snake.Next(pendX, pendY)) {
      GameOver();
      return;
    }
    pendX = pendY = 0;
  }

  if (gameState == PLAYING && millis() - lastLCD >= 500) {
    lastLCD = millis();
    int snakeLen = snake.GetLength();
    unsigned long sec = (millis() - playStart) / 1000;

    lcd.setCursor(0, 0);
    lcd.print("Playing...      ");
    lcd.setCursor(0, 1);
    char line[17];
    snprintf(line, sizeof(line), "L:%2d T:%3lus      ", snakeLen, sec);
    lcd.setCursor(0, 1);
    lcd.print(line);

    static unsigned long lastDBG = 0;
    if (millis() - lastDBG >= 200) {  // 5 Hz
      lastDBG = millis();

      Serial.print("STATE:");
      Serial.print(gameState == LOBBY ? "LOBBY" : gameState == PLAYING ? "PLAY"
                                                                       : "PAUSE");

      Serial.print("  LKD:");
      Serial.print(lockGame);
      Serial.print("/");
      Serial.print(tempLocked);

      Serial.print("  Len:");
      Serial.print(snake.GetLength());
      Serial.print("  Head(");
      Serial.print(snake.GetHeadX());
      Serial.print(",");
      Serial.print(snake.GetHeadY());
      Serial.print(")");

      Serial.print(" Dir(");
      Serial.print(snake.GetDX());
      Serial.print(",");
      Serial.print(snake.GetDY());
      Serial.print(")");

      Serial.print(" Apple(");
      Serial.print(snake.GetAppleX());
      Serial.print(",");
      Serial.print(snake.GetAppleY());
      Serial.print(")");

      Serial.print(" pend(");
      Serial.print(pendX);
      Serial.print(",");
      Serial.print(pendY);
      Serial.print(")");

      Serial.println();
    }
  }

  if (menu) MenuHandler();
}

void GameOver() {
  playLED.Off();
  Serial.println("GAME OVER!");
  lcd.setCursor(0, 0);
  lcd.print("   Game Over!   ");
  playTime = (millis() - playStart) / 1000;
  screenTime += playTime;

  if (client.connect(server, 80)) {
    Serial.println("Connecting to ThingSpeak...");
    String url = "/update?api_key=" + apiKey + "&field1=" + String(snake.GetLength()) + "&field2=" + String(playTime) + "&field3=" + String(screenTime);
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();
    delay(1000);

    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    client.stop();
    Serial.println("Upload Success!");
  } else {
    Serial.println("Failed to Connect to ThingSpeak!");
  }
  ToLobby();
}

void BtnInterrupt() {
  unsigned long now = millis();
  if (now - btnTime > 100) {
    btnEdge = true;
    btnTime = now;
  }
}
