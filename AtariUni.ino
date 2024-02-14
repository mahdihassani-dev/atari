#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);


// page Constants
#define PAGE_MAIN 0
#define PAGE_SETTINGS 1
#define PAGE_CONTRAST 2
#define PAGE_GAMES 3
#define PAGE_2048 4
#define PAGE_SNAKE 5

// default values
boolean backlight = false;
boolean isPlaying = false;
boolean wifiConnected = false;

int contrast = 50;
int menuitem = 1;
int page = PAGE_MAIN;

volatile boolean up = false;
volatile boolean down = false;
volatile boolean middle = false;

// joystick pins
int xPin = A0;
int yPin = A1;
int buttonPin = 12;


// led and buzzer pins
int buzzerPin = 8;
int ledPin = 9;

// lcd pins -> (CLK, DIN, DC, CE, RST) : you may changes them based on your need.
Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 10, 11);

//--------------------------2048 values-------------------------------
int selection[4];
int m[4][4];
int rotatetext = 1;
bool moved = false;
bool joystickMove = false;
int direction_2048;

//defintions for the direction of movement with jostick in 2048 game
#define UP_2048 0
#define DOWN_2048 2
#define LEFT_2048 3
#define RIGHT_2048 1
//----------------------------------------------------------

//--------------------------snake values--------------------------------
#define RESOLUTION 4
#define MAXLEN 100
#define UP_SNAKE 0
#define DOWN_SNAKE 1
#define LEFT_SNAKE 2
#define RIGHT_SNAKE 3

#define DISP_HEIGHT 84
#define DISP_WIDTH 48

char direction_snake = RIGHT_SNAKE;
bool game_over = false;

char snakesize = 6;
char snakex[MAXLEN];
char snakey[MAXLEN];

char foodx = -1;
char foody = -1;

char sensorValueX = 0;
char sensorValueY = 0;

//---------------------------------------------------------------------


