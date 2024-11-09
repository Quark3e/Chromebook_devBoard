/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#define useWiFi     true
#define useAccel    true
#define builtInLED      2 //used as wifi indicator led
#define LED__1         16

/**
 * ID of the current accelerometer board that's being used.
 *  `0` - ADXL345
 *  `1` - MPU6050
 *  `2` - LIS3DH
 */
#define ACCEL__BOARD    2

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiUdp.h>

#if useAccel
    #include <Wire.h>
    #include <Adafruit_Sensor.h>
#if ACCEL__BOARD==0
    #include <Adafruit_ADXL345_U.h>
    Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
#elif ACCEL__BOARD==1
#elif ACCEL__BOARD==2
    #include <SPI.h>
    #include <Adafruit_LIS3DH.h>

    #define LIS3DH_CLK  13
    #define LIS3DH_MISO 12
    #define LIS3DH_MOSI 11
    #define LIS3DH_CS   10

    Adafruit_LIS3DH lis3 = Adafruit_LIS3DH();

    #define CLICKTHRESHOLD 80
#endif
#endif


#if useWiFi
    // const char *ssid  ="Telia5GHz-47118D";
    const char *ssid    = "Telia-47118D";
    const char *password= "0C94C28B5D";
    unsigned UDP__PORT = 1089;

    WiFiUDP Udp;
    char incomingPacket[255];
    char replyPacket[] = "Hi there! Got the message.";
#endif

float   toDegrees(float radians) { return(radians*180)/M_PI; }
void    blinkSignal(int pin, int times, int delay_ms=1000);
#if useAccel
    struct val_out {
        float x;
        float y;
        float z;
        val_out(float par_x, float par_y, float par_z):
        x(par_x), y(par_y), z(par_z) {}
        String exprt(String sep=":") {
            String str = "";
            str += String(x) + ":" + String(y) + ":" + String(z);
            return str;
        }
    };
    struct val_out A_out{0.1, 0.1, 0.1}, G_out{0.1, 0.1, 0.1};

    bool    accel_useFilter = false;
    bool    gyro__useFilter = false;
    float   accel_filter[3] = {0.1, 0.1, 0.1};
    float   gyro__filter[3] = {0.1, 0.1, 0.1};
    
    
    String  filterToggle="on";
    String  totStr;
    void    readAccelerometer();
#endif

void setup() {
    Serial.begin(115200);
    Serial.flush();

    pinMode(builtInLED, OUTPUT);
    pinMode(LED__1, OUTPUT);

    blinkSignal(LED__1, 10, 100);
    blinkSignal(builtInLED, 10, 100);
    
#if useAccel

#if ACCEL__BOARD==0
    if(!accel.begin()) {
        Serial.println("Can't detect ADXL345 board. Check the wiring!");
        while(1);
    }
    accel.setRange(ADXL345_RANGE_16_G);
#elif ACCEL__BOARD==1
#elif ACCEL__BOARD==2
    if(!lis3.begin(0x18)) {
        Serial.println("Can't detect LIS3DH board. Check the wiring!");
        while(1);
    }
    lis3.setRange(LIS3DH_RANGE_2_G);
    lis3.setClick(2, CLICKTHRESHOLD);
#endif

#endif

    Serial.println();
#if useWiFi
        Serial.printf("Connecting to %s", ssid);
        // WiFi.setPhyMode(WIFI_PHY_MODE_11B);
        WiFi.setTxPower(WIFI_POWER_19_5dBm);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        int blinkCount=0;
        while(WiFi.status()!=WL_CONNECTED) {
            if(blinkCount==3) digitalWrite(builtInLED, HIGH);
            delay(500);
            Serial.print(".");

            blinkCount++;
            if(blinkCount>=4) {
                digitalWrite(builtInLED, LOW);
                blinkCount=0;
            }
        }
        blinkSignal(builtInLED, 3, 10);
        Serial.println("connected!");
        Udp.begin(UDP__PORT);
        Serial.printf("Now listening at IP %s, UDPport %d\n",WiFi.localIP().toString().c_str(), UDP__PORT);
#endif
    digitalWrite(builtInLED, LOW);
}

