#pragma once
#ifndef HPP_SSD1306_INCLINOMETER
#define HPP_SSD1306_INCLINOMETER

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SSD1306_SCREEN_WIDTH    128 //OLED display width in pixels
#define SSD1306_SCREEN_HEIGHT   64 //OLED display height in pixels (though note: ssd1306 is 32, but works with 64 for some reason)

#define OLED_RESET              -1
#define SSD1306_SCREEN_ADDRESS  0x3C

class oledInclinometer_SSD1306 {
    Adafruit_SSD1306 *displayPtr;


    oledInclinometer_SSD1306(Adafruit_SSD1306 *oled_pointer);

};


#endif