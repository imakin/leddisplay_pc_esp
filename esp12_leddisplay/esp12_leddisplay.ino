//for oled
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     5 // Reset pin GPIO5(D1)# (or -1 if sharing Arduino reset pin), somehow this only works if i set it like this
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//for dotmatrix
#include <LedControl.h>
int DIN = 16; // D0
int CS =  5;  // D1
int CLK = 4;  // D2
LedControl lc=LedControl(DIN,CLK,CS,0);

//enddotmatrix
void setup(){
  Serial.begin(115200);
  setup_oled();
  setup_dotmatrix();
}
void setup_oled() {
  Wire.begin(0,2);//SDA gpio0 D3, SCL gpio2 D4
  if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(("SSD1306 allocation failed"));
  }
  oled.clearDisplay();
  oled.setTextSize(5);      // Normal 1:1 pixel scale
  oled.setTextColor(SSD1306_WHITE); // Draw white text
  oled.setCursor(0, 0);     // Start at top-left corner
  oled.cp437(true);         // Use full 256 char 'Code Page 437' font
  oled.write("M");
  oled.display();//ready test
}
void setup_dotmatrix() {
  //for dot matrix
  lc.shutdown(0,false);     //The MAX72XX is in power-saving mode on startup
  lc.shutdown(1,false);     //setting up next 8x8 max72xx on the same i2c wire
  lc.shutdown(2,false);     //total 4 displays
  lc.shutdown(3,false);
  lc.setIntensity(0,5);     // Set the brightness
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
    //if serial string input started with whitespace, it is printed on oled,
    //or if 1st charracter is 1digit number representing fontsize 1~5 AND 2nd charracter is whitespace
    //fontsize 1 oled max char is 168 
    String value_sequence = Serial.readStringUntil('\n');
    if (value_sequence.length()<2) {
      return; //loop(), continue 
    }
    uint8_t head0 = value_sequence.charAt(0);
    uint8_t head1 = value_sequence.charAt(1);
    if ((head0==' ') ||
      (head1==' ')
    ) {
      //print to oled mode
      oled.clearDisplay();
      oled.setCursor(0, 0);
      uint8_t i=1;//if first char is " " next substring is the text
      if (head0>='1') {
        oled.setTextSize(head0-'0');
        //if first char is font size, 2ndchar " ", next substring is the text
        i = 2;
      }
      int length = value_sequence.length();
      for (;i<length;i++) { //skip the " " header [index0]
        oled.write(value_sequence.charAt(i));
      }
      oled.display();
    }
    else {
      //print to dotmatrix
      //if it's not started with whitespace, it is printed on 4x8x8 dotmatrix and must contains number sequence
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
        * explanation:
        *  192192192192192192255255 is a sequence of 3 digit numbers (zero padded for <100),
        *  when separated it is 192 192 192 192 192 192 255 255,
        *  this represents per line of dotmatrix
        *  192 is 0b11000000
        *  192192192192192192255255 is
        *  0b11000000
        *  0b11000000
        *  0b11000000
        *  0b11000000
        *  0b11000000
        *  0b11111111
        *  0b11111111
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
  }//end if (Serial.available()>0)
}
