#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define PAGE_MAIN 0
#define PAGE_SETTINGS 1
#define PAGE_CONTRAST 2
#define PAGE_GAMES 3

boolean backlight = true;
int contrast = 50;

int xPin = A0;
int yPin = A1;
int buttonPin = 12;

int menuitem = 1;
int page = PAGE_MAIN;

volatile boolean up = false;
volatile boolean down = false;
volatile boolean middle = false;

int downButtonState = 0;
int upButtonState = 0;
int selectButtonState = 0;
int lastDownButtonState = 0;
int lastSelectButtonState = 0;
int lastUpButtonState = 0;

Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 10, 11);

void setup() {

  pinMode(2, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(7, OUTPUT);

  digitalWrite(7, HIGH);  //Turn Backlight ON

  Serial.begin(9600);

  display.begin();
  display.setContrast(contrast);  //Set contrast to 50
  display.clearDisplay();
  display.display();
}

void loop() {

  drawMenu();

  downButtonState = digitalRead(2);
  selectButtonState = digitalRead(1);
  upButtonState = digitalRead(0);

  int yVal = analogRead(yPin);
  boolean button = digitalRead(buttonPin);

  if (yVal <= 450) {
    up = true;
    delay(150);
  }

  if (yVal >= 650) {
    down = true;
    delay(150);
  }

  if (button == LOW) {
    middle = true;
    delay(150);
  }

  checkIfDownButtonIsPressed();
  checkIfUpButtonIsPressed();
  checkIfSelectButtonIsPressed();

  if (up && page == PAGE_MAIN) {
    up = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 3;
    }
  } else if (up && page == PAGE_SETTINGS) {
    up = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 4;
    }
  } else if (up && page == PAGE_CONTRAST) {
    up = false;
    contrast++;
    setContrast();
  } else if (up && page == PAGE_GAMES) {
    up = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 3;
    }
  }


  if (down && page == PAGE_MAIN) {
    down = false;
    menuitem++;
    if (menuitem == 4) {
      menuitem = 1;
    }
  } else if (down && page == PAGE_SETTINGS) {
    down = false;
    menuitem++;
    if (menuitem == 5) {
      menuitem = 1;
    }
  } else if (down && page == PAGE_CONTRAST) {
    down = false;
    contrast--;
    setContrast();
  } else if (down && page == PAGE_GAMES) {
    down = false;
    menuitem++;
    if (menuitem == 4) {
      menuitem = 1;
    }
  }


  if (middle) {
    middle = false;

    // handle MAIN PAGE
    if (page == PAGE_MAIN && menuitem == 1) {
      page = PAGE_GAMES;
    } else if (page == PAGE_MAIN && menuitem == 2) {
      //wifi connection
    } else if (page == PAGE_MAIN && menuitem == 3) {
      page = PAGE_SETTINGS;
      menuitem = 1;
    }

    //handle GAME PAGE
    else if (page == PAGE_GAMES && menuitem == 1) {
      //2048 codes
    } else if (page == PAGE_GAMES && menuitem == 2) {
      //snake codes
    } else if (page == PAGE_GAMES && menuitem == 3) {
      page = PAGE_MAIN;
      menuitem = 1;
    }

    else if (page == PAGE_SETTINGS && menuitem == 2) {
      if (backlight) {
        backlight = false;
        turnBacklightOff();
      } else {
        backlight = true;
        turnBacklightOn();
      }
    }

    else if (page == PAGE_SETTINGS && menuitem == 3) {
      resetDefaults();
    }

    else if (page == PAGE_SETTINGS && menuitem == 1) {
      page = PAGE_CONTRAST;
    } else if (page == PAGE_CONTRAST) {
      page = PAGE_SETTINGS;
    } else if (page == PAGE_SETTINGS && menuitem == 4) {
      page = PAGE_MAIN;
      menuitem = 1;
    }
  }
}

void checkIfDownButtonIsPressed() {
  if (downButtonState != lastDownButtonState) {
    if (downButtonState == 0) {
      down = true;
    }
    delay(50);
  }
  lastDownButtonState = downButtonState;
}

void checkIfUpButtonIsPressed() {
  if (upButtonState != lastUpButtonState) {
    if (upButtonState == 0) {
      up = true;
    }
    delay(50);
  }
  lastUpButtonState = upButtonState;
}

void checkIfSelectButtonIsPressed() {
  if (selectButtonState != lastSelectButtonState) {
    if (selectButtonState == 0) {
      middle = true;
    }
    delay(50);
  }
  lastSelectButtonState = selectButtonState;
}


void drawMenu() {

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


  } else if (page == PAGE_CONTRAST) {

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
  } else if (page == PAGE_GAMES) {
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

void turnBacklightOn() {
  digitalWrite(7, LOW);
}

void turnBacklightOff() {
  digitalWrite(7, HIGH);
}