#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <uRTCLib.h>
#include <EEPROM.h>

// initialisation des différents modules
const int bouton1 = 8, bouton2 = 9, rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo monservo;

int bestRecord[4] = {0, 0, 0, 0};

// Variables of the buttons:
int button1State = HIGH;
int lastButton1State = 1;

int button2State = HIGH;
int lastButton2State = 1;

// initialisation of the clock
uRTCLib rtc(0x68);

// initialisation
void setup()
{
  // initialisation des modules/machines
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(bouton1, INPUT);  // le bouton est une entrée
  pinMode(bouton2, OUTPUT); // le bouton est une entrée

  lcd.setCursor(2, 0);
  lcd.print("PUSH BUTTON");

  lcd.noBlink();

  monservo.attach(A0);
  monservo.write(0);

  URTCLIB_WIRE.begin();
  rtc.set(0, 0, 0, 0, 0, 0, 0);

  delay(1000);

  for (int a = 0; a < 4; a++)
  {
    int value = EEPROM.read(a);
    Serial.print(a);
    Serial.print("\t");
    Serial.print(value);
    Serial.println();
  }
}

void But1Push()
{
  lcd.clear(); // effacer le contenu de l’Afficheur LCD
  lcd.setCursor(3, 0);
  lcd.print("TIME PASS");
  lcd.setCursor(7, 1);

  rtc.refresh();

  lcd.setCursor(2, 2);
  lcd.print(rtc.day());
  lcd.print("J ");
  lcd.print(rtc.hour());
  lcd.print("H ");
  lcd.print(rtc.minute());
  lcd.print("M ");
  lcd.print(rtc.second());
  lcd.print("S ");

  if (monservo.read() != 90)
  {
    monservo.write(90);
  }
}

void But1UnPush()
{
  if (monservo.read() != 0)
  {
    monservo.write(-90);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TOTAL TIME SPENT");

  lcd.setCursor(2, 2);
  lcd.print(rtc.day());
  lcd.print("J ");
  lcd.print(rtc.hour());
  lcd.print("H ");
  lcd.print(rtc.minute());
  lcd.print("M ");
  lcd.print(rtc.second());
  lcd.print("S ");

  rtc.set(0, 0, 0, 0, 0, 0, 0);

  if (rtc.day() > EEPROM.read(0))
  {
    EEPROM.update(0, rtc.day());
    EEPROM.update(1, rtc.hour());
    EEPROM.update(2, rtc.minute());
    EEPROM.update(3, rtc.second());
  }
  else if (rtc.hour() > EEPROM.read(1))
  {
    EEPROM.update(1, rtc.hour());
    EEPROM.update(2, rtc.minute());
    EEPROM.update(3, rtc.second());
  }
  else if (rtc.minute() > EEPROM.read(2))
  {
    EEPROM.update(2, rtc.minute());
    EEPROM.update(3, rtc.second());
  }
  else if (rtc.second() > EEPROM.read(3))
  {
    EEPROM.update(3, rtc.second());
  }
}

void But2Push()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("BEST RECORD");
  lcd.setCursor(2, 2);
  lcd.print(EEPROM.read(0));
  lcd.print("J ");
  lcd.print(EEPROM.read(1));
  lcd.print("H ");
  lcd.print(EEPROM.read(2));
  lcd.print("M ");
  lcd.print(EEPROM.read(3));
  lcd.print("S ");
}

int a;

boolean game = false;

void loop()
{
  // take the state (0 or 1 of the button)
  // 1 is when the button is not pressed and 0 is when the button is pressed
  int reading1 = digitalRead(bouton1);
  int reading2 = digitalRead(bouton2);

  // launch the programme
  if (reading1 != 1)
  {
    game = true;
    // reading1 = 1;
  }
  if (game == true)
  {
    // state of the button 1
    if (button2State == HIGH)
    {
      if (reading1 != lastButton1State)
      {
        if (reading1 == HIGH)
        {
          button1State = !button1State;
          // affichage de l' EEPROM dans le terminal
          for (int a = 0; a < 4; a++)
          {
            int value = EEPROM.read(a);

            Serial.println();
            Serial.print(a);
            Serial.print("\t");
            Serial.print(value);
          }
          Serial.println();
        }
      }
      if (button1State == HIGH)
      {
        But1UnPush();
      }
      else if (button1State == LOW)
      {
        if (reading2 == HIGH)
        {
          But2Push();
        }
        else
        {
          But1Push();
        }
      };
      lastButton1State = reading1;
    }

    // state of the button 2
    if (button1State == HIGH)
    {
      if (reading2 != lastButton2State)
      {
        if (reading2 == HIGH)
        {
          button2State = !button2State;
        }
      }
      if (button2State == LOW)
      {
        But2Push();
      };
      lastButton2State = reading2;
    }
  }
  // frequency of refreching (with to low dalay, lcd bug)
  delay(100);
}