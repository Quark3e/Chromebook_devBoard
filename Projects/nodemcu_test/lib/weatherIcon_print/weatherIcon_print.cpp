
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library

#include <rawBMP_arrays.h>
#include <weatherIcon_print.h>

/// @brief Class constructor
/// @param tft_pointer pointer to Adafruit_ST7735 instance object.
printOWM_icons::printOWM_icons(Adafruit_ST7735 *tft_pointer) {
    TFTptr = tft_pointer;
}


/// @brief Print weather icon onto the st7735 screen
/// @param iconString number-letter value of weather
void printOWM_icons::printIcon(String iconString) {
    if(iconString==prev_iconString) return;
    prev_iconString = iconString;
    char dayOrNight = ' ';
    if(prev_iconString.indexOf("d") != -1) dayOrNight = 'd';
    else if(prev_iconString.indexOf("n") != -1) dayOrNight = 'n';
    if(prev_iconString.substring(0, 2)=="01") bmp01(dayOrNight, 0x0000);
    else if(prev_iconString.substring(0, 2)=="02") bmp02(dayOrNight, 0x0000);
    else if(prev_iconString.substring(0, 2)=="03") bmp03(0x0000);
    else if(prev_iconString.substring(0, 2)=="04") bmp04(0x0000);
    else if(prev_iconString.substring(0, 2)=="09") bmp09(0x0000);
    else if(prev_iconString.substring(0, 2)=="10") bmp10(dayOrNight, 0x0000);
    else if(prev_iconString.substring(0, 2)=="11") bmp11(0x0000);
    else if(prev_iconString.substring(0, 2)=="13") bmp13(0x0000);
    else if(prev_iconString.substring(0, 2)=="50") bmp50(0x0000);
    // TFTptr->fillRect(iconX, iconY, iconWidth, iconHeight, bgColor);

    
    if(iconString.indexOf("d") != -1) dayOrNight = 'd';
    else if(iconString.indexOf("n") != -1) dayOrNight = 'n';
    else {
        Serial.println("Error: wrong iconString argument.");
        return;
    }
    if(iconString.substring(0, 2)=="01") bmp01(dayOrNight, 1);
    else if(iconString.substring(0, 2)=="02") bmp02(dayOrNight, 1);
    else if(iconString.substring(0, 2)=="03") bmp03(1);
    else if(iconString.substring(0, 2)=="04") bmp04(1);
    else if(iconString.substring(0, 2)=="09") bmp09(1);
    else if(iconString.substring(0, 2)=="10") bmp10(dayOrNight, 1);
    else if(iconString.substring(0, 2)=="11") bmp11(1);
    else if(iconString.substring(0, 2)=="13") bmp13(1);
    else if(iconString.substring(0, 2)=="50") bmp50(1);
    else {
        Serial.println("ERROR: couldn't read number.");
        return;
    }
    // displayBMP();
}



void printOWM_icons::displayBMP() {
    // uint16_t tempColour;
    for(int col=0; col<lenColourKey; col++) {
        for(int i=0; i<(iconWidth*iconHeight); i++) {
            if(BMPptr[i]==colourKey[col]) printTemp[i] = 0xFF;
            else printTemp[i] = 0x00;
        }

    digitalWrite(D8, LOW);
    delay(1000);
    digitalWrite(D8, HIGH);
    delay(1000);
        TFTptr->drawBitmap(iconX, iconY, printTemp, 50, 50, colourKey[col]);
        delay(1000);
    }
    // for(int i=0; i<sizeof(keyColours)/sizeof(keyColours[0]); i++) {}
}

