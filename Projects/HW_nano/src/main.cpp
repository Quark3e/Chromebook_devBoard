

#define useAccel    true
#define useScr      true

/*
whichScr =
0   tft     ST7735
1   oled    SSD1306
*/
#define whichScr    1


/*
whichAccel =
0   adxl345
1   mpu6050
*/
#define whichAccel  1


#include <Arduino.h>
#if useAccel
    #include <Wire.h>
    #include <Adafruit_Sensor.h>
    #if whichAccel  == 0
        #include <Adafruit_ADXL345_U.h>
        /*Assign an unique ID to this sensor at the same time*/
        Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

        #include <adxl345_displayFunc.hpp>

    #elif whichAccel== 1
        #include <Adafruit_MPU6050.h>
        Adafruit_MPU6050 mpuAccel;
    #endif
#endif

#if useScr
    #include <Adafruit_GFX.h>
    #if whichScr    == 0
        #include <tft_inclinometer.hpp>

        Adafruit_ST7735 tftObj = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
        tftInclinometer_ST7735 inclinometer = tftInclinometer_ST7735(&tftObj);

    #elif whichScr  == 1
        #include <SSD1306_inclinometer.hpp>

        Adafruit_SSD1306 display = Adafruit_SSD1306(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, &Wire, OLED_RESET);
        oledInclinometer_SSD1306 oledInclinometer = oledInclinometer_SSD1306(&display);
    #endif
#endif



int ledPin = 13;


float accelLim[3] = {
    1.5,
    1.5,
    0
};

String negativeSpace(int inp);
String negativeSpace(float inp, int decimalPl);
String negativeSpace(String inp);

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

    #if useAccel
        #if whichAccel==0
            /*TherewasaproblemdetectingtheADXL345...checkyourconnections*/
            while(!accel.begin()) {
                Serial.println("Ooops, no ADXL345 detected...Check your wiring!");
                delay(1000);
            }
            
            /*Settherangetowhateverisappropriateforyourproject*/
            accel.setRange(ADXL345_RANGE_16_G);
            
            /*Displaysomebasicinformationonthissensor*/
            displaySensorDetails(&accel);
            
            /*Displayadditionalsettings(outsidethescopeofsensor_t)*/
            displayDataRate(&accel);
            displayRange(&accel);
        #elif whichAccel==1
            while(!mpuAccel.begin()) {
                Serial.println("Failed to find MPU6050 chip");
                delay(1000);
            }
            Serial.println("MPU6050 Found!");

            mpuAccel.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
            mpuAccel.setMotionDetectionThreshold(1);
            mpuAccel.setMotionDetectionDuration(20);
            mpuAccel.setInterruptPinLatch(true);
            mpuAccel.setInterruptPinPolarity(true);
            mpuAccel.setMotionInterrupt(true);
        #endif
    #endif
    #if useScr
        #if whichScr==0

        #elif whichScr==1
            display.fillRect(0, 0, 128, 32, SSD1306_BLACK);
            oledInclinometer.printText("setup() init.", 5, 5, 1, true);
        #endif
    #endif


    Serial.println();

    digitalWrite(ledPin, HIGH);
    pinMode(0,INPUT_PULLUP);
    // inclinometer = tftInclinometer_ST7735(&tftObj);
    #if useScr && whichScr==0
        inclinometer.tftSetup();
    #endif
    t1=millis();
}

float X_out=0.1, Y_out=0.1, Z_out=0.1;
float X_gyr=0.1, Y_gyr=0.1, Z_gyr=0.1;
String filterToggle="on";
int angle=0;

String totStr;

