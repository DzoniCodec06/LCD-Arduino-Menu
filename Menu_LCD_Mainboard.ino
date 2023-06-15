#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int CLK = 10;
const int DT = 9;
const int SW = 3;

const int KEY_PIN = 2;
int key;
//const int RESSET_BTN = 5;

int resset;

int screen_delay = 1000;

bool selected_channel = false;

int lastCLKState;
int currentCLKState;
int DTState;
int SWState;

int screen = 1;
const int max_screens = 4;

bool selectedScreen = false;
int pressed = 0;

int counter = 0;

int value;

const int SHORT_PRESS_TIME = 500; // 500 milliseconds
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

byte arrow[8] = {
  0b01000,
  0b01100,
  0b01110,
  0b01111,
  0b01110,
  0b01100,
  0b01000,
  0b00000
};

int key_insert = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Fireworks");
  lcd.setCursor(4, 1);
  lcd.print("Igniter");

  lcd.createChar(0, arrow);

  delay(2000);
  lcd.clear();
  checkScreen(screen);

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(KEY_PIN, INPUT_PULLUP);

  lastCLKState = digitalRead(CLK);
}

void checkScreen(int sc) {
  switch (sc) {
    case 1:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 1");
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      lcd.setCursor(2, 1);
      lcd.print("Node 2");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 1");
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
      lcd.setCursor(2, 1);
      lcd.print("Node 2");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 3");
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      lcd.setCursor(2, 1);
      lcd.print("Node 4");
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Node 3");
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
      lcd.setCursor(2, 1);
      lcd.print("Node 4");
      break;
    default: screen = 4;
  }
}

void selectScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Channel:");
  lcd.setCursor(0, 1);
  lcd.print("CH1");
  lcd.setCursor(4, 1);
  lcd.print("CH2");
  lcd.setCursor(8, 1);
  lcd.print("CH3");
  lcd.setCursor(12, 1);
  lcd.print("CH4");
}

void controll_servo(int sc) {
  switch (sc) {
    case 1:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Servo 1");
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      lcd.setCursor(2, 1);
      lcd.print("Servo 2");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Servo 1");
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
      lcd.setCursor(2, 1);
      lcd.print("Servo 2");
      break;
    default:
      sc = 2;
      break;
  }
}

void selectChannel(int value) {
  if (value < 100) {          //Ch1
    Serial.println("CH1");
    controll_servo(screen);
    pressed = 0;
  } else if (value == 513) {    //Ch2
    Serial.println("CH2");
    controll_servo(screen);
    pressed = 0;
  } else if (value == 509 || value == 510) {    //Ch3
    Serial.println("CH3");
    controll_servo(screen);
    pressed = 0;
  } else if (value == 512) {    //Ch4
    Serial.println("CH4");
    controll_servo(screen);
    pressed = 0;
  }
}




void loop() {
  key = digitalRead(KEY_PIN);
  if (key == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Key");
    lcd.setCursor(1, 1);
    lcd.print("Required!");
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    lcd.clear();
    lcd.setCursor(8, 0);
    lcd.print("Key");
    lcd.setCursor(5, 1);
    lcd.print("Required!");
    delay(500);
    key_insert = 0;
  }
  else if (key == LOW) { 
    if (key_insert == 0) {
      key_insert = 1;
      checkScreen(screen);
    }
    digitalWrite(LED_BUILTIN, LOW);
    currentCLKState = digitalRead(CLK);

    if (currentCLKState != lastCLKState && currentCLKState == 1) {
      DTState = digitalRead(DT);
      if (DTState != currentCLKState) { // CW
        if (!selectedScreen) {
          if (screen < max_screens) screen++;
          checkScreen(screen);
          if (selected_channel) controll_servo(screen);
        } else return;
      } else {                          // CCW
        if (!selectedScreen) {
          if (screen > 1) screen--;
          checkScreen(screen);
        }
      }
    }

    SWState = digitalRead(SW);

    if (lastState == HIGH && SWState == LOW)       // button is pressed

      pressedTime = millis();
    else if (lastState == LOW && SWState == HIGH) { // button is released
      releasedTime = millis();

      long pressDuration = releasedTime - pressedTime;

      if ( pressDuration < SHORT_PRESS_TIME ) {
        pressed = 1;
        selectScreen();
        selectedScreen = true;
        while (pressed == 1) {
          value = analogRead(A0);
          digitalWrite(LED_BUILTIN, HIGH);
          selectChannel(value);
          delay(250);
          digitalWrite(LED_BUILTIN, LOW);
          delay(250);
        }
      } else {
        lcd.clear();
        pressed = 0;
        screen = 1;
        checkScreen(screen);
        selectedScreen = false;
      }
    }

    // save the the last state
    lastState = SWState;
    lastCLKState = currentCLKState;
  }
}
