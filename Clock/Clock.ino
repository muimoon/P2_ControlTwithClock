#include "Wire.h"
#include "SevenSeg.h"
#include <DS1302.h>
#include <LowPower.h>

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
  //rtc.setTime(9, 5, 30);
  //rtc.writeProtect(true);.
  ADCSRA = 0;
  disp.setDigitPins(numOfDigits, digitPins);
  disp.setCommonCathode();
  disp.setColonPin(colonPin);
  disp.setDutyCycle(dc);
  disp.setDigitDelay (1667);

}

void loop()
{
  LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);
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
