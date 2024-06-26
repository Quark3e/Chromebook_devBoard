
#define useWiFi     true
#define useTFT      false
#define useOLED     false
#define useOLED_ptr true
#define useAccel    false

#define oled_debug  false

bool accel_useFilter    = true;
float accel_filter[3]   = {0.5, 0.5, 0.5};


#include <Arduino.h>

#if useWiFi
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#endif
// #include <Adafruit_Sensor.h>

#if useAccel
#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#endif

#if useTFT
    #include <tft_inclinometer.hpp>

    Adafruit_ST7735 tftObj = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
    tftInclinometer_ST7735 inclinometer = tftInclinometer_ST7735(&tftObj);
#endif
#if useOLED
    #include <SSD1306_inclinometer.hpp>
    #if useOLED_ptr
        Adafruit_SSD1306 display = Adafruit_SSD1306(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, &Wire, OLED_RESET);
        #if !oled_debug
            oledInclinometer_SSD1306 oledInclinometer = oledInclinometer_SSD1306(&display);
        #endif
    #elif !useOLED_ptr
        oledInclinometer_SSD1306 oledInclinometer{};
    #endif
#endif

#if useAccel
    /*Assign an unique ID to this sensor at the same time*/
    Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

    #include <displayFunc.hpp>
#endif

#if useWiFi
    const char *ssid="Telia-47118D";
    // const char *ssid="Telia5GHz-47118D";
    const char *password="0C94C28B5D";
    // const char *ssid="Stockholms_stadsbibliotek";
    // const char *password="stockholm";

    WiFiUDP Udp;
    unsigned int localUdpPort=53;//localporttolistenon
    char incomingPacket[255];//bufferforincomingpackets
    char replyPacket[]="Hi there! Got the message:-)";//areplystringtosendback
#endif

float toDegrees(float radians) { return(radians*180)/M_PI; }

unsigned long t1;
String oldStr0, oldStr1;
int frames;


void blinkSignal(int pin, int times, int delay_ms=1000);

void setup() {
    #if oled_debug
    while(!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_SCREEN_ADDRESS)){
        Serial.println(F("SSD1306 allocation in oled_debug failed"));
        delay(1000);
    }
    Wire.begin(14, 12);
    display.display();
    display.clearDisplay();
    
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(2, 2);
    display.print("oled_debug");

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 20);
    display.print("-test");
    display.display();
    #endif
    Serial.begin(115200);
    Serial.flush();
    // Wire.setClock(400000); //experimental

    pinMode(D8,OUTPUT);
    blinkSignal(D8, 10, 100);
    digitalWrite(D8,LOW);
    

    #if useAccel
        if(!accel.begin()) {
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
    #endif

    // Wire.begin(14, 12);
    Serial.println();
    #if useWiFi
        Serial.printf("Connecting to %s",ssid);
        #if useOLED
            oledInclinometer.printText(
                "Connecting to:",
                5,
                5,
                1
            );
            oledInclinometer.printText(
                "\""+String(ssid)+"\"",
                5,
                16,
                1,
                false
            );
            oledInclinometer.printText(
                " -connecting...",
                5,
                27,
                1,
                false
            );
            char oled_WiFiwaitingText[256];
            char oled_WiFiwaitingSymbols[4] = {'-', '\\', '|', '/'};
        #endif
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        WiFi.setPhyMode(WIFI_PHY_MODE_11B);
        int blinkCount=0;
        while(WiFi.status()!=WL_CONNECTED){
            if(blinkCount==3) digitalWrite(D8,HIGH);
            delay(500);
            Serial.print(".");
            #if useOLED
                // int totLen = 0;
                // for(int ii=0; ii<blinkCount; ii++) {
                //     oled_WiFiwaitingText[ii*2]      = '.';
                //     oled_WiFiwaitingText[ii*2+1]    = ' ';
                //     totLen+=ii*2+1;
                // }
                // if(blinkCount==0) {
                //     for(int ii=0; ii=blinkCount*2; ii++) {
                //         oled_WiFiwaitingText[ii]  = ' ';
                //         totLen+=ii;
                //     }
                // }
                // oled_WiFiwaitingText[totLen+1]  = ' ';
                oled_WiFiwaitingText[0] = oled_WiFiwaitingSymbols[blinkCount];
                display.fillRect(100, 27, 20, 20, SSD1306_BLACK);
                oledInclinometer.printText(String(oled_WiFiwaitingText), 100, 27, 1, false);
            #endif
            blinkCount++;
            if(blinkCount==4){
                digitalWrite(D8,LOW);
                blinkCount=0;
            }
            // blinkCount++;
        }
        for(int i=0;i<3;i++){
            digitalWrite(D8,HIGH);
            delay(10);
            digitalWrite(D8,LOW);
            delay(10);
        }
        Serial.println("connected");
        #if useOLED
            oledInclinometer.printText(
                "\nconnected",
                5, 30, 1, false
            );
        #endif
        Udp.begin(localUdpPort);
        // WiFi.setAutoReconnect(true);
        // WiFi.persistent(true);

        // Udp.setPhyMode(WIFI_PHY_MODE_11B);
        Serial.printf("Now listening at IP %s, UDPport %d\n",WiFi.localIP().toString().c_str(), localUdpPort);
        #if useOLED
            oledInclinometer.printText(
                "Now listeing at IP "+WiFi.localIP().toString()+", UDPport "+String(localUdpPort)+"\n",
                5, 40, 1, true
            );
        #endif
    #endif
    digitalWrite(D8,HIGH);
    pinMode(0,INPUT_PULLUP);
    // inclinometer = tftInclinometer_ST7735(&tftObj);
    #if useTFT
        inclinometer.tftSetup();
    #endif
    
    t1=millis();
}

