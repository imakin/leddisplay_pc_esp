#include <LedControl.h>
int DIN = 16; // D0
int CS =  5;  // D1
int CLK = 4;  // D2
LedControl lc=LedControl(DIN,CLK,CS,0);
void setup(){
  Serial.begin(115200);

  lc.shutdown(0,false);     //The MAX72XX is in power-saving mode on startup
  lc.shutdown(1,false);
  lc.shutdown(2,false);
  lc.shutdown(3,false);
  lc.setIntensity(0,5);     // Set the brightness to maximum value
  lc.setIntensity(1,5);
  lc.setIntensity(2,5);
  lc.setIntensity(3,5);

  lc.clearDisplay(0);        // and clear the display
  lc.clearDisplay(1);
  lc.clearDisplay(2);
  lc.clearDisplay(3);

}
void loop() {
  if (Serial.available()>0) {
    String value_sequence = Serial.readStringUntil('\n');
    //each display line is 8bit byte written in base10 3 digit with 0 leading.
    /*read all 3 digit value from value_sequence
      display 0 line 0,
      display 0 line 1,
      ..,
      display 0 line 7,
      display 1 line 0,
      display 1 line 1,
      ..,
      display 1 line 7,
      display 2 line 0,
      ...
      * and so on until \n is detected from inputstring
      
      for example input 
      * 192192192192192192255255
      * will print    L on display_n 0 only
      * 192192192192192192255255192192192192192192255255192192192192192192255255192192192192192192255255
      * will print LLLL on 4 display_n
    */
     
    int len = value_sequence.length();
    int i=0;
    int display_n = 0;
    int display_line = 0;
    while (i<len) {
      int v = value_sequence.substring(i,i+3).toInt();
      lc.setRow(display_n, display_line, (byte)v);
      display_line+=1;
      if (display_line>7) {
        display_n += 1;
        display_line = 0;
      }
      i+=3;
    }
  }
}