void setup() {

  pinMode(2, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(7, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(7, HIGH);  //Turn Backlight OFF

  Serial.begin(9600);
  mySerial.begin(9600);

  display.begin();
  display.setContrast(contrast);  //Set contrast to 50
  display.clearDisplay();
  display.display();
}

void loop() {

  if (!wifiConnected) {
    mySerial.write(-1);
    turnOffWifi();
  } else {
    int wifiState = 0;

    if (mySerial.available()) {
      wifiState = mySerial.read();
      // Serial.println(wifiState);
    }

    if (wifiState == 200) {
      turnOnWifi();
    } else {
      blynkWifi();
    }
  }

  drawPage();
  if (!isPlaying) {
    checkDir();
    handleDir();
  }
}

void checkDir() {
  int menuDir = analogRead(yPin);
  boolean button = digitalRead(buttonPin);

  if (menuDir <= 300) {
    down = true;
    delay(150);
  }

  if (menuDir >= 700) {
    up = true;
    delay(150);
  }

  if (button == LOW) {
    middle = true;
    delay(150);
  }
}

void handleDir() {

  // if up pressed
  if (up)
    upDir();

  // if down pressed
  if (down)
    downDir();

  // if button pushed
  if (middle) {
    middleDir();
  }
}

void upDir() {
  up = false;

  switch (page) {

    case PAGE_MAIN:
      menuitem--;
      if (menuitem == 0)
        menuitem = 3;
      break;

    case PAGE_GAMES:
      menuitem--;
      if (menuitem == 0)
        menuitem = 3;
      break;

    case PAGE_SETTINGS:
      menuitem--;
      if (menuitem == 0)
        menuitem = 4;
      break;

    case PAGE_CONTRAST:
      contrast++;
      setContrast();
      break;
  }
}

void downDir() {
  down = false;
  switch (page) {

    case PAGE_MAIN:
      menuitem++;
      if (menuitem == 4)
        menuitem = 1;
      break;

    case PAGE_GAMES:
      menuitem++;
      if (menuitem == 4)
        menuitem = 1;
      break;

    case PAGE_SETTINGS:
      menuitem++;
      if (menuitem == 5)
        menuitem = 1;
      break;

    case PAGE_CONTRAST:
      contrast--;
      setContrast();
      break;
  }
}

void middleDir() {
  middle = false;

  if (page == PAGE_MAIN) {
    handleMainPage();
  }

  else if (page == PAGE_GAMES) {
    handleGamePage();
  }

  else if (page == PAGE_SETTINGS) {
    handleSettingPage();
  }

  else if (page == PAGE_CONTRAST) {
    handleContrastPage();
  }
}

void handleMainPage() {
  if (menuitem == 1) {
    page = PAGE_GAMES;
  } else if (menuitem == 2) {
    if (wifiConnected) {
      wifiConnected = false;
    } else {
      wifiConnected = true;
    }
  } else if (menuitem == 3) {
    page = PAGE_SETTINGS;
    menuitem = 1;
  }
}

void handleGamePage() {
  if (menuitem == 1) {
    size_t const address{ 0 };
    unsigned int seed{};
    EEPROM.get(address, seed);
    randomSeed(seed);
    EEPROM.put(address, seed + 1);

    initializeMatrix();
    page = PAGE_2048;

  } else if (menuitem == 2) {
    display.clearDisplay();
    delay(200);
    display.drawRect(0, 0, 84, 46, BLACK);
    display.setCursor(12, 18);
    display.println("SNAKE GAME");

    display.display();
    delay(200);
    display.clearDisplay();

    init_snake();
    page = PAGE_SNAKE;
  } else if (menuitem == 3) {
    page = PAGE_MAIN;
    menuitem = 1;
  }
}

void handleSettingPage() {
  if (menuitem == 1) {
    page = PAGE_CONTRAST;
  } else if (menuitem == 2) {
    changeBacklight();
  } else if (menuitem == 3) {
    resetDefaults();
  } else if (menuitem == 4) {
    page = PAGE_MAIN;
    menuitem = 1;
  }
}

void handleContrastPage() {
  page = PAGE_SETTINGS;
}

void drawPage() {

  // mainPageDraw
  if (page == PAGE_MAIN) {
    drawMainPage();
  }

  // settingPageDraw
  else if (page == PAGE_SETTINGS) {
    drawSettingPage();
  }

  // contrastPageDraw
  else if (page == PAGE_CONTRAST) {
    drawContrastPage();
  }

  // gamePageDraw
  else if (page == PAGE_GAMES) {
    drawGamePage();
  }

  // 2048PageDraw
  else if (page == PAGE_2048) {
    draw2048Page();
  }

  // SnakePageDraw
  else if (page == PAGE_SNAKE) {
    drawSnakePage();
  }
}

void drawMainPage() {
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print("MAIN MENU");
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(0, 15);

  if (menuitem == 1) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(">Games");
  display.setCursor(0, 25);

  if (menuitem == 2) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(">Wifi:");
  if (wifiConnected) {
    display.print("ON");
  } else {
    display.print("OFF");
  }
  display.display();

  if (menuitem == 3) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.setCursor(0, 35);
  display.print(">Settings");
  display.display();
}

void drawSettingPage() {
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print("SETTINGS");
  display.drawFastHLine(0, 8, 83, BLACK);
  display.setCursor(0, 10);

  if (menuitem == 1) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(">Contrast");
  display.setCursor(0, 20);

  if (menuitem == 2) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(">Light: ");

  if (backlight) {
    display.print("ON");
  } else {
    display.print("OFF");
  }
  display.display();

  if (menuitem == 3) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.setCursor(0, 30);
  display.print(">Reset");
  display.display();

  if (menuitem == 4) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.setCursor(0, 40);
  display.print(">Back to menu");
  display.display();
}

void drawContrastPage() {
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print("CONTRAST");
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print("Value");
  display.setTextSize(2);
  display.setCursor(5, 25);
  display.print(contrast);

  display.setTextSize(2);
  display.display();
}

void drawGamePage() {
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print("GAMES");
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(0, 15);

  if (menuitem == 1) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(">2048");
  display.setCursor(0, 25);

  if (menuitem == 2) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(">Snake");
  display.display();


  if (menuitem == 3) {
    display.setTextColor(WHITE, BLACK);
  } else {
    display.setTextColor(BLACK, WHITE);
  }
  display.setCursor(0, 35);
  display.print(">back to Menu");
  display.display();
}

void draw2048Page() {
  isPlaying = true;

  if (isLoser()) {

    display.setCursor(25, 10);
    display.setTextSize(1);
    int score = getScore();
    display.print("SCORE: ");
    display.println(score);
    display.println(" ");
    display.print("press button");
    display.display();



    while (digitalRead(buttonPin)) {
    }
    mySerial.write(score);
    page = PAGE_GAMES;
  }

  // convertToPower();
  displayTable();
  play();

  if (moved) {
    addRandomNum();
    moved = false;
  }
}

void drawSnakePage() {
  isPlaying = true;

  if (game_over) {
    display.clearDisplay();
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, Low);
    display.display();
    show_score();

    while (digitalRead(buttonPin)) {
      // while not pressed button, do nothing
    }

    // start again
    display.clearDisplay();
    display.display();
    game_over = false;
    snakesize = 6;
    init_snake();
    mySerial.write(snakesize);
  }

  display.clearDisplay();

  //----------------game cycle------------------>

  // 1- move parts based on dir
  movesnake();

  // 2- check food
  food();

  // 3- draw snake
  snake();

  //<--------------------------------------------


  display.display();
  delay(100);
}

void resetDefaults() {
  contrast = 50;
  setContrast();
  backlight = true;
  turnBacklightOn();
}

void setContrast() {
  display.setContrast(contrast);
  display.display();
}

void changeBacklight() {
  if (backlight) {
    backlight = false;
    turnBacklightOff();
  } else {
    backlight = true;
    turnBacklightOn();
  }
}

void turnBacklightOn() {
  digitalWrite(7, LOW);
}

void turnBacklightOff() {
  digitalWrite(7, HIGH);
}

void initializeMatrix() {
  m[0][0] = 0;
  m[0][1] = 0;
  m[0][2] = 0;
  m[0][3] = 0;
  m[1][0] = 0;
  m[1][1] = 0;
  m[1][2] = 0;
  m[1][3] = 0;
  m[2][0] = 0;
  m[2][1] = 0;
  m[2][2] = 0;
  m[2][3] = 0;
  m[3][0] = 0;
  m[3][1] = 0;
  m[3][2] = 0;
  m[3][3] = 0;

  int x1 = random(4);
  int y1 = random(4);

  int x2 = random(4);
  int y2 = random(4);

  while (x1 == x2 && y1 == y2) {
    int x2 = random(4);
    int y2 = random(4);
  }

  m[x1][y1] = 2;
  m[x2][y2] = 2;
}

void addRandomNum() {

  int x, y;

  do {
    x = random(4);
    y = random(4);
  } while (m[x][y] != 0);

  m[x][y] = 2;
}

bool isLoser() {

  // zero exist
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (m[i][j] == 0) {
        return false;
      }
    }
  }

  // can move horizentally
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      if (m[i][j] == m[i][j + 1]) {
        return false;
      }
    }
  }

  // can move vertically
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      if (m[j][i] == m[j + 1][i]) {
        return false;
      }
    }
  }

  return true;
}

int getScore() {
  int score = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (m[i][j] > score)
        score = m[i][j];
    }
  }
  return score;
}

void input() {

  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  int mappedX = map(xValue, 0, 1023, -100, 100);
  int mappedY = map(yValue, 0, 1023, -100, 100);


  if (joystickMove == false) {
    if (mappedY > 10) {
      direction_2048 = UP_2048;
      joystickMove = true;
    } else if (mappedY < -10) {
      direction_2048 = DOWN_2048;
      joystickMove = true;
    } else if (mappedX > 10) {
      direction_2048 = RIGHT_2048;
      joystickMove = true;
    } else if (mappedX < -10) {
      direction_2048 = LEFT_2048;
      joystickMove = true;
    } else {
      direction_2048 = 5;
    }
  } else if (-10 <= mappedY && mappedY <= 10 && -10 <= mappedX && mappedX <= 10)
    joystickMove = false;

  if (digitalRead(buttonPin) == LOW) {
    page = PAGE_GAMES;
    isPlaying = false;
    delay(150);
  }
}

void play() {

  //action to user movement
  input();
  switch (direction_2048) {
    case RIGHT_2048:
      for (int i = 0; i < 4; ++i) {
        Shift(RIGHT_2048, i);
      }
      break;
    case LEFT_2048:
      for (int i = 0; i < 4; ++i) {
        Shift(LEFT_2048, i);
      }

      break;
    case DOWN_2048:
      for (int i = 0; i < 4; ++i) {
        Shift(DOWN_2048, i);
      }
      break;
    case UP_2048:
      for (int i = 0; i < 4; ++i) {
        Shift(UP_2048, i);
      }
      break;
    default:
      //cout << endl << "null" << endl;  // not arrow
      break;
  }

  direction_2048 = 5;
}

