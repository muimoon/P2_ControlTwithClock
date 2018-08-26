#include <IRremote.h> 

IRsend irsend;   
    
int ThermistorPin = 0;
int Airswitch=3;
int TLimit=30;
int Vo1;
boolean Opentime=false;
float R1 = 10000;
float logR2, R2, T1;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
  pinMode(Airswitch, OUTPUT);   
  //Serial.begin(9600);

}

void loop() {
  Vo1 = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo1 - 1.0);
  logR2 = log(R2);
  T1 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  T1 = T1 - 273.15;
//  T = (T * 9.0) / 5.0 + 32.0; //Comment out for Celsius

  static unsigned long timer = millis();

  if (millis() >= timer) {
    timer += 5000;
      if((T1>TLimit) && (Opentime==false)) {
        irsend.sendNEC(0x20DF20DF, 32);
        Opentime=true;        
        //Serial.print(T1);
        //Serial.print(TLimit);
        //Serial.println("AirCondition open");

      //} else {        
        //Serial.print(T1);
        //Serial.print(TLimit);
        //Serial.println("AirCondition close");

      }
     
  }


}


