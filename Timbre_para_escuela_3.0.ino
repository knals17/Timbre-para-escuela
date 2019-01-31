#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <Wtv020sd16p.h>
#include <EEPROM.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
RTC_DS1307 RTC;

byte retardation = 5;
//-------------------------- Pines para Reproductor -----------------------------------------------
int resetPin = 17;        // Reset pin.
int clockPin = 16;        // Clock pin.
int dataPin = 15;         // Data pin.
int busyPin = 14;         // Busy pin.
byte sound[70];
byte rele = 2;
Wtv020sd16p wtv020sd16p(resetPin, clockPin, dataPin, busyPin);
//---------------------------------------- Botones -------------------------------------------------
int up = 5, down = 6, back = 4, next = 7, alarm = 8;
//---------------------------------------- Variables -----------------------------------------------
byte Hour[70];
byte Minutes[70];
byte ring;
byte menu = 0;
byte screen;
byte data;
byte DAY;

//-----------------------------------------SetTime----------------------------------------------------
int setYear;
int setMonth;
int setDay;
int setHour;
int setMinutes;
int setSeconds;
//----------------------------------------------------------------------------------------------------

void setup () {
  digitalWrite(rele , HIGH);
  Serial.begin(9600);
  lcd.begin(20, 4);
  Wire.begin();
  RTC.begin();
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(up, INPUT);
  pinMode(down, INPUT);
  pinMode(next, INPUT);
  pinMode(back, INPUT);
  pinMode(alarm, INPUT);
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    //following line sets the RTC to the date & time this sketch was compiled

    RTC.adjust(DateTime(2014, 12, 17, 0, 47, 00));


  }
  //EEPROM.put(800, 0);

  EEPROM.get(800, data);

  if (data != 1 or digitalRead(back) == 1) {
    lcd.clear();
    lcd.home ();
    lcd.setCursor ( 0, 0 );
    lcd.print("Deleting data...");
    EEPROM.put(800, 1);
    for ( byte i = 0 ; i < 69 ; i++) {
      EEPROM.put(i, 0);
      EEPROM.put(i + 70, 0);
      EEPROM.put(i + 140, 0);
      Serial.print("Format... Slot ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(Hour[i]);
      Serial.print(":");
      Serial.print(Minutes[i]);
      Serial.print(" Sound: ");
      Serial.println(sound[i]);
      delay(50);
    }
    lcd.clear();
  }
  Read();
}

void Read () {
  lcd.clear();
  lcd.home ();
  lcd.setCursor ( 0, 0 );
  lcd.print("Reading data...");
  for ( byte i = 0 ; i < 70 ; i++) {
    EEPROM.get(i, Hour[i]);
    EEPROM.get(i + 70, Minutes[i]);
    EEPROM.get(i + 140, sound[i]);
    Serial.print(Hour[i]);
    Serial.print(":");
    Serial.print(Minutes[i]);
    Serial.print(" Sound: ");
    Serial.println(sound[i]);
    delay(50);
  }
  lcd.setCursor ( 6, 1 );
  lcd.print("OK");
  delay (2000);
  lcd.clear();
  wtv020sd16p.reset();

  if (digitalRead(4) == HIGH && digitalRead(7) == HIGH) {
    timerSet();
  }
}



