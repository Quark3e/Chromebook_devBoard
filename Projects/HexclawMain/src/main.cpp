
#define useWiFi     true
#define useTFT      false
#define useOLED     true
#define useOLED_ptr true
#define useAccel    true

#define oled_debug  false

/**
 * ID for using the different accelerometer boards.
 *  `0` - ADXL345
 *  `1` - MPU605
 * 
 */
#define ACCEL__BOARD    1


#include <Arduino.h>

#if useWiFi
    #include <ESP8266WiFi.h>
    #include <WiFiUdp.h>

    #define BOOT_WIFIBUTTON D7
    #define BOOT_WIFILED    D8
    bool dynamic_true = true;
#endif
// #include <Adafruit_Sensor.h>

#if useAccel
    #include <Wire.h>
#if ACCEL__BOARD==0
    #include <Adafruit_ADXL345_U.h>
#elif ACCEL__BOARD==1
    #include <Adafruit_MPU6050.h>
    #include <Adafruit_Sensor.h>
#endif
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
    // Struct for holding x, y, values
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
    struct val_out A_out{0.1, 0.1, 0.1};

    bool accel_useFilter    = false;
    bool gyro__useFilter    = false;
    float accel_filter[3]   = {0.1, 0.1, 0.1};
    float gyro__filter[3]   = {0.1, 0.1, 0.1};

#if ACCEL__BOARD==0
    Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
    #include <displayFunc.hpp>

    // float X_out=0.069, Y_out=0.069, Z_out=0.069;
#elif ACCEL__BOARD==1
    Adafruit_MPU6050 mpu;

    struct val_out G_out{0.1, 0.1, 0.1};
    float board_temp = 0;

#endif
#endif

#if useWiFi
    const char *ssid="Telia-47118D";
    // const char *ssid="Telia5GHz-47118D";
    const char *password="0C94C28B5D";
    // const char *ssid="Stockholms_stadsbibliotek";
    // const char *password="stockholm";

    WiFiUDP Udp;
    /**
     * The port for which the udp server will be running on:
     *  - `1087` for HW364A-esp8266 board
     *  - `1089` for nodemcu esp8266 board
     */
    unsigned int localUdpPort=1089;//localporttolistenon
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


    pinMode(0, INPUT_PULLUP);
    pinMode(D7, INPUT_PULLUP);
    pinMode(D8, OUTPUT);
    blinkSignal(D8, 10, 100);
    digitalWrite(D8, LOW);
    

    #if useAccel
    #if ACCEL__BOARD==0
        if(!accel.begin()) {
            /*TherewasaproblemdetectingtheADXL345...checkyourconnections*/
            Serial.println("No ADXL345 board detected. Check the wiring!");
            while(1);
        }
        /*Settherangetowhateverisappropriateforyourproject*/
        accel.setRange(ADXL345_RANGE_16_G);
        
        /*Displaysomebasicinformationonthissensor*/
        displaySensorDetails();
        
        /*Displayadditionalsettings(outsidethescopeofsensor_t)*/
        displayDataRate();
        displayRange();

    #elif ACCEL__BOARD==1
        if(!mpu.begin()) {
            Serial.println("No MPU6050 board detected. Check the wiring!");
            while(1);
        }
        mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
        mpu.setMotionDetectionThreshold(1);
        mpu.setMotionDetectionDuration(20);
        mpu.setInterruptPinLatch(true);
        mpu.setInterruptPinPolarity(true);
        mpu.setMotionInterrupt(true);
    #endif
    #endif

    // Wire.begin(14, 12);
    Serial.println();
    #if useWiFi
        dynamic_true = useWiFi;
        delay(750);
        blinkSignal(BOOT_WIFILED, 5, 50);
        delay(750);
        if(digitalRead(BOOT_WIFIBUTTON)==LOW) {
            Serial.println("Wifi mode is off.");
            dynamic_true = false;
            blinkSignal(BOOT_WIFILED, 3, 200); // O
            delay(750);

            blinkSignal(BOOT_WIFILED, 2, 200);
            delay(200);
            digitalWrite(BOOT_WIFILED, HIGH);
            delay(600);
            digitalWrite(BOOT_WIFILED, LOW);
            delay(200);
            digitalWrite(BOOT_WIFILED, HIGH);
            delay(200);
            digitalWrite(BOOT_WIFILED, LOW);
            delay(200);

            delay(750);
            blinkSignal(BOOT_WIFILED, 2, 200);
            delay(200);
            digitalWrite(BOOT_WIFILED, HIGH);
            delay(600);
            digitalWrite(BOOT_WIFILED, LOW);
            delay(200);
            digitalWrite(BOOT_WIFILED, HIGH);
            delay(200);
            digitalWrite(BOOT_WIFILED, LOW);
            delay(200);
        }
        else {
            Serial.println("Wifi mode is on.");
            blinkSignal(BOOT_WIFILED, 3, 200);
            delay(750);

            digitalWrite(BOOT_WIFILED, HIGH);
            delay(600);
            digitalWrite(BOOT_WIFILED, LOW);
            delay(200);
            digitalWrite(BOOT_WIFILED, HIGH);
            delay(200);
            digitalWrite(BOOT_WIFILED, LOW);
            delay(200+750);
        }
        delay(750);
        blinkSignal(BOOT_WIFILED, 5, 50);
        delay(100);

        if(dynamic_true) {
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
                    "port: "+String(localUdpPort),
                    5, 27, 1, false
                );
                oledInclinometer.printText(
                    " -connecting...",
                    5,
                    38,
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
                    "Now listening at IP ", //+WiFi.localIP().toString()+", UDPport "+String(localUdpPort)+"\n",
                    5, 5, 1, true
                );
                oledInclinometer.printText(
                    WiFi.localIP().toString(),
                    5, 16, 1, false
                );
                oledInclinometer.printText(
                    "port: " + String(localUdpPort),
                    5, 27, 1, false
                );
                for(int i=0; i<3; i++) {
                    digitalWrite(D8, HIGH);
                    delay(1000);
                    digitalWrite(D8, LOW);
                    delay(1000);
                }
            #endif
        }
    #endif
    digitalWrite(D8, LOW);
    // inclinometer = tftInclinometer_ST7735(&tftObj);
    #if useTFT
        inclinometer.tftSetup();
    #endif
    
    t1=millis();
}

int angle=0;
String filterToggle="on";
String totStr;

#if useAccel
void readAccelerometer() {
#if ACCEL__BOARD==0
    sensors_event_t event;

    accel.getEvent(&event);

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
#if ACCEL__BOARD==0
#elif ACCEL__BOARD==1
    oledInclinometer.printText(String(board_temp,1)+" C", 80, 38, 1, true, false);
#endif
#if useWiFi
    if(dynamic_true) {
        oledInclinometer.printText("192.168.1.118", 5, 51, 1, false, false);
        oledInclinometer.printText(String(localUdpPort), 95, 51, 1, false, false);
    }
#endif

    oledInclinometer.update(A_out.x, A_out.y, A_out.z, false);
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

#if useWiFi 
    int packetSize = 0;
    if(dynamic_true) packetSize = Udp.parsePacket();
    if(dynamic_true && packetSize) {//receive incoming UDPpackets
        digitalWrite(D8, HIGH);
        // if(digitalRead(0)==HIGH) filterToggle = "off;";
        // else filterToggle = "on ;";
        int len = Udp.read(incomingPacket,255);
        if(len>0) {incomingPacket[len]=0;}

        String resultStr = "{";
#if useAccel
        resultStr += A_out.exprt();
#if ACCEL__BOARD==0
#elif ACCEL__BOARD==1
        resultStr += ";" + G_out.exprt();
#endif
#endif

        resultStr += "}";//+filterToggle;//if"off":toggletiltfilteroff
        resultStr +=
            String(digitalRead(0))  +
            String(digitalRead(D7)) +
            ";";

        int newPackLen = resultStr.length()+1;
        char newReplyPack[newPackLen];
        resultStr.toCharArray(newReplyPack,newPackLen);

        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(newReplyPack);
        Udp.endPacket();
        digitalWrite(D8, LOW);
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

