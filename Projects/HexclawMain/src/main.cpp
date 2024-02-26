/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };


void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawtriangle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}



void setup() {
    Wire.begin(14, 12);
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  testdrawline();      // Draw many lines

  testdrawrect();      // Draw rectangles (outlines)

  testfillrect();      // Draw rectangles (filled)

  testdrawcircle();    // Draw circles (outlines)

  testfillcircle();    // Draw circles (filled)

  testdrawroundrect(); // Draw rounded rectangles (outlines)

  testfillroundrect(); // Draw rounded rectangles (filled)

  testdrawtriangle();  // Draw triangles (outlines)

  testfilltriangle();  // Draw triangles (filled)

  testdrawchar();      // Draw characters of the default font

  testdrawstyles();    // Draw 'stylized' characters

  testscrolltext();    // Draw scrolling text

  testdrawbitmap();    // Draw a small bitmap image

  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);

  testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}

void loop() {
}

// #include <Arduino.h>
// #include <ESP8266WiFi.h>
// #include <WiFiUdp.h>
// // #include <Adafruit_Sensor.h>

// #include <Wire.h>
// #include <Adafruit_ADXL345_U.h>
// #include <Adafruit_SSD1306.h>

// #include <tft_inclinometer.hpp>


// bool useAccel   = true;
// bool useWiFi    = false;
// bool useTFTscr  = true;

// #define TFT_RST		D4		// TFT RST	pin is connected to NodeMCU pin D4 (GPIO02)
// #define TFT_CS		D3		// TFT CS	pin is connected to NodeMCU pin D3 (GPIO00)
// #define TFT_DC		D0 //EXPERIMENTAL: WORKS: 2		// TFT DC	pin is connected to NodeMCU pin D2 (GPIO04)

// Adafruit_ST7735 tftObj = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// tftInclinometer_ST7735 inclinometer = tftInclinometer_ST7735(&tftObj);

// /*Assign an unique ID to this sensor at the same time*/
// Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// #include <displayFunc.hpp>

// const char*ssid="Telia-47118D";
// const char*password="0C94C28B5D";

// // const char *ssid="Stockholms_stadsbibliotek";
// // const char *password="stockholm";

// WiFiUDP Udp;
// unsigned int localUdpPort=53;//localporttolistenon
// char incomingPacket[255];//bufferforincomingpackets
// char replyPacket[]="Hi there! Got the message:-)";//areplystringtosendback


// float toDegrees(float radians) { return(radians*180)/M_PI; }

// unsigned long t1;
// String oldStr0, oldStr1;
// int frames;


// void setup() {
//     Serial.begin(115200);
//     Serial.flush();
//     // Wire.setClock(400000); //experimental

//     pinMode(D8,OUTPUT);
//     digitalWrite(D8,LOW);
    
//     if(useAccel && !accel.begin()) {
//         /*TherewasaproblemdetectingtheADXL345...checkyourconnections*/
//         Serial.println("Ooops, no ADXL345 detected...Check your wiring!");
//         while(1);
    
        
//         /*Settherangetowhateverisappropriateforyourproject*/
//         accel.setRange(ADXL345_RANGE_16_G);
        
//         /*Displaysomebasicinformationonthissensor*/
//         displaySensorDetails();
        
//         /*Displayadditionalsettings(outsidethescopeofsensor_t)*/
//         displayDataRate();
//         displayRange();
//     }

//     Serial.println();
//     if(useWiFi) {
//         Serial.printf("Connecting to %s",ssid);
//         WiFi.mode(WIFI_STA);
//         WiFi.begin(ssid, password);
//         int blinkCount=0;
//         while(WiFi.status()!=WL_CONNECTED){
//             if(blinkCount==4)digitalWrite(D8,HIGH);
//             delay(500);
//             Serial.print(".");
//             if(blinkCount==4){
//                 digitalWrite(D8,LOW);
//                 blinkCount=0;
//             }
//             blinkCount++;
//         }
//         for(int i=0;i<3;i++){
//             digitalWrite(D8,HIGH);
//             delay(10);
//             digitalWrite(D8,LOW);
//             delay(10);
//         }
//         Serial.println("connected");
//         Udp.begin(localUdpPort);
//         Serial.printf("Now listening at IP %s, UDPport %d\n",WiFi.localIP().toString().c_str(), localUdpPort);
//     }
//     digitalWrite(D8,HIGH);
//     pinMode(0,INPUT_PULLUP);
//     // inclinometer = tftInclinometer_ST7735(&tftObj);
//     if(useTFTscr) inclinometer.tftSetup();

//     t1=millis();
// }

// float X_out=0.1, Y_out=0.1, Z_out=0.1;
// String filterToggle="on";
// int angle=0;

// String totStr;

// void readAccelerometer() {
//     sensors_event_t event;

//     accel.getEvent(&event);

//     Y_out = -event.acceleration.x/10;
//     X_out = -event.acceleration.y/10;
//     Z_out = event.acceleration.z/10;

//     totStr = "read ["+String(X_out,2)+" "+String(Y_out,2)+" "+String(Z_out,2)+"]";

//     Serial.println(totStr);

// }


// void loop() {
  
//     if(useAccel) readAccelerometer();
    
//     if(!useAccel && useTFTscr) {
//         inclinometer.update(
//             sin(radians(45)*sin(radians(angle))),
//             sin(radians(45)*sin(radians(angle+90))),
//             cos(radians(45))
//         );
//     }
//     if(useAccel && useTFTscr)
//         inclinometer.update(X_out, Y_out, Z_out);


//     if(useTFTscr) {
//         inclinometer.drawText(oldStr0, 0, 0, ST7735_BLACK, 1);
//         inclinometer.drawText(oldStr1, 70, 0, ST7735_BLACK, 1);

//         oldStr0 = "FPS:"+String(float(frames*1000)/(millis()-t1),2);
//         oldStr1 = String((millis()-t1))+"ms";

//         inclinometer.drawText(oldStr0, 0, 0, ST7735_RED, 1, false);
//         inclinometer.drawText(oldStr1, 70, 0, ST7735_RED, 1, false);
        
//         t1 = millis();
//         frames=1;
//     }
//     // else frames++;


//     if(useWiFi) {
//         int packetSize=Udp.parsePacket();
//         if(packetSize) {//receiveincomingUDPpackets
//             digitalWrite(D8, LOW);
//             if(digitalRead(0)==HIGH) filterToggle="off;";
//             else filterToggle="on ;";
//             int len = Udp.read(incomingPacket,255);
//             if(len>0){incomingPacket[len]=0;}

//             sensors_event_t event;
//             accel.getEvent(&event);

//             X_out = event.acceleration.x/10;
//             Y_out = event.acceleration.y/10;
//             Z_out = event.acceleration.z/10;

//             String resultStr="{"+
//             String(X_out)+":"+
//             String(Y_out)+":"+
//             String(Z_out)+"}"+filterToggle;//if"off":toggletiltfilteroff
            
//             int newPackLen = resultStr.length()+1;
//             char newReplyPack[newPackLen];
//             resultStr.toCharArray(newReplyPack,newPackLen);

//             Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
//             Udp.write(newReplyPack);
//             Udp.endPacket();
//             digitalWrite(D8, HIGH);
//         }
//     }
//     if(!useAccel) {
//         angle+=10;
//         if(angle>360) angle=0;
//     }
// }