void printOWM_icons::bmp01(char dayOrNight, uint16_t baseCol=1) {
    if(baseCol==0) {
        if(dayOrNight=='d') {
            TFTptr->drawBitmap(iconX, iconY, circles01Bitmap, iconWidth, iconHeight, baseCol);
            BMPptr = (uint16_t*)bgr565_01d;
            colourKey = key_01d;
            lenColourKey = 19;
        }
        else {
            TFTptr->drawBitmap(iconX, iconY, circles01Bitmap, iconWidth, iconHeight, baseCol);
            BMPptr = (uint16_t*)bgr565_01n;
            colourKey = key_01n;
            lenColourKey = 8;
        }
        return;
    }
    if(dayOrNight=='d') {
        TFTptr->drawBitmap(iconX, iconY, circles01Bitmap, iconWidth, iconHeight, 0xeb69);
        BMPptr = (uint16_t*)bgr565_01d;
        colourKey = key_01d;
        lenColourKey = 19;
    }
    else {
        TFTptr->drawBitmap(iconX, iconY, circles01Bitmap, iconWidth, iconHeight, 0x4a49);
        BMPptr = (uint16_t*)bgr565_01n;
        colourKey = key_01n;
        lenColourKey = 8;
    }
}
void printOWM_icons::bmp02(char dayOrNight, uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, whiteClouds02Bitmap, iconWidth, iconHeight, baseCol);
        if(dayOrNight=='d') { TFTptr->drawBitmap(iconX, iconY, circles02Bitmap, iconWidth, iconHeight, baseCol); }    
        else { TFTptr->drawBitmap(iconX, iconY, circles02Bitmap, iconWidth, iconHeight, baseCol); }
        return;
    }

    TFTptr->drawBitmap(iconX, iconY, whiteClouds02Bitmap, iconWidth, iconHeight, 0xf79e);
    if(dayOrNight=='d') {
        TFTptr->drawBitmap(iconX, iconY, circles02Bitmap, iconWidth, iconHeight, 0xeb69);
        BMPptr = (uint16_t*)bgr565_02d;
        colourKey = key_02d;
        lenColourKey = 43;
    }    
    else {
        TFTptr->drawBitmap(iconX, iconY, circles02Bitmap, iconWidth, iconHeight, 0x4a49);
        BMPptr = (uint16_t*)bgr565_02n;
        colourKey = key_02n;
        lenColourKey = 41;
    }
}
void printOWM_icons::bmp03(uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, whiteClouds03Bitmap, iconWidth, iconHeight, baseCol);
        return;
    }
    TFTptr->drawBitmap(iconX, iconY, whiteClouds03Bitmap, iconWidth, iconHeight, 0xf79e);
    BMPptr = (uint16_t*)bgr565_03;
    colourKey = key_03;
    lenColourKey = 9;
}
void printOWM_icons::bmp04(uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, whiteClouds04Bitmap, iconWidth, iconHeight, baseCol);
        TFTptr->drawBitmap(iconX, iconY, greyClouds04Bitmap, iconWidth, iconHeight, baseCol);
        return;
    }
    TFTptr->drawBitmap(iconX, iconY, whiteClouds04Bitmap, iconWidth, iconHeight, 0xf79e);
    TFTptr->drawBitmap(iconX, iconY, greyClouds04Bitmap, iconWidth, iconHeight, 0x4a49);
    BMPptr = (uint16_t*)bgr565_04;
    colourKey = key_04;
    lenColourKey = 44;
}
void printOWM_icons::bmp09(uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, whiteClouds09Bitmap, iconWidth, iconHeight, baseCol);
        TFTptr->drawBitmap(iconX, iconY, greyClouds09Bitmap, iconWidth, iconHeight, baseCol);
        TFTptr->drawBitmap(iconX, iconY, rainBitmap, iconWidth, iconHeight, baseCol);
        return;
    }
    TFTptr->drawBitmap(iconX, iconY, whiteClouds09Bitmap, iconWidth, iconHeight, 0xf79e);
    TFTptr->drawBitmap(iconX, iconY, greyClouds09Bitmap, iconWidth, iconHeight, 0xf79e);
    TFTptr->drawBitmap(iconX, iconY, rainBitmap, iconWidth, iconHeight, 0x4a49);
    BMPptr = (uint16_t*)bgr565_09;
    colourKey = key_09;
    lenColourKey = 40;
}
void printOWM_icons::bmp10(char dayOrNight, uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, whiteClouds10Bitmap, iconWidth, iconHeight, baseCol);
        TFTptr->drawBitmap(iconX, iconY, rainBitmap, iconWidth, iconHeight, baseCol);
        if(dayOrNight=='d') {
            TFTptr->drawBitmap(iconX, iconY, circles10Bitmap, iconWidth, iconHeight, baseCol);
        }
        else {
            TFTptr->drawBitmap(iconX, iconY, circles10Bitmap, iconWidth, iconHeight, baseCol);
        }
        return;
    }
    if(dayOrNight=='d') {
        // TFTptr->drawBitmap(iconX, iconY, circles10Bitmap, iconWidth, iconHeight, 0xeb69);
        BMPptr = (uint16_t*)bgr565_10d;
        colourKey = key_10d;
        lenColourKey = 61;
    }
    else {
        TFTptr->drawBitmap(iconX, iconY, circles10Bitmap, iconWidth, iconHeight, 0x4a49);
        BMPptr = (uint16_t*)bgr565_10n;
        colourKey = key_10n;
        lenColourKey = 46;
    }
    TFTptr->drawBitmap(iconX, iconY, rainBitmap, iconWidth, iconHeight, 0x4a49);
    // TFTptr->drawBitmap(iconX, iconY, whiteClouds10Bitmap, iconWidth, iconHeight, 0xf79e);
}
void printOWM_icons::bmp11(uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, whiteClouds11Bitmap, iconWidth, iconHeight, baseCol);
        TFTptr->drawBitmap(iconX, iconY, greyClouds11Bitmap, iconWidth, iconHeight, baseCol);
        TFTptr->drawBitmap(iconX, iconY, lightningBitmap, iconWidth, iconHeight, baseCol);
        return;
    }
    TFTptr->drawBitmap(iconX, iconY, whiteClouds11Bitmap, iconWidth, iconHeight, 0xf79e);
    TFTptr->drawBitmap(iconX, iconY, greyClouds11Bitmap, iconWidth, iconHeight, 0x4a49);
    TFTptr->drawBitmap(iconX, iconY, lightningBitmap, iconWidth, iconHeight, 0xeb69);
    BMPptr = (uint16_t*)bgr565_11;
    colourKey = key_01d;
    lenColourKey = 70;
}
void printOWM_icons::bmp13(uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, snowBitmap, iconWidth, iconHeight, 0x4a49);
        return;
    }
    TFTptr->drawBitmap(iconX, iconY, snowBitmap, iconWidth, iconHeight, 0x4a49);
    BMPptr = (uint16_t*)bgr565_13;
    colourKey = key_13;
    lenColourKey = 16;
}
void printOWM_icons::bmp50(uint16_t baseCol=1) {
    if(baseCol==0) {
        TFTptr->drawBitmap(iconX, iconY, fogBitmap, iconWidth, iconHeight, baseCol);
        return;
    }
    TFTptr->drawBitmap(iconX, iconY, fogBitmap, iconWidth, iconHeight, 0x4a49);
    BMPptr = (uint16_t*)bgr565_50;
    colourKey = key_13;
    lenColourKey = 10;
}