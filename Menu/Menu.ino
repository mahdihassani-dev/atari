#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// page Constants
#define PAGE_MAIN 0
#define PAGE_SETTINGS 1
#define PAGE_CONTRAST 2
#define PAGE_GAMES 3

// default values
boolean backlight = false;

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

// lcd pins -> (CLK, DIN, DC, CE, RST) : you may changes them based on your need.
Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 10, 11);

void setup() {

  pinMode(2, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(7, OUTPUT);

  digitalWrite(7, HIGH);  //Turn Backlight OFF

  Serial.begin(9600);

  display.begin();
  display.setContrast(contrast);  //Set contrast to 50
  display.clearDisplay();
  display.display();
}

void loop() {

  drawMenu();
  checkDir();
  handleDir();
}

void handleDir() {

  // if up pressed
  if (up) {
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

  // if down pressed
  if (down) {
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

  // if button pushed
  if (middle) {

    middle = false;

    if (page == PAGE_MAIN) {
      if (menuitem == 1) {
        page = PAGE_GAMES;
      } else if (menuitem == 2) {
        //wifi code ------->
      } else if (menuitem == 3) {
        page = PAGE_SETTINGS;
        menuitem = 1;
      }
    }

    else if (page == PAGE_GAMES) {
      if (menuitem == 1) {
        //2048 codes ------>
      } else if (menuitem == 2) {
        //snake codes ------>
      } else if (menuitem == 3) {
        page = PAGE_MAIN;
        menuitem = 1;
      }
    }

    else if (page == PAGE_SETTINGS) {
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

    else if (page == PAGE_CONTRAST) {
      page = PAGE_SETTINGS;
    }
  }
}

void checkDir() {
  int menuDir = analogRead(yPin);
  boolean button = digitalRead(buttonPin);

  if (menuDir <= 300) {
    up = true;
    delay(150);
  }

  if (menuDir >= 700) {
    down = true;
    delay(150);
  }

  if (button == LOW) {
    middle = true;
    delay(150);
  }
}

void drawMenu() {

  // mainPageDraw
  if (page == PAGE_MAIN) {

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
    display.print(">Wifi:OFF");
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

  // settingPageDraw
  else if (page == PAGE_SETTINGS) {
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

  // contrastPageDraw
  else if (page == PAGE_CONTRAST) {

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

  // gamePageDraw
  else if (page == PAGE_GAMES) {
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