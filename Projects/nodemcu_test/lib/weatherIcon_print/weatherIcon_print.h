#pragma once
#ifndef H_WEATHER_ICON_PRINT
#define H_WEATHER_ICON_PRINT


/// @brief printing OpenWeatherMap api's images on an adafruit st7735 tft
class printOWM_icons {
    void displayBMP();
    
    void bmp01(char dayOrNight, uint16_t baseCol); //d=0; n=1
    void bmp02(char dayOrNight, uint16_t baseCol);
    void bmp03(uint16_t baseCol);
    void bmp04(uint16_t baseCol);
    void bmp09(uint16_t baseCol);
    void bmp10(char dayOrNight, uint16_t baseCol);
    void bmp11(uint16_t baseCol);
    void bmp13(uint16_t baseCol);
    void bmp50(uint16_t baseCol);

    public:

    uint16_t *BMPptr;
    uint16_t *colourKey;
    int lenColourKey;

    int iconWidth=50, iconHeight=50;
    int iconX=0, iconY=0;
    uint16_t bgColor = ST7735_BLACK;

    String prev_iconString = "";


    Adafruit_ST7735 *TFTptr;
    // printOWM_icons();
    printOWM_icons(Adafruit_ST7735 *tft_pointer);
    
    void printIcon(String iconString);
}; 
/*
Might need to find a simpler solution: ex:
- all images with clouds uses the same one just resized and moved
    - all dark clouds use same shape just smaller, behind and darkened.
- all spheres use same shape, just smaller and/or darker moved around
- {"09d", "09n", "10d", "10n"} use same rain, same location, same colour
- {"02d", "02b", "04d", "04n", "11d", "11n"} uses same cloud size, colour and position
*/


#endif