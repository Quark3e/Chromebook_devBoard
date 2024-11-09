
#define useWiFi     true
#define useAccel    true

#define builtInLED      2 //used as wifi indicator led
#define LED__1         16


#include <Arduino.h>

#include <WiFi.h>
#include <WiFiUdp.h>


#include <Wire.h>
#include <Adafruit_ADXL345_U.h>


#if useAccel
    Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

#endif

#if useWiFi
    const char *ssid        = "/*ssid*/";
    const char *password    = "/**password/";
    unsigned UDP__PORT = 1089;

    WiFiUDP Udp;
    char incomingPacket[255];
    char replyPacket[] = "Hi there! Got the message.";
#endif

float   toDegrees(float radians) { return(radians*180)/M_PI; }
void    blinkSignal(int pin, int times, int delay_ms=1000);
#if useAccel
    bool    accel_useFilter = false;
    float   accel_filter[3] = {0.1, 0.1, 0.1};
    float   X_out=0.1, Y_out=0.1, Z_out=0.1;
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
        if(!accel.begin()) {
            Serial.println("Can't detect ADXL345 board. Check the wiring!");
            while(1);
        }
        accel.setRange(ADXL345_RANGE_16_G);
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
                sendStr += String(X_out)+":"+String(Y_out)+":"+String(Z_out);
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
    sensors_event_t event;

    accel.getEvent(&event);

    float new_x = -event.acceleration.x/10;
    float new_y = -event.acceleration.y/10;
    float new_z = event.acceleration.z/10;
    if(accel_useFilter) {
        X_out = accel_filter[0]*new_x + (1-accel_filter[0])*X_out;
        Y_out = accel_filter[1]*new_y + (1-accel_filter[1])*Y_out;
        Z_out = accel_filter[2]*new_z + (1-accel_filter[2])*Z_out;
    }
    else if (!accel_useFilter) {
        X_out = event.acceleration.x/10;
        Y_out = event.acceleration.y/10;
        Z_out = event.acceleration.z/10;
    }
    totStr = "read ["+String(X_out,2)+" "+String(Y_out,2)+" "+String(Z_out,2)+"]";

    Serial.println(totStr);
}
#endif