void loop() {
#if useAccel
        readAccelerometer();
#endif

#if useWiFi
        int packetSize = Udp.parsePacket();
        if(packetSize) {
            digitalWrite(builtInLED, HIGH);
            if(digitalRead(0)==HIGH) filterToggle = "off;";
            else filterToggle = "on ;";

            int len = Udp.read(incomingPacket, 255);
            if(len>0) incomingPacket[len] = 0;

            String sendStr = "{";
#if useAccel
                sendStr += String(A_out.x)+":"+String(A_out.y)+":"+String(A_out.z);
#else
                sendStr += String(replyPacket);
#endif
            sendStr+="}"+filterToggle;

            int newPackLen = sendStr.length()+1;
            char newReplyPack[newPackLen];
            sendStr.toCharArray(newReplyPack, newPackLen);
            uint8_t nRP[newPackLen];
            for(int i=0; i<newPackLen; i++) {
                nRP[i] = newReplyPack[i];
            }

            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write(nRP, newPackLen);
            Udp.endPacket();
            digitalWrite(builtInLED, LOW);

        }
#endif
}

void blinkSignal(int pin, int times, int delay_ms) {
    for(int i=0; i<times; i++) {
        digitalWrite(pin, HIGH);
        delay(delay_ms);
        digitalWrite(pin, LOW);
        delay(delay_ms);
    }
}

#if useAccel
void readAccelerometer() {
#if ACCEL__BOARD==0 || ACCEL__BOARD==2
    sensors_event_t event;

#if ACCEL__BOARD==0
    accel.getEvent(&event);
#else
    lis3.getEvent(&event);
#endif

    float new_x = -event.acceleration.x/10;
    float new_y = -event.acceleration.y/10;
    float new_z = event.acceleration.z/10;
    if(accel_useFilter) {
        A_out.x = accel_filter[0]*new_x + (1-accel_filter[0])*A_out.x;
        A_out.y = accel_filter[1]*new_y + (1-accel_filter[1])*A_out.y;
        A_out.z = accel_filter[2]*new_z + (1-accel_filter[2])*A_out.z;
    }
    else if (!accel_useFilter) {
        A_out.x = event.acceleration.x/10;
        A_out.y = event.acceleration.y/10;
        A_out.z = event.acceleration.z/10;
    }
    // totStr = "read ["+String(A_out.x, 2)+" "+String(A_out.y, 2)+" "+String(A_out.z, 2)+"]";

#elif ACCEL__BOARD==1
    sensors_event_t accl, gyro, temp;

    mpu.getEvent(&accl, &gyro, &temp);
    if(accel_useFilter) {
        float new_Ax = -accl.acceleration.x/10;
        float new_Ay = -accl.acceleration.y/10;
        float new_Az = accl.acceleration.z/10;
        A_out.x = accel_filter[0]*new_Ax + (1-accel_filter[0])*A_out.x;
        A_out.y = accel_filter[1]*new_Ay + (1-accel_filter[1])*A_out.y;
        A_out.z = accel_filter[2]*new_Az + (1-accel_filter[2])*A_out.z;
    }
    else if (!accel_useFilter) {
        A_out.x = accl.acceleration.x/10;
        A_out.y = accl.acceleration.y/10;
        A_out.z = accl.acceleration.z/10;
    }
    if(gyro__useFilter) {
        float new_Gx = gyro.gyro.x;
        float new_Gy = gyro.gyro.y;
        float new_Gz = gyro.gyro.z;
        G_out.x = gyro__filter[0]*new_Gx + (1-gyro__filter[0])*G_out.x;
        G_out.y = gyro__filter[1]*new_Gy + (1-gyro__filter[1])*G_out.y;
        G_out.z = gyro__filter[2]*new_Gz + (1-gyro__filter[2])*G_out.z;
    }
    else if (!gyro__useFilter) {
        G_out.x = gyro.gyro.x;
        G_out.y = gyro.gyro.y;
        G_out.z = gyro.gyro.z;
    }
    board_temp = temp.temperature;
    // totStr = "read ["+
    //     String(A_out.x, 2)+" "
    //     "]";
#endif

    // Serial.println(totStr);
}
#endif