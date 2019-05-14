//Programa : Relogio com Arduino, DS1307 e Display 7 Seg
//Autor : FILIPEFLOP

#include "Wire.h"
#include "SevenSeg.h"
#include <DS1302.h>

//int ThermistorPin = 0; // A0
//float T1; // Testing Temperature

DS1302 rtc(7, 6, 5); // RST, DAT, CLK
int Now_Hour;
int Now_Min;

const int numOfDigits = 4;
SevenSeg disp(9, A2, 13, 4, 2, 8, A1);
int digitPins[numOfDigits] = {10, 11, 12, 1};
int colonPin = 0;
int dc = 75;  // range 0 to 100
char c = ':' ;


void setup()
{
  //rtc.halt(false);
  //rtc.writeProtect(false);
  //rtc.setTime(13, 46, 30);
  //rtc.writeProtect(true);

  disp.setDigitPins(numOfDigits, digitPins);
  disp.setCommonCathode();
  disp.setColonPin(colonPin);
  disp.setDutyCycle(dc);

}

void loop()
{
  getHourTime();
  disp.writeClock(Now_Hour, Now_Min, c);
}


void getHourTime()
{
  String Now_Hour_s = rtc.getTimeStr();
  String Now_Hour_m = rtc.getTimeStr(); // 00：00：00 time fomat
  String Hour_temp1;
  Hour_temp1 = Now_Hour_s.charAt(0);
  Now_Hour_s = Hour_temp1 + Now_Hour_s.charAt(1);
  Now_Hour = Now_Hour_s.toInt();
  Hour_temp1 = Now_Hour_m.charAt(3);
  Now_Hour_m = Hour_temp1 + Now_Hour_m.charAt(4);
  Now_Min = Now_Hour_m.toInt();
}

void getTemperature()
{
  int Vo1;
  float R1 = 10000;
  float logR2, R2;
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  Vo1 = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float) Vo1 - 1.0);
  logR2 = log(R2);
  T1 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  T1 = T1 - 273.15;
}
