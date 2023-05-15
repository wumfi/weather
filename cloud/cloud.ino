#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>
#include <ArduinoOTA.h>

#define SIMON  // Comment this out for me

#ifdef SIMON
  #define NUM_LEDS 13
  #define DATA_PIN D4
  #define SSID "SimonWeatherCloud"
#else
  #define NUM_LEDS 10
  #define DATA_PIN D5
  #define SSID "WeatherCloud"
#endif

CRGB leds[NUM_LEDS];
int weatherstatus;
unsigned long previousMillis = 0;  // will store last time LED was updated

const long interval = 2; // Minutes between weather updates

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Alloff();
  WiFiManager wifiManager;
  wifiManager.setTimeout(120);
   
  if(!wifiManager.autoConnect(SSID)) {
    Serial.println("failed to connect and hit timeout");
    
    delay(3000);
    //reset and try again
    ESP.reset();
  }
  WiFi.softAP(SSID, "S36MUSFCFM");

  getURL("https://home.wumfi.com/weather/get_cond.php");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void getURL(String url) {
  Serial.print("LEDS: ");
  Serial.println(NUM_LEDS);

  const int httpPort = 80;
  const int httpsPort = 443;

  WiFiClientSecure client;
  client.setInsecure();
  client.connect(url, httpsPort);

  HTTPClient http;
  
  http.begin(client, url);

  int httpCode = http.GET();
  weatherstatus = http.getString().toInt();
  http.end();
  Serial.println(url+" - "+weatherstatus);

  switch(weatherstatus) {
    case 1: // Thunder
      Thunder();
      break;
    case 2: // Drizzle
      Rain();
      break;
    case 3: // Rain
      Rain();
      break;
    case 4: // Snow
      Snow();
      break;
    case 5: // Misty/fog
      Cloudy(4);
      break;
    case 6: // Clear day
      ClearDay();
      break;
    case 7: // Clear night
      ClearNight();
      break;
    case 801: // Few Clouds
      Cloudy(1);
      break;
    case 802: // Scattered Clouds
      Cloudy(2);
      break;
    case 803: // Broken Clouds
      Cloudy(3);
      break;
    case 804: // Overcast
      Cloudy(4);
      break;
    case 9: // Demo
      Serial.println("Demo");
      Thunder();
      Alloff();
      Rain();
      Alloff();
      Snow();
      Alloff();
      Cloudy(1);
      delay(5000);
      Alloff();
      Cloudy(2);
      delay(5000);
      Alloff();
      Cloudy(3);
      delay(5000);
      Alloff();
      Cloudy(4);
      delay(5000);
      Alloff();
      ClearDay();
      delay(5000);
      Alloff();
      ClearNight();
      delay(5000);
      Alloff();
      break;      
    case 10: // Nightime - all lights off
      Alloff();
      break;
  }
}

void Alloff() {
  int ledctr;
  int fadectr;
  for(fadectr=255;fadectr>=0;fadectr--) {
    FastLED.setBrightness(fadectr);
    FastLED.show();
    delay(2);
  }

  // Set all LEDs off (I think this saves power, rather than just leaving brightness at 0)
  for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
    leds[ledctr]=CRGB::Black;
    FastLED.show();
  }
}

void Thunder() {
  int ledctr;
  int flickerctr;

  Serial.println("Thunder");
  // Seed random num gen here, rather than on each call of GetRND
  randomSeed(analogRead(0));
  
  for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
    setLED(ledctr,75,0,240);
  }
  
  FadeUp();

  for(flickerctr=0;flickerctr<=10;flickerctr++) {
    Flicker(9,4,255,255,255,50,50,random(500,2000));
  }
}

void Rain() {
  int ledctr;
  int flickerctr;

  Serial.println("Rain");

  // Seed random num gen here, rather than on each call of GetRND
  randomSeed(analogRead(0));
  
  for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
    setLED(ledctr,0,0,255);
  }
  
  FadeUp();

  for(flickerctr=0;flickerctr<=100;flickerctr++) {
    Flicker(5,1,0,0,0,50,150,0);
  }
}

void Snow() {
  int ledctr;
  int flickerctr;

  Serial.println("Snow");

  // Seed random num gen here, rather than on each call of GetRND
  randomSeed(analogRead(0));
  
  for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
    setLED(ledctr,128,128,128);
  }
  
  FadeUp();

  for(flickerctr=0;flickerctr<=100;flickerctr++) {
    Flicker(5,1,255,255,255,150,250,0);
  }
}

void Cloudy(int CloudLevel) {
  int ledctr;
  int fadectr;

  //Serial.print("Cloudlevel: ");
  //Serial.println(CloudLevel);
  switch(CloudLevel) {
    case 1:
      for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
        setLED(ledctr,255,255,0);
      }
      for(ledctr=0;ledctr<5;ledctr++) {
        setLED(ledctr,248,246,168);
      }
      break;
    case 2:
      for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
          setLED(ledctr,248,246,168);
      }
      for(ledctr=6;ledctr<NUM_LEDS-1;ledctr++) {
          setLED(ledctr,255,255,0);
      }
     break;
    case 3:
      for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
        setLED(ledctr,248,246,168);
      }
      if(SSID=="SimonWeatherCloud") {
        setLED(8,255,255,0);
        setLED(10,255,255,0);
        setLED(12,255,255,0);
      } else {
        setLED(5,255,255,0);
        setLED(7,255,255,0);
        setLED(9,255,255,0);
      }
      break;
    case 4:
      for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
        setLED(ledctr,248,246,168);
      }
      break;
  }
  FadeUp();
}

void ClearDay() {
  int ledctr;
  int fadectr;

  Serial.println("Clear day");

  for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
    setLED(ledctr,255,255,0);
  }
  FadeUp();
}

void ClearNight() {
  int ledctr;
  int fadectr;

  Serial.println("Clear night");

  for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
    setLED(ledctr,108,20,184);
  }
  FadeUp();
}

int GetRND(int retvalue) {
  return random(0,retvalue);
}

void FadeUp() {
  int fadectr;
  if (FastLED.getBrightness()!=255) {
    for(fadectr=0;fadectr<256;fadectr++) {
      FastLED.setBrightness(fadectr);
      FastLED.show();
      delay(2);
    }
  }
  FastLED.show();
}

void Flicker(int lednum, int howmany, int flickercolR, int flickercolG, int flickercolB, int speedL, int speedH, int afterDelay) {
int currcolR=leds[lednum].r;
int currcolG=leds[lednum].g;
int currcolB=leds[lednum].b;
int howmanyctr;
int ledctr;

  for(howmanyctr=0;howmanyctr<=howmany;howmanyctr++) {
    setLED(random(lednum),flickercolR,flickercolG,flickercolB);
  }
  FastLED.show();
  delay(random(speedL,speedH));
  for(ledctr=0;ledctr<NUM_LEDS;ledctr++) {
    setLED(ledctr,currcolR,currcolG,currcolB);
  }
  FastLED.show();
  delay(afterDelay);
}

void setLED(int lednum, int r, int g, int b) {
  leds[lednum].r=r;
  leds[lednum].g=g;
  leds[lednum].b=b;
}

void loop() {
  ArduinoOTA.handle();
  unsigned long lastMillis = millis();
  if (millis() - lastMillis >= 2*60*1000UL) 
  {
    lastMillis = millis();  //get ready for the next iteration
    getURL("https://home.wumfi.com/weather/get_cond.php");
  }
}