#if useAccel
    #if whichAccel==0
    void readAccelerometer() {
        sensors_event_t event;

        accel.getEvent(&event);

        X_out = event.acceleration.x/10;
        Y_out = event.acceleration.y/10;
        Z_out = event.acceleration.z/10;

        if(Y_out>accelLim[1]) {
            digitalWrite(ledPin, HIGH);
            delay(100);
        }
        else {
            digitalWrite(ledPin, LOW);
        }

        totStr = "read ["+String(X_out,2)+" "+String(Y_out,2)+" "+String(Z_out,2)+"]";
        Serial.println(totStr);
    }
    #elif whichAccel==1
    void readAccelerometer(bool printResult=false) {
        // if(mpuAccel.getMotionInterruptStatus()) {
            sensors_event_t mpu_a, mpu_g, mpu_temp;

            mpuAccel.getEvent(&mpu_a, &mpu_g, &mpu_temp);

            X_out = mpu_a.acceleration.x/10;
            Y_out = mpu_a.acceleration.y/10;
            Z_out = mpu_a.acceleration.z/10;

            X_gyr = mpu_g.gyro.x;
            Y_gyr = mpu_g.gyro.y;
            Z_gyr = mpu_g.gyro.z;

            if(printResult) {
                Serial.print("{a_x:");
                Serial.print(negativeSpace(X_out, 1));
                Serial.print(",");
                Serial.print(" a_y:");
                Serial.print(negativeSpace(Y_out, 1));
                Serial.print(",");
                Serial.print(" a_z:");
                Serial.print(negativeSpace(Z_out, 1));
                Serial.print("}, ");
                Serial.print("{g_x:");
                Serial.print(negativeSpace(X_gyr, 2));
                Serial.print(",");
                Serial.print(" g_y:");
                Serial.print(negativeSpace(Y_gyr, 2));
                Serial.print(",");
                Serial.print(" g_z:");
                Serial.print(negativeSpace(Z_gyr, 2));
                Serial.print("}, ");
                Serial.print("Temp:");
                Serial.print(negativeSpace(mpu_temp.temperature, 2));
                Serial.println("");
            }
        // }
    }
    #endif
#endif



void loop() {
    digitalWrite(ledPin, LOW);
    #if useAccel
        #if whichAccel==0
            readAccelerometer();
        #elif whichAccel==1
            readAccelerometer(true);
            delay(10);
        #endif
        #if useScr
            #if whichScr==0
                inclinometer.update(Y_out, X_out, Z_out);
            #elif whichScr==1
                oledInclinometer.update(X_out, Y_out, Z_out);
            #endif
        #endif
    #endif
    #if !useAccel && (useScr && whichScr==0)
        inclinometer.update(
            sin(radians(45)*sin(radians(angle))),
            sin(radians(45)*sin(radians(angle+90))),
            cos(radians(45))
        );
    #endif

    #if useScr && whichScr==0
        inclinometer.drawText(oldStr0, 0, 0, ST7735_BLACK, 1);
        inclinometer.drawText(oldStr1, 70, 0, ST7735_BLACK, 1);

        oldStr0 = "FPS:"+String(float(frames*1000)/(millis()-t1),2);
        oldStr1 = String((millis()-t1))+"ms";

        inclinometer.drawText(oldStr0, 0, 0, ST7735_RED, 1, false);
        inclinometer.drawText(oldStr1, 70, 0, ST7735_RED, 1, false);
        
        t1 = millis();
        frames=1;
    #endif
    // else frames++;

    #if (!useAccel)
        angle+=10;
        if(angle>360) angle=0;
    #endif
}


/// @brief Add an empty space char in front of string incase the String number is >=0
/// @param inp integer input
/// @return modified inp
String negativeSpace(int inp) {
    String result = "";
    if(inp>=0) {
        result = " "+String(inp);
    }
    else {
        result = String(inp);
    }
    return result;
}/// @brief Add an empty space char in front of string incase the String number is >=0
/// @param inp floating point input
/// @return modified inp
String negativeSpace(float inp, int decimalPl=1) {
    String result = "";
    if(inp>=0) {
        result = " "+String(inp, decimalPl);
    }
    else {
        result = String(inp, decimalPl);
    }
    return result;
}
/// @brief Add an empty space char in front of string incase the String number is >=0
/// @param inp string input
/// @return modified inp
String negativeSpace(String inp) {
    String result = "";
    if(inp.toFloat()>=0) {
        result = " "+inp;
    }
    else {
        result = inp;
    }
    return result;
}