float X_out=0.069, Y_out=0.069, Z_out=0.069;
String filterToggle="on";
int angle=0;

String totStr;

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


void loop() {
  
    #if useAccel
        readAccelerometer();
    #endif

    #if useAccel && useTFT
        inclinometer.update(X_out, Y_out, Z_out);
    #elif !useAccel && useTFT
        inclinometer.update(
            sin(radians(45)*sin(radians(angle))),
            sin(radians(45)*sin(radians(angle+90))),
            cos(radians(45))
        );
    #endif
    #if useAccel && useOLED && !oled_debug
        oledInclinometer.update(X_out, Y_out, Z_out);
        oledInclinometer.printText(WiFi.localIP().toString(), 50, 55, 1, false, false);
        oledInclinometer.printText(String(localUdpPort), 70, 45, 1, false, false);
    #elif !useAccel && useOLED
        oledInclinometer.update(
            sin(radians(45)*sin(radians(angle))),
            sin(radians(45)*sin(radians(angle+90))),
            cos(radians(45))
        );
    #endif


    #if useTFT
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


    #if useWiFi 
        int packetSize=Udp.parsePacket();
        if(packetSize) {//receiveincomingUDPpackets
            digitalWrite(D8, LOW);
            if(digitalRead(0)==HIGH) filterToggle="off;";
            else filterToggle="on ;";
            int len = Udp.read(incomingPacket,255);
            if(len>0){incomingPacket[len]=0;}

            #if useAccel
            sensors_event_t event;
            accel.getEvent(&event);

            X_out = event.acceleration.x/10;
            Y_out = event.acceleration.y/10;
            Z_out = event.acceleration.z/10;

            #endif

            String resultStr="{"+
            String(X_out)+":"+
            String(Y_out)+":"+
            String(Z_out)+"}"+filterToggle;//if"off":toggletiltfilteroff
            
            int newPackLen = resultStr.length()+1;
            char newReplyPack[newPackLen];
            resultStr.toCharArray(newReplyPack,newPackLen);

            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write(newReplyPack);
            Udp.endPacket();
            digitalWrite(D8, HIGH);
        }
    #endif
    #if !useAccel
        angle+=10;
        if(angle>360) angle=0;
    #endif
    #if !useAccel && !useTFT && !useOLED && !useWiFi
        digitalWrite(D8, HIGH);
        delay(1000);
        digitalWrite(D8, LOW);
        delay(900);
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

