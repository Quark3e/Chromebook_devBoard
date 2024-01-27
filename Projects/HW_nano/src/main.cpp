
#include <Arduino.h>

// #include <Adafruit_Sensor.h>

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

#include <tft_inclinometer.hpp>


bool useAccel   = true;
bool useWiFi    = false;
bool useTFTscr  = true;

#define TFT_RST		4		// TFT RST	pin is connected to NodeMCU pin D4 (GPIO02)
#define TFT_CS		8		// TFT CS	pin is connected to NodeMCU pin D3 (GPIO00)
#define TFT_DC		6 //EXPERIMENTAL: WORKS: 2		// TFT DC	pin is connected to NodeMCU pin D2 (GPIO04)

int ledPin = 3;

Adafruit_ST7735 tftObj = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

tftInclinometer_ST7735 inclinometer = tftInclinometer_ST7735(&tftObj);

/*Assign an unique ID to this sensor at the same time*/
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

#include <displayFunc.hpp>


float toDegrees(float radians) { return(radians*180)/M_PI; }

unsigned long t1;
String oldStr0, oldStr1;
int frames;


void setup() {
    Serial.begin(115200);
    Serial.flush();
    // Wire.setClock(400000); //experimental

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    delay(500);
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);

    if(useAccel && !accel.begin()) {
        /*TherewasaproblemdetectingtheADXL345...checkyourconnections*/
        Serial.println("Ooops, no ADXL345 detected...Check your wiring!");
        while(1);
    
        
        /*Settherangetowhateverisappropriateforyourproject*/
        accel.setRange(ADXL345_RANGE_16_G);
        
        /*Displaysomebasicinformationonthissensor*/
        displaySensorDetails();
        
        /*Displayadditionalsettings(outsidethescopeofsensor_t)*/
        displayDataRate();
        displayRange();
    }

    Serial.println();

    digitalWrite(ledPin, HIGH);
    pinMode(0,INPUT_PULLUP);
    // inclinometer = tftInclinometer_ST7735(&tftObj);
    if(useTFTscr) inclinometer.tftSetup();

    t1=millis();
}

float X_out=0.1, Y_out=0.1, Z_out=0.1;
String filterToggle="on";
int angle=0;

String totStr;

void readAccelerometer() {
    sensors_event_t event;

    accel.getEvent(&event);

    X_out = event.acceleration.x/10;
    Y_out = event.acceleration.y/10;
    Z_out = event.acceleration.z/10;

    totStr = "read ["+String(X_out,2)+" "+String(Y_out,2)+" "+String(Z_out,2)+"]";

    Serial.println(totStr);

}


void loop() {
    if(useAccel) analogWrite(A6, float(angle)/361*1024);

    if(useAccel) readAccelerometer();
    
    if(!useAccel && useTFTscr) {
        inclinometer.update(
            sin(radians(45)*sin(radians(angle))),
            sin(radians(45)*sin(radians(angle+90))),
            cos(radians(45))
        );
    }
    if(useAccel && useTFTscr)
        inclinometer.update(Y_out, X_out, Z_out);


    if(useTFTscr) {
        inclinometer.drawText(oldStr0, 0, 0, ST7735_BLACK, 1);
        inclinometer.drawText(oldStr1, 70, 0, ST7735_BLACK, 1);

        oldStr0 = "FPS:"+String(float(frames*1000)/(millis()-t1),2);
        oldStr1 = String((millis()-t1))+"ms";

        inclinometer.drawText(oldStr0, 0, 0, ST7735_RED, 1, false);
        inclinometer.drawText(oldStr1, 70, 0, ST7735_RED, 1, false);
        
        t1 = millis();
        frames=1;
    }
    // else frames++;

    if(!useAccel) {
        angle+=10;
        if(angle>360) angle=0;
    }
}

