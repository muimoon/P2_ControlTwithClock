#include <IRremote.h>
#include <stdio.h>
#include <DS1302.h>
#include <Wire.h>

IRsend irsend;
DS1302 rtc(5, 6, 7); // RST, DAT, CLK

int ThermistorPin = 0; //A0
int Airswitch = 3;
const int SmallAirC = 0;
const int Fan = 1;
int Fanswitch = 4;
int TLimit = 30;

float T1;

boolean Opentime = false;
int Now_Hour;

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

  Serial.begin(9600);

  //rtc.halt(false);
  //rtc.writeProtect(false);
  //rtc.setDOW(SUNDAY);
  //rtc.setTime(22, 05, 30);
  //rtc.setDate(12, 8, 2018);
  //rtc.writeProtect(true);

  valvePinNumbers[0] = Airswitch;
  valvePinNumbers[1] = Fanswitch;


}

void loop() {

  static unsigned long timer = millis();

  if (millis() >= timer) {
    timer += 5000;
    getHourTime();
    getTemperature();
    checkUserInteraction();
    if ((T1 > TLimit) && (Now_Hour >= onoffTimes[SmallAirC][ONTIME])
        && (Now_Hour < onoffTimes[SmallAirC][OFFTIME]) ) {
      if (Opentime == false) {
        irsend.sendNEC(0x20DF20DF, 32);
        Opentime = true;
        Serial.print(T1);
        Serial.println(Now_Hour);
        Serial.println(" AirCondition open");
      }


    }
    else if (Now_Hour > onoffTimes[SmallAirC][OFFTIME]) {
      if (Opentime == true) {
        irsend.sendNEC(0x20DF20DF, 32);
        Opentime = false;
        Serial.println(Now_Hour);
        Serial.println(" AirCondition close");
      }

    }

    for (int valve = 1; valve < NUMBEROFVALVES; valve++) {
      if ( (Now_Hour >= onoffTimes[valve][ONTIME])
           && (Now_Hour < onoffTimes[valve][OFFTIME]) ) {
        digitalWrite(Fanswitch, HIGH);
        Serial.println(T1);
        Serial.println(Now_Hour);
        Serial.println(" Fan On");
      }
      else {
        digitalWrite(Fanswitch, LOW);
        Serial.println(T1);
        Serial.println(Now_Hour);
        Serial.println(" Fan Off");
      }
    }
  }
}




void getHourTime() {
  //Serial.println(rtc.getDateStr(FORMAT_SHORT, FORMAT_LITTLEENDIAN, '/'));
  //Serial.print(rtc.getDOWStr());
  //Serial.println(rtc.getTimeStr());
  //get the hour int value;
  //Serial.println(rtc.getTimeStr());
  String Now_Hour_s = rtc.getTimeStr();
  //Serial.println(Now_Hour_s);
  String Hour_temp1;
  Hour_temp1 = Now_Hour_s.charAt(0);
  //Serial.println(Hour_temp1);
  Now_Hour_s = Hour_temp1 + Now_Hour_s.charAt(1);
  //Serial.println(Now_Hour_s);
  Now_Hour = Now_Hour_s.toInt();
  //Serial.println(Now_Hour);

}


//Setting Hour for Aircondition and Fan
//Air condition 1N09;1F18;
//Fan           2N18;1F20;
void expectValveSetting() {
  int valveNumber = Serial.parseInt();
  char onoff = Serial.read();
  int desireHour = Serial.parseInt();
  if (Serial.read() != '\n') {
    Serial.println("Make sure to end your request with a Newline");
    Serial.flush();
    return;
  }
  if (onoff == 'N') {
    onoffTimes[valveNumber][ONTIME] = desireHour;
  }
  else if (onoff == 'F') {
    onoffTimes[valveNumber][OFFTIME] = desireHour;
  }
  else {
    Serial.println("You must use upper case N or F to indicate ON time or OFF time");
    Serial.flush();
    return;
  }

  for (int valve = 0; valve < NUMBEROFVALVES; valve++) {
    Serial.println();
    Serial.print("Valve");
    Serial.print(valve);
    Serial.print("will turn ON at");
    Serial.print(onoffTimes[valve][ONTIME]);
    Serial.print(" and will turn OFF at");
    Serial.print(onoffTimes[valve][OFFTIME]);
    Serial.println();
  }
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


void checkUserInteraction() {
  while (Serial.available() > 0) {
    char temp = Serial.read();
    if (temp == 'P') {
      for (int valve = 0; valve < NUMBEROFVALVES; valve++) {
        Serial.println();
        Serial.print("Valve");
        Serial.print(valve);
        Serial.print(" scheduled ON at");
        Serial.print(onoffTimes[valve][ONTIME]);
        Serial.print(" and OFF at");
        Serial.print(onoffTimes[valve][OFFTIME]);
        Serial.println();
      }
      Serial.flush();
      break;
    }
    else if (temp == 'S') {
      expectValveSetting();
    }
    else
    {
      printMenu();
      Serial.flush();
      break;
    }
  }
}

void printMenu() {
  Serial.println("Please enter P to print the current settings");
  Serial.println("Please enter 0N9 to set Aircondition on time to 9 o'clock");
}

