#pragma once
#ifndef HPP_SSD1306_INCLINOMETER
#define HPP_SSD1306_INCLINOMETER

#include <math.h>

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
    Adafruit_SSD1306 display;

    bool pointerInitialized = false;

    int rollPos[2]      = {5, 5};
    int pitchPos[2]     = {65, 5};
    int accelPos[3][2]  = {
        {5, 20},
        {5, 30},
        {5, 40},
    };
    public:
    float offset_accel[3] = {0, 0, 0};
    
    float accel[3]  = {0.1, 0.1, 0.1};
    float orient[2] = {0, 0};
    
    float toRadians(float degrees);
    float toDegrees(float radians);
    oledInclinometer_SSD1306();
    oledInclinometer_SSD1306(Adafruit_SSD1306 *oled_pointer);

    void printText(String text, int posX, int posY, int textSize=1, bool clearDisp=true, bool callDisplay=true);
    void setup();
    void drawAccel();
    void drawRoll();
    void drawPitch();

    void solveOrients();

    void setAccel(float x, float y, float z);

    void update();
    void update(float xAccel, float yAccel, float zAccel);
};




#endif