void displayTable() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(m[0][0]);

  display.setCursor(20, 0);
  display.print(m[0][1]);

  display.setCursor(40, 0);
  display.print(m[0][2]);

  display.setCursor(60, 0);
  display.print(m[0][3]);

  //-----------------------

  display.setCursor(0, 10);
  display.print(m[1][0]);

  display.setCursor(20, 10);
  display.print(m[1][1]);

  display.setCursor(40, 10);
  display.print(m[1][2]);

  display.setCursor(60, 10);
  display.print(m[1][3]);

  //-----------------------


  display.setCursor(0, 20);
  display.print(m[2][0]);

  display.setCursor(20, 20);
  display.print(m[2][1]);

  display.setCursor(40, 20);
  display.print(m[2][2]);

  display.setCursor(60, 20);
  display.print(m[2][3]);

  //-----------------------

  display.setCursor(0, 30);
  display.print(m[3][0]);

  display.setCursor(20, 30);
  display.print(m[3][1]);

  display.setCursor(40, 30);
  display.print(m[3][2]);

  display.setCursor(60, 30);
  display.print(m[3][3]);

  //-----------------------


  display.display();
  display.clearDisplay();
}

int ParseTheMatrix(int par, int num)  // Change line and column to a, b, c and d
{
  int i = 0;
  int j = 0;
  int k1 = 0;
  int k2 = 0;

  switch (par) {
    case 0:
      selection[0] = m[0][num];
      selection[1] = m[1][num];
      selection[2] = m[2][num];
      selection[3] = m[3][num];
      break;
    case 1:
      selection[0] = m[num][3];
      selection[1] = m[num][2];
      selection[2] = m[num][1];
      selection[3] = m[num][0];
      break;
    case 2:
      selection[0] = m[3][num];
      selection[1] = m[2][num];
      selection[2] = m[1][num];
      selection[3] = m[0][num];
      break;
    case 3:
      selection[0] = m[num][0];
      selection[1] = m[num][1];
      selection[2] = m[num][2];
      selection[3] = m[num][3];
      break;
  }

  return selection[4];
}

void PassToMatrix(int par, int num, int a, int b, int c, int d)  // This function save our a, b, c and d at list m.
{
  int i, j, k1, k2;
  k1 = 0;
  k2 = 0;

  switch (par) {
    case 0:
      m[0][num] = a;
      m[1][num] = b;
      m[2][num] = c;
      m[3][num] = d;
      break;
    case 1:
      m[num][3] = a;
      m[num][2] = b;
      m[num][1] = c;
      m[num][0] = d;
      break;
    case 2:
      m[3][num] = a;
      m[2][num] = b;
      m[1][num] = c;
      m[0][num] = d;
      break;
    case 3:
      m[num][0] = a;
      m[num][1] = b;
      m[num][2] = c;
      m[num][3] = d;
      break;
  }
}

void Sum(int par, int num, int a, int b, int c, int d)  // For sum a, b, c and d.
{
  if (a == b && a != 0) {
    a += b;
    b = c;
    c = d;
    d = 0;
    moved = true;
  }

  if (b == c && b != 0) {
    b += c;
    c = d;
    d = 0;
    moved = true;
  }

  if (c == d && c != 0) {
    c += d;
    d = 0;
    moved = true;
  }

  PassToMatrix(par, num, a, b, c, d);
}

void Shift(int par, int num)  // Shift a, b, c and d.
{
  ParseTheMatrix(par, num);
  int a, b, c, d;
  a = selection[0];
  b = selection[1];
  c = selection[2];
  d = selection[3];

  for (int i = 0; i < 3; ++i) {
    if (a == 0) {
      if (!(b == 0 && c == 0 && d == 0)) {
        a = b;
        b = c;
        c = d;
        d = 0;
        moved = true;
      }
    }

    if (b == 0) {
      if (!(c == 0 && d == 0)) {
        b = c;
        c = d;
        d = 0;
        moved = true;
      }
    }

    if (c == 0) {
      if (d != 0) {
        c = d;
        d = 0;
        moved = true;
      }
    }
  }

  Sum(par, num, a, b, c, d);
}


static void init_snake() {

  // initialize all index in snake
  for (char i = 0; i < MAXLEN; i++) {
    snakex[i] = -2;
    snakey[i] = -2;
  }

  // assign snake position based on its size
  for (char i = 0; i < snakesize; i++) {
    snakex[i] = 11 - i;
    snakey[i] = 5;
  }

  // default direction
  direction_snake = RIGHT_SNAKE;

  // show snake
  for (char i = 0; i <= 2; i++) {
    snake();
    display.display();
    delay(350);
    display.clearDisplay();
    display.display();
    delay(350);
  }
}

// show after lost
static void show_score() {
  display.setCursor(25, 10);
  display.setTextSize(1);
  int score = snakesize;
  display.print("SCORE: ");
  display.println(score);
  display.println(" ");
  display.print("press button");
  display.display();
}

static void food() {
  if (foodx < 0) {
    foodx = random(0, DISP_HEIGHT / RESOLUTION);
    foody = random(0, DISP_WIDTH / RESOLUTION);
  }
  display.drawCircle(foodx * RESOLUTION, foody * RESOLUTION, RESOLUTION / 2, BLACK);
}

static void snake() {

  // all index is drew -> break
  for (char i = 0; i < MAXLEN; i++) {
    if (snakex[i] == -2) {
      break;
    }

    // draw head
    if (i == 0) {
      display.drawCircle(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION / 2, BLACK);
    } else {

      // if head hit body
      if (snakex[0] == snakex[i] && snakey[0] == snakey[i]) {
        game_over = true;
      }

      // filled body while not pressed. Low - btn not pressed. High - pressed.
      if (digitalRead(buttonPin) == LOW) {
        page = PAGE_GAMES;
        isPlaying = false;
        snakesize = 6;
        // display.drawCircle(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION / 2, BLACK);
        //display.fillRect(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION, RESOLUTION, BLACK);
      } else {
        display.fillCircle(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION / 2, BLACK);
      }
    }
  }
}

static void movesnake() {
  char tmpx = snakex[0];
  char tmpy = snakey[0];
  input_snake();
  int xdirection, ydirection;
  switch (direction_snake) {
    case UP_SNAKE:
      xdirection = 0;
      ydirection = -1;
      break;
    case DOWN_SNAKE:
      xdirection = 0;
      ydirection = 1;
      break;
    case LEFT_SNAKE:
      xdirection = -1;
      ydirection = 0;
      break;
    case RIGHT_SNAKE:
      xdirection = 1;
      ydirection = 0;
      break;
  }

  // move snake x and y points based on x an y directions
  char prevx = tmpx + xdirection;
  char prevy = tmpy + ydirection;

  // when snake is going out of screen
  if (prevx >= DISP_HEIGHT / RESOLUTION) {
    prevx = 0;
  } else if (prevx == -1) {
    prevx = DISP_HEIGHT / RESOLUTION;
  }
  if (prevy >= DISP_WIDTH / RESOLUTION) {
    prevy = 0;
  } else if (prevy == -1) {
    prevy = DISP_WIDTH / RESOLUTION;
  }

  for (char i = 0; i < MAXLEN; i++) {
    if (i == 0) {
      if (foodx == snakex[i] && foody == snakey[i]) {
        if (snakesize < MAXLEN) {
          snakex[snakesize] = 0;
          snakey[snakesize] = 0;
          snakesize++;
        }

        foodx = -1;
      }
    }
    if (snakex[i] == -2 || (prevx == snakex[i] && prevy == snakey[i])) {
      break;
    }
    tmpx = snakex[i];
    tmpy = snakey[i];

    snakex[i] = prevx;
    snakey[i] = prevy;

    prevx = tmpx;
    prevy = tmpy;
  }
}

void input_snake() {

  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  int mappedX = map(xValue, 0, 1023, -100, 100);
  int mappedY = map(yValue, 0, 1023, -100, 100);

  if (mappedY > 10) {
    if (direction_snake != DOWN_SNAKE)
      direction_snake = UP_SNAKE;
  } else if (mappedY < -10) {
    if (direction_snake != UP_SNAKE)
      direction_snake = DOWN_SNAKE;
  } else if (mappedX > 10) {
    if (direction_snake != LEFT_SNAKE)
      direction_snake = RIGHT_SNAKE;
  } else if (mappedX < -10) {
    if (direction_snake != RIGHT_SNAKE)
      direction_snake = LEFT_SNAKE;
  }
}

void blynkWifi() {
  digitalWrite(ledPin, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(100);                  // wait for a second
  digitalWrite(ledPin, LOW);   // turn the LED off by making the voltage LOW
  delay(100);
}

void turnOnWifi() {
  digitalWrite(ledPin, HIGH);
}

void turnOffWifi() {
  digitalWrite(ledPin, LOW);
}
