
#include <SSD1306_inclinometer.hpp>

float oledInclinometer_SSD1306::toRadians(float degrees) { return float(degrees*M_PI)/180; }
float oledInclinometer_SSD1306::toDegrees(float radians) { return float(radians*180)/M_PI; }


oledInclinometer_SSD1306::oledInclinometer_SSD1306():
display(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, &Wire, OLED_RESET)
{
    displayPtr = &display;
    pointerInitialized = false;
}
oledInclinometer_SSD1306::oledInclinometer_SSD1306(Adafruit_SSD1306 *oled_pointer) {
    // displayPtr = oled_pointer;
    // pointerInitialized = true;
}

void oledInclinometer_SSD1306::setup() {
    Wire.begin(14, 12);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    display.display();
    delay(2000);
    display.clearDisplay();

    display.drawPixel(10, 10, SSD1306_WHITE);
    display.display();
    delay(2000);
}

void oledInclinometer_SSD1306::drawAccel() {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(accelPos[0][0], accelPos[0][1]);
    display.print("G_x: "+String(accel[0],2));

    display.setCursor(accelPos[1][0], accelPos[1][1]);
    display.print("G_y: "+String(accel[1],2));

    display.setCursor(accelPos[2][0], accelPos[2][1]);
    display.print("G_z: "+String(accel[2],2));
}
void oledInclinometer_SSD1306::drawRoll() {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(rollPos[0], rollPos[1]);
    display.print("Roll: "+String(orient[0],1));
}
void oledInclinometer_SSD1306::drawPitch() {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(pitchPos[0], pitchPos[1]);
    display.print("Pitch:"+String(orient[1],1));
}

void oledInclinometer_SSD1306::solveOrients() {
    orient[0]   = toDegrees( atan(accel[1] / sqrt(pow(accel[0],2)+pow(accel[2],2))) ); //degrees
    orient[1]   = toDegrees( atan(-1 * accel[0] / sqrt(pow(accel[1],2)+pow(accel[2],2))) ); //degrees
    if(accel[2]<0) {
        orient[1]=180-orient[1];
    }
}

void oledInclinometer_SSD1306::setAccel(float x, float y, float z) {
    accel[0] = x;
    accel[1] = y;
    accel[2] = z;
}

void oledInclinometer_SSD1306::update() {
    solveOrients();

    display.clearDisplay();
    drawAccel();
    drawRoll();
    drawPitch();
    display.display();
}
void oledInclinometer_SSD1306::update(float xAccel, float yAccel, float zAccel) {
    setAccel(xAccel, yAccel, zAccel);
    update();
}
