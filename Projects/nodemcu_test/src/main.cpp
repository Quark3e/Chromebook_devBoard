#include <Arduino.h>
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
// #include <UnixTime.h>

// #include <rawBMP_arrays.h>
#include <weatherBitmaps.h>
#include <symbolBitmaps.h>
#include <weatherIcon_print.h>


#define TFT_RST		D4		// TFT RST	pin is connected to NodeMCU pin D4 (GPIO02)
#define TFT_CS		D3		// TFT CS	pin is connected to NodeMCU pin D3 (GPIO00)
#define TFT_DC		D2		// TFT DC	pin is connected to NodeMCU pin D2 (GPIO04)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
printOWM_icons owmIcon = printOWM_icons(&tft);

const char* ssid = "Telia-47118D";
const char* password = "0C94C28B5D";
String openWeatherMapApiKey = "7c28bdf205051aa2ce610d67d648c55c";
String city = "Södertälje"; //NOTE: Might need to ö. check later
String countryCode = "SE";
String latitude = "59.20";
String longitude = "17.62";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

const long utcOffsetInSeconds = 7200;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
WiFiClient client;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String httpGETRequest(const char* serverName);



void setup() {
	pinMode(D8, OUTPUT);
	digitalWrite(D8, LOW);
	Serial.begin(115200);

	WiFi.begin(ssid, password);
	Serial.println("Connecting");
	int blinkCount=0;
	while(WiFi.status() != WL_CONNECTED) {
		if(blinkCount==4) digitalWrite(D8, HIGH);
		delay(500);
		Serial.print(".");
		if(blinkCount==4) {
			digitalWrite(D8, LOW);
			blinkCount=0;
		}
		blinkCount++;
	}
	for(int i=0; i<3; i++) {
		digitalWrite(D8, HIGH);
		delay(10);
		digitalWrite(D8, LOW);
		delay(10);
	}
	digitalWrite(D8, HIGH);

	Serial.println("");
	Serial.print("Connected to WiFi network with IP Address: ");
	Serial.println(WiFi.localIP());

	Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
	timeClient.begin();
	tft.initR(INITR_BLACKTAB);
	uint16_t time = millis();
	tft.fillScreen(ST7735_BLACK);
	time = millis() - time;

	delay(500);

	tft.fillScreen(ST7735_BLACK);
	//layout
	tft.drawFastHLine(0, 0, tft.width(), ST7735_WHITE);
	tft.drawFastHLine(1, 159, tft.width(), ST7735_WHITE);
	tft.drawFastVLine(0, 0, tft.height(), ST7735_WHITE);
	tft.drawFastVLine(127, 0, tft.height(), ST7735_WHITE);

	// tft.drawBitmap(0, 0, cloud, 45, 38, ST7735_YELLOW);
	// tft.drawBitmap(6, 5, cloud, 45, 38, ST7735_YELLOW);
	//delay(2000);
	tft.drawBitmap(90, 47, CLOCK, 30, 28, ST7735_WHITE);
	
	tft.drawBitmap(6, 75, humid, 22, 30, 0x07FF);
	tft.drawBitmap(95, 100, pressure, 30, 30, ST7735_RED);
	tft.drawBitmap(6, 131, windS, 40, 19, 0xAFE5);
	
	tft.setFont(&FreeSans9pt7b);
	tft.setTextColor(ST7735_YELLOW);
	tft.setCursor(50, 20);
	tft.setTextSize(1);
	tft.print(city);
	// tft.setFont(&FreeMono9pt7b);
	tft.setFont();
	tft.setTextSize(1);
	tft.setCursor(55, 31);
	tft.print(countryCode);

	tft.setCursor(100, 29);
	tft.print(" C");

	tft.setTextColor(0x07FF);
	tft.setFont(&FreeSans9pt7b);
	tft.setTextSize(1);
	tft.setCursor(74, 95);
	tft.print("%");
	tft.setFont();
	tft.setTextColor(0x07FF);
	tft.setCursor(95, 85);
	tft.setTextSize(1);
	tft.print("Humid");
	tft.setTextColor(0xAFE5);
	tft.setTextSize(1);
	tft.setCursor(107, 135);
	tft.print("m/s");
	tft.setTextColor(ST7735_RED);
	tft.setCursor(72, 108);
	tft.print(" hpa");
	configTime(0, 0, "0.se.pool.ntp.org");
	delay(1000);
}

