#include <Arduino.h>

/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */
 
// include TFT and SPI libraries
#include <TFT_eSPI>
#include <SPI.h>

// pin definition for Arduino UNO
#define TFT_RST		D4		// TFT RST	pin is connected to NodeMCU pin D4 (GPIO02)
#define TFT_CS		D3		// TFT CS	pin is connected to NodeMCU pin D3 (GPIO00)
#define TFT_DC		D0 //EXPERIMENTAL: WORKS: 2		// TFT DC	pin is connected to NodeMCU pin D2 (GPIO04)


// create an instance of the library
TFT_eSPI TFTscreen = TFT(TFT_CS, TFT_DC, TFT_RST);

void setup() {

    //initialize the library
    TFTscreen.begin();

    // clear the screen with a black background
    TFTscreen.background(0, 0, 0);
}

void loop() {

    //generate a random color
    int redRandom = random(0, 255);
    int greenRandom = random (0, 255);
    int blueRandom = random (0, 255);

    // set the color for the figures
    TFTscreen.stroke(redRandom, greenRandom, blueRandom);

    // light up a single point
    TFTscreen.point(80,64);
    // wait 200 miliseconds until change to next figure
    delay(500);

    // draw a line
    TFTscreen.line(0,64,160,64);
    delay(500);

    //draw a square
    TFTscreen.rect(50,34,60,60);
    delay(500);

    //draw a circle
    TFTscreen.circle(80,64,30);
    delay(500);

    //erase all figures
    TFTscreen.background(0,0,0);
}