# P1_TControlAirCondition
***
# P2_ControlTwithClock
***
# P3_ProMiniTCotrolwithLEDshow
Time (DS1302) and Temperature(Thermistor_100K) control air condition together. 

And both of them will be shown on the LEDS. 

Consider there are not enough digital pins for 4 bits LED, only 3 bits are working.

* ***Pro mini*** instead of UNO

* `DS1302 rtc(5, 6, 7); // RST, DAT, CLK`

* digitPins[] = {10, 11, 12, ~~13~~}

* segmentPins[] = {9, 2, 13, ~~5, 6~~~，8, ~~7~~, 4}
  

  *instead by*

  segmentPins[] = {9, 2, 13, **A3, A2**，8, **A1**, 4}
  
* 3 is kept for IR.

  `irsend.sendNEC(0x20DF20DF, 32)`