int temp, pres, hum;
String wind;

void loop() {
	if((millis() - lastTime) > timerDelay) {
		if(WiFi.status()==WL_CONNECTED) {
			String serverPath = "http://api.openweathermap.org/data/2.5/weather?q="+city+","+countryCode+"&APPID="+openWeatherMapApiKey;

			jsonBuffer = httpGETRequest(serverPath.c_str());
			// Serial.println(jsonBuffer);
			JSONVar myObject = JSON.parse(jsonBuffer);
			if(JSON.typeof(myObject)=="undefined") {
				Serial.print("| Parsing input failed! ");
				return;
			}
			timeClient.update();
			String jsonString = JSON.stringify(myObject["main"]["temp"]);
			temp=jsonString.toInt();
			temp=(temp-273);
			jsonString = JSON.stringify(myObject["main"]["pressure"]);
			pres=jsonString.toInt();
			jsonString = JSON.stringify(myObject["main"]["humidity"]);
			hum=jsonString.toInt();
			jsonString = JSON.stringify(myObject["wind"]["speed"]);
			wind=jsonString;//.toInt();
			Serial.println();
			Serial.print("JSON object = ");
			Serial.println(myObject);
			Serial.print("Temperature: ");
			Serial.println(temp);
			Serial.print("Pressure: ");
			Serial.println(pres);
			Serial.print("Humidity: ");
			Serial.println(hum);
			Serial.print("Wind Speed: ");
			Serial.println(wind);

			tft.fillRect(14, 45, 68, 28, ST7735_BLACK);
			tft.fillRect(24, 70, 55, 10, ST7735_BLACK);
			tft.fillRect(73, 24, 25, 21, ST7735_BLACK);
			tft.fillRect(40, 80, 26, 21, ST7735_BLACK);
			tft.fillRect(20, 105, 55, 18, ST7735_BLACK);
			tft.fillRect(50, 130, 45, 19, ST7735_BLACK);

			owmIcon.printIcon(myObject["weather"][0]["icon"]);
			// owmIcon.printIcon("10d");

			tft.setTextColor(ST7735_YELLOW);
			tft.setFont(&FreeMonoBold12pt7b);
			tft.setTextSize(1);
			tft.setCursor(72, 42);
			if(temp!=-273) tft.print(temp);
			// tft.setFont();
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(13, 62);
			// tft.setTextSize(1);
			int minutes = timeClient.getMinutes();
			tft.print(timeClient.getHours());
			tft.print(":");
			if(minutes<10) {
				tft.print(0);
				tft.print(minutes);
			}
			else tft.print(timeClient.getMinutes());
			tft.setFont();
			tft.setTextSize(1);
			tft.setCursor(26, 70);
			tft.print(daysOfTheWeek[timeClient.getDay()]);
			tft.setTextColor(0x07FF);
			tft.setFont(&FreeMono12pt7b);
			tft.setTextSize(1);
			tft.setCursor(40, 97);
			tft.print(hum);
			tft.setTextColor(ST7735_RED);
			tft.setCursor(18, 121);
			tft.print(pres);

			tft.setTextColor(0xAFE5);
			tft.setCursor(48, 148);
			tft.print(wind);
		}
		else {
			Serial.println("WiFi Disconnected");
		}
		lastTime = millis();
	}
}

String httpGETRequest(const char* serverName) {
	HTTPClient http;
	http.begin(client, serverName);
	int httpResponseCode = http.GET();
	String payload = "{}";

	if(httpResponseCode>0) {
		Serial.print("HTTP Response code: ");
		Serial.println(httpResponseCode);
		payload = http.getString();
	}
	else {
		Serial.print("Error code: ");
		Serial.println(httpResponseCode);
	}
	http.end();

	return payload;
}


