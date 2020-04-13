#include<dht.h>
#include <RTClib.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(5, 6);
DFRobotDFPlayerMini myDFPlayer;
int maxSongs = 0;

//MODES
int mode = 0;
int maxMode = 5;
int modePin = 2;
int buttonState = 0;
int buttonState2 = 0;

//ALARM
bool isAlarm = true;
int alarmPin = 3;

MD_Parola P = MD_Parola(MD_MAX72XX::PAROLA_HW, 10, 4);
RTC_DS1307 rtc;
dht DHT;
//DaysOfTheWeek
char daysOfTheWeek[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

void setup() {
  pinMode(modePin, INPUT);
  pinMode(alarmPin, INPUT);
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial))
  {
    while (true);
  }
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30);
  myDFPlayer.EQ(5);
  maxSongs = myDFPlayer.readFileCounts(DFPLAYER_DEVICE_SD);
  Serial.println(maxSongs);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (true);
  }
  else {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //RTC.adjust(DateTime(y, mon, d, h, minu, s));
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  P.begin();
}

void playTrack(uint8_t track) {
  myDFPlayer.stop();
  delay(10);
  myDFPlayer.play(track);
  delay(10);
  int file = myDFPlayer.readCurrentFileNumber();

  Serial.print("Track:"); Serial.println(track);
  Serial.print("File:"); Serial.println(file);

  while (file != track) {
    myDFPlayer.play(track);
    delay(10);
    file = myDFPlayer.readCurrentFileNumber();
  }
}

void printModes() {
  DateTime now = rtc.now();
  if (mode == 0) {
    char sentence[8];
    const uint8_t h = now.hour();
    const uint8_t hr_12 = h % 12;
    //AM/PM Time format
    if (h < 12) {
      sprintf(sentence, "%02d:%02dA", hr_12, now.minute());
      P.setCharSpacing(1);
      P.displayText(sentence, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      P.displayAnimate();
    }
    else {
      sprintf(sentence, "%02d:%02dP", hr_12, now.minute());
      P.setCharSpacing(1);
      P.displayText(sentence, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      P.displayAnimate();
    }
    //ALARM
    if (h == 7 && now.minute() == 0) {
      playTrack(1);
      delay(15000);
      playTrack(2);
      delay(15000);
    }
    else if (h == 14 && now.minute() == 0) {
      playTrack(1);
      delay(15000);
      playTrack(3);
      delay(15000);
    }
    else if (h == 15 && now.minute() == 0) {
      playTrack(1);
      delay(15000);
      playTrack(4);
      delay(15000);
    }
    else if (h == 17 && now.minute() == 0) {
      playTrack(1);
      delay(15000);
      playTrack(5);
      delay(15000);
    }
    else if (h == 20 && now.minute() == 0) {
      playTrack(1);
      delay(15000);
      playTrack(6);
      delay(15000);
    }
    else if (h == 21 && now.minute() == 0) {
      playTrack(1);
      delay(15000);
      playTrack(7);
      delay(15000);
    }
  }
  //Days of the Week
  else if (mode == 1) {
    P.setCharSpacing(1);
    P.displayText(daysOfTheWeek[now.dayOfTheWeek()], PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
  }
  //month.date.year (mm.dd.yy)
  else if (mode == 2) {
    char sentence[8];
    sprintf(sentence, "%02d.%02d.%d", now.month(), now.day(), now.year() % 100);
    P.setCharSpacing(0);
    P.displayText(sentence, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
  }
  //Temperature
  else if (mode == 3) {
    char sentence[8];
    char str_temp[4];
    DHT.read11(7);
    dtostrf(DHT.temperature, 2, 0, str_temp);
    sprintf(sentence, "T:%sC", str_temp);
    P.setCharSpacing(1);
    P.displayText(sentence, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
    delay(2000);
  }
  //Humidity
  else if (mode == 4) {
    char sentence[8];
    char str_temp[4];
    DHT.read11(7);
    dtostrf(DHT.humidity, 2, 0, str_temp);
    sprintf(sentence, "H:%s%%", str_temp);
    P.setCharSpacing(1);
    P.displayText(sentence, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
    delay(2000);
  }
}

void changeMode() {
  mode += 1;
  if (mode == maxMode) {
    mode = 0;
  }
}

void loop() {
  printModes();
  if (digitalRead(modePin) == HIGH && buttonState == LOW) {
    changeMode();
    buttonState = HIGH;
  }
  else if (digitalRead(modePin) == LOW && buttonState == HIGH) {
    buttonState = LOW;
  }
  if (digitalRead(alarmPin) == HIGH && buttonState2 == LOW) {
    if (isAlarm) {
      isAlarm = false;
      P.setCharSpacing(1);
      P.displayText("A-OFF", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      P.displayAnimate();
      delay(2000);
    }
    else {
      isAlarm = true;
      P.setCharSpacing(1);
      P.displayText("A-ON", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      P.displayAnimate();
      delay(2000);
    }
    buttonState2 = HIGH;
  }
  else if (digitalRead(alarmPin) == LOW && buttonState2 == HIGH) {
    buttonState2 = LOW;
  }
}