void loop () {

  DateTime now = RTC.now();

  lcd.home ();
  lcd.setCursor ( 4, 0 );
  if (now.hour() < 10) {
    lcd.print("0");
    lcd.print(now.hour()); // dayOfTheWeek()
  } else {
    lcd.print(now.hour()); // dayOfTheWeek()
  }
  lcd.print(":");
  if (now.minute() < 10) {
    lcd.print("0");
    lcd.print(now.minute());
  } else {
    lcd.print(now.minute());
  };
  lcd.print(":");
  if (now.second() < 10) {
    lcd.print("0");
    lcd.print(now.second());
  } else {
    lcd.print(now.second());
  }
  lcd.setCursor ( 1, 1 );
  lcd.print(DayW(now.dayOfTheWeek())); // dayOfTheWeek()
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());

  /*if (now.second() == 0){
    wtv020sd16p.asyncPlayVoice(0);
    }*/

  if (digitalRead(next) == 1) {
    menu = 0;
    stup();
  }

  if (digitalRead(alarm) == 1) {
    wtv020sd16p.asyncPlayVoice(31);
    while (digitalRead(alarm) == 1) {
      digitalWrite(rele , LOW);
    }
    digitalWrite(rele , HIGH);
  }

  for ( byte i = now.dayOfTheWeek() * 10 ; i < 9 + (now.dayOfTheWeek() * 10) ; i++) {
    /*Serial.print(Hour[i]);
      Serial.print(":");
      Serial.print(Minutes[i]);
      Serial.print(" = ");
      Serial.print(now.hour());
      Serial.print(":");
      Serial.println(now.minute());
      delay(500);*/
    if (Hour[i] == now.hour() && Minutes[i] == now.minute() && now.second() == 0) {
      digitalWrite(rele , LOW);
      wtv020sd16p.asyncPlayVoice(sound[i]);
      Serial.print(Hour[i]);
      Serial.print(":");
      Serial.print(Minutes[i]);
      Serial.print("Sound ");
      Serial.println(sound[i]);
    }
  }
  if (digitalRead(rele) == LOW and now.second() == retardation) {
    digitalWrite(rele , HIGH);
  }
}



void stup() {
  lcd.clear();
  lcd.home();
  lcd.print("   Setup Mode   ");
  delay(2000);
  while (menu == 0) {
    switch (screen) {
      case 0:
        lcd.clear();
        lcd.home();
        lcd.print("   Day to set");
        lcd.setCursor ( 6, 1 );
        lcd.print(DayW(DAY));
        if (digitalRead(up) == HIGH) {
          DAY++;
          if (DAY > 6) {
            DAY = 6;
          }
        }
        if (digitalRead(down) == HIGH) {
          DAY--;
          if (DAY == 255) {
            DAY = 0;
          }
        }
        if (digitalRead(next) == HIGH) {
          screen = 1;
        }

        if (digitalRead(back) == HIGH) {
          menu = 1;
        }
        break;

      case 1:
        lcd.clear();
        lcd.home();
        lcd.print("  Ring to set");
        lcd.setCursor ( 7, 1 );
        lcd.print(ring);
        if (digitalRead(up) == HIGH) {
          ring++;
          if (ring > 9) {
            ring = 9;
          }
        }
        if (digitalRead(down) == HIGH) {
          ring--;
          if (ring == 255) {
            ring = 0;
          }
        }
        if (digitalRead(next) == HIGH) {
          screen = 2;
        }

        if (digitalRead(back) == HIGH) {
          screen = 0;
        }
        break;

      case 2:
        lcd.clear();
        lcd.home();
        lcd.print(" Add or modify ");
        lcd.setCursor ( 2, 1 );
        lcd.print("Hour ");
        if (Hour[ring] < 10) {
          lcd.print("0");
          lcd.print(Hour[ring]);
        } else {
          lcd.print(Hour[ring]);
        }
        lcd.print(":");
        if (Minutes[ring] < 10) {
          lcd.print("0");
          lcd.print(Minutes[ring]);
        } else {
          lcd.print(Minutes[ring]);
        };

        if (digitalRead(up) == HIGH) {
          Hour[ring]++;
          if (Hour[ring] > 23) {
            Hour[ring] = 0;
          }
        }
        if (digitalRead(down) == HIGH) {
          Hour[ring]--;
          if (Hour[ring] == 255) {
            Hour[ring] = 23;
          }
        }
        if (digitalRead(next) == HIGH) {
          screen = 3;
        }

        if (digitalRead(back) == HIGH) {
          screen = 1;
        }
        break;

      case 3:
        lcd.clear();
        lcd.home();
        lcd.print(" Add or modify ");
        lcd.setCursor ( 2, 1 );
        lcd.print("Minu ");
        if (Hour[ring] < 10) {
          lcd.print("0");
          lcd.print(Hour[ring]);
        } else {
          lcd.print(Hour[ring]);
        }
        lcd.print(":");
        if (Minutes[ring] < 10) {
          lcd.print("0");
          lcd.print(Minutes[ring]);
        } else {
          lcd.print(Minutes[ring]);
        };

        if (digitalRead(up) == HIGH) {
          Minutes[ring]++;
          if (Minutes[ring] > 59) {
            Minutes[ring] = 0;
          }
        }
        if (digitalRead(down) == HIGH) {
          Minutes[ring]--;
          if (Minutes[ring] == 255) {
            Minutes[ring] = 59;
          }
        }
        if (digitalRead(next) == HIGH) {
          screen = 4;
        }

        if (digitalRead(back) == HIGH) {
          screen = 2;
        }
        break;

      case 4:
        lcd.clear();
        lcd.home();
        lcd.print("  Sound track  ");
        lcd.setCursor ( 2, 1 );
        lcd.print("Number: ");
        lcd.print(sound[ring]);
        if (digitalRead(up) == HIGH) {
          sound[ring]++;
          if (sound[ring] > 30) {
            sound[ring] = 0;
          }
        }
        if (digitalRead(down) == HIGH) {
          sound[ring]--;
          if (sound[ring] == 255) {
            sound[ring] = 30;
          }
        }
        if (digitalRead(next) == HIGH) {
          screen = 5;
        }

        if (digitalRead(back) == HIGH) {
          screen = 3;
        }
        break;

      case 5:
        lcd.clear();
        lcd.home();
        lcd.print("Saved");
        EEPROM.put(ring + (DAY * 10), Hour[ring]);
        EEPROM.put(ring + (DAY * 10) + 70, Minutes[ring]);
        EEPROM.put(ring + (DAY * 10) + 140, sound[ring]);
        delay(2000);
        screen = 0;
        break;
    }
    delay(200);
  }
  Read();
  lcd.clear();
}

