#include <IRremote.h>
#include <stdio.h>
#include <DS1302.h>
#include <Wire.h>
#include "SevSeg.h"

SevSeg sevseg;
IRsend irsend;
DS1302 rtc(5, 6, 7); // RST, DAT, CLK

int ThermistorPin = 0; //A0
int Airswitch = 3;
const int SmallAirC = 0;

int TLimit = 30;

float T1;

boolean Opentime = false;
int Now_Hour;
int Now_Min;

const int NUMBEROFVALVES = 3;
const int NUMBEROFTIMES = 2;

int onoffTimes [NUMBEROFVALVES][NUMBEROFTIMES] = {
  {9, 18}, {18, 20}, {5, 9}
};
int valvePinNumbers[NUMBEROFVALVES];

const int ONTIME = 0;
const int OFFTIME = 1;


void setup() {

  pinMode(Airswitch, OUTPUT);

  Opentime = false;

  //rtc.halt(false);
  //rtc.writeProtect(false);
  //rtc.setDOW(SATURDAY);
  //rtc.setTime(19, 55, 30);
  //rtc.setDate(25, 8, 2018);
  // rtc.writeProtect(true);

  byte numDigits = 4;
  byte digitPins[] = {10, 11, 12, 1};
  byte segmentPins[] = {9, 2, 13, A3, A2, 8, A1, 4};
  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_CATHODE;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(50);


  valvePinNumbers[0] = Airswitch;
  //  valvePinNumbers[1] = Fanswitch;


}

void loop() {

  static unsigned long timer = millis();

  if (millis() >= timer) {
    timer += 1000;
    getHourTime();
    sevseg.setNumber(Now_Hour * 100 + Now_Min , 1);
  }

  sevseg.refreshDisplay();

  if (millis() >= timer) {
    timer += 1000;
    getHourTime();
    getTemperature();

    if ((T1 > TLimit) && (Now_Hour >= onoffTimes[SmallAirC][ONTIME])
        && (Now_Hour < onoffTimes[SmallAirC][OFFTIME]) ) {
      if (Opentime == false) {
        irsend.sendNEC(0x20DF20DF, 32);
        //digitalWrite(Airswitch, HIGH);
        Opentime = true;

      }


    }
    else if (Now_Hour >= onoffTimes[SmallAirC][OFFTIME]) {
      if (Opentime == true) {
        irsend.sendNEC(0x20DF20DF, 32);
        Opentime = false;
      }

    }
    else {
      //digitalWrite(Airswitch, HIGH);

    }

    for (int valve = 1; valve < NUMBEROFVALVES; valve++) {
      if ( (Now_Hour >= onoffTimes[valve][ONTIME])
           && (Now_Hour < onoffTimes[valve][OFFTIME]) ) {
        //digitalWrite(Fanswitch, HIGH);

      }
      else {
        //digitalWrite(Fanswitch, LOW);

      }
    }

  }

  if (millis() >= timer) {
    timer += 1000;
    sevseg.setNumber(T1, 2);
  }

  sevseg.refreshDisplay();


}

void getHourTime() {

  String Now_Hour_s = rtc.getTimeStr();
  String Now_Hour_m = rtc.getTimeStr();
  String Hour_temp1;

  Hour_temp1 = Now_Hour_s.charAt(0);
  Now_Hour_s = Hour_temp1 + Now_Hour_s.charAt(1);
  Now_Hour = Now_Hour_s.toInt();
  Hour_temp1 = Now_Hour_m.charAt(3);
  Now_Hour_m = Hour_temp1 + Now_Hour_m.charAt(4);
  Now_Min = Now_Hour_m.toInt();
}

void getTemperature() {

  int Vo1;
  float R1 = 10000;
  float logR2, R2;
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

  Vo1 = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo1 - 1.0);
  logR2 = log(R2);
  T1 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  T1 = T1 - 273.15;
}


