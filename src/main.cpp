#include <Arduino.h>

#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Wire.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define TFT_CS        4 // Chip select
#define TFT_RST       6 // Display reset
#define TFT_DC        5 // Display data/command select
#define TFT_BACKLIGHT  9 // Display backlight pin
#define TFT_LED 0 // 0 if wired to +5V directly

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)


volatile uint8_t frame = 0;
volatile uint8_t secondary_frame = 0;
volatile uint8_t secondary_mod = 1;
volatile uint8_t cnt = 0;
volatile uint16_t inNum = 0;
const size_t    MaxString               = 16;
const long int colors[5] = {
  0x07E0,
  0x7FE0,
  0xF7E0,
  0xFFA0,
  0xF800,

};
char console[8];
char topTemp[12];
char botTemp[12];
char initialData;
volatile uint16_t temps[10] = {1, 1, 1, 1, 100};
String inputString = "";         // a String to hold incoming data
bool stringComplete = true;  // whether the string is complete

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
void displayInit();
void initGraphics();

void setup(void)
{
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  analogWrite(TFT_BACKLIGHT, 255);
  tft.setRotation(1);
  inputString.reserve(200);
  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&FreeMonoBold12pt7b);
  tft.setTextWrap(false);
  displayInit();
  initGraphics();
  Serial.begin(9600);
}


void initGraphics(){
  //tft.drawBitmap(10, 30, cat_body, 50, 60, 0xFFFFFF);
  //tft.drawBitmap(43, 59, cat_tail[0], 25, 27, 0xFFFFFF);
}

void progressAnimation(){
 if(secondary_frame == 9){
   secondary_mod = -1;
 }
 if(secondary_frame == 0){
   secondary_mod = 1;
 }
 //tft.drawBitmap(43, 59, cat_tail[secondary_frame], 25, 27, 0x0);
 secondary_frame += secondary_mod;
 //tft.drawBitmap(43, 59, cat_tail[secondary_frame], 25, 27, 0xFFFFFF);
}

void displayInit(){
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(74, 20);
  tft.print("GPU");
  tft.setCursor(74, 70);
  tft.print("CPU");
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(110,  110);
  tft.print("RAM");
  tft.drawLine(107, 124, 107, 102, 0xFFFFFF);
  tft.drawLine(2, 124, 2, 102, 0xFFFFFF);
}

void refreshDisp(){
  char data[8];
  tft.setFont(&FreeMonoBold12pt7b);
  tft.setCursor(112, 20);
  tft.fillRect(112,  6,  50, 16, 0x000000);
  sprintf(data, "%02dC\n", temps[0]);
  sprintf(data, "%02dC", temps[0]);
  tft.print(data);
  tft.fillRect(98,  26,  50, 16, 0x000000);
  tft.setCursor(98, 40);
  sprintf(data, "%03d%%", temps[1]);
  tft.print(data);

  tft.fillRect(112,  56,  50, 16, 0x000000);
  tft.setCursor(112, 70);
  sprintf(data, "%02dC", temps[2]);
  tft.print(data);
  tft.fillRect(98,  76,  50, 16, 0x000000);
  tft.setCursor(98, 90);
  sprintf(data, "%03d%%", temps[3]);
  tft.print(data);
  tft.setFont(&FreeMonoBold9pt7b);
  tft.fillRect(110,  113,  50, 13, 0x000000);
  tft.setCursor(110, 124);
  sprintf(data, "%03d%%", temps[4]);
  tft.print(data);
      tft.fillRect(
      5, 126, 100, -28, 0x000000
    );
  for(uint_fast8_t x = 0; x < (int)(temps[4]/20); x++){
    tft.fillRect(
      5+20*x, 126, 18, -28, colors[x]
    );
  }
  if(temps[4]%20){
    //
    tft.fillRect(
      5+20*(int)(temps[4]/20), 126, 18, -(int)((temps[4]%20)/4)*7, colors[(int)(temps[4]/20)]
      );
  }
}

void handleData(){
  while(Serial.available()){
    char inChar = (char)Serial.read();
      if(inChar != ';'){
        inNum = (10 * inNum) + (inChar - '0');
      }
      else {
        temps[cnt] = inNum;
        inNum = 0;
        cnt += 1;
      }

    if (cnt == 5) {
      cnt = 0;
      Serial.println("OK");
      stringComplete = true;
    }
  }
}

void loop(void)
{
  
    delay(80);
    progressAnimation();
    frame = (frame+1) % 24;
    if(stringComplete){
      refreshDisp();
      stringComplete = false;
    }
    if (Serial.available()) {
      initialData = (char)Serial.read();
      if(initialData== ';'){
        handleData();
      }
      else if(initialData == '?'){
        Serial.println("UNDER_GPU");
      }
      else{
        Serial.println("BAD_DATA");
      }
        while(Serial.available())
          Serial.read();
  }
}