String DayW(int Nday) {
  if (Nday == 0) {
    return ("Sun. ");
  }
  if (Nday == 1) {
    return ("Mon. ");
  }
  if (Nday == 2) {
    return ("Tue. ");
  }
  if (Nday == 3) {
    return ("Wed. ");
  }
  if (Nday == 4) {
    return ("Thu. ");
  }
  if (Nday == 5) {
    return ("Fri. ");
  }
  if (Nday == 6) {
    return ("Sat. ");
  }
  return ("Nah");
}

void timerSet() {
  DateTime now = RTC.now();
  lcd.clear();
  lcd.home();
  lcd.print(" Time and date  ");
  lcd.setCursor ( 0, 1 );
  lcd.print("    settings    ");
  delay(2000);
  setYear = now.year();
  setMonth = now.month();
  setDay = now.day();
  setHour = now.hour();
  setMinutes = now.minute();
  setSeconds = now.second();


  lcd.clear();
  lcd.home ();
  lcd.setCursor ( 4, 0 );
  if (setHour < 10) {
    lcd.print("0");
    lcd.print(setHour); // dayOfTheWeek()
  } else {
    lcd.print(setHour); // dayOfTheWeek()
  }
  lcd.print(":");
  if (setMinutes < 10) {
    lcd.print("0");
    lcd.print(setMinutes);
  } else {
    lcd.print(setMinutes);
  };
  lcd.print(":");
  if (setSeconds < 10) {
    lcd.print("0");
    lcd.print(setSeconds);
  } else {
    lcd.print(setSeconds);
  }

  byte page = 0;
  
  switch (page) {
  case 0:
    // statements
    break;
  case 1:
    // statements
    break;
}
  /*lcd.setCursor ( 1, 1 );
  lcd.print(setDay);
  lcd.print("/");
  lcd.print(setMonth);
  lcd.print("/");
  lcd.print(setYear);*/
  
}

