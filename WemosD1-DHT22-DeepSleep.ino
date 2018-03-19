#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define DHTPIN            D4
#define DHTTYPE           DHT22     // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);

/***********************************
  Load my WIFI settings :)
  File includes following lines:
    const char* ssid     = "SSIDOFYOURWIFI";
    const char* password = "YOURPASSWORD";
  Remove the include and insert the lines above with your credentials
************************************/
#include "mywifi.h"

const char* host = "ESPDHT22-01";
const char* ioBrokerHost="rpi";

// How long should we go to sleep 
const int sleepMinutes=10;

// I use static IPs for this
IPAddress ip(192, 168, 178, 152);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 178, 1);
IPAddress dns(192, 168, 178, 1);

float value ;
int rvalue;
uint32_t delayMS;

struct HumTemp {
  float hum;
  float temp;
};

void setup()
{

  Serial.begin(115200);

  Serial.print(host);
  Serial.println(" is starting");

  connectWifi();
  setupOTA();

  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Setup complete");
  dht.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void connectWifi() {
  WiFi.config(ip, gateway, subnet, dns);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println();
    Serial.println("Fail connecting");
    delay(5000);
    ESP.restart();
  }
  if (!MDNS.begin(host)) {
    Serial.println("Could not register DNS");
  }

}

void setupOTA() {

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  // Commented out because i decided to use an update-server because of deepsleep
  //  ArduinoOTA.begin();

}



HumTemp getSingleHumTemp() {
  HumTemp h={};
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
    return {0, 0};
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
    h.temp = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
    return {0, 0};
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
    h.hum = event.relative_humidity;
  }
  return h;
}


HumTemp getAvgHumTemp() {
  HumTemp h={} ;
  int o = 0;
  for (int i = 0; i < 4; i++) {
    HumTemp p = getSingleHumTemp();
    if (p.temp > 0 && p.hum > 0) {
      h.hum += p.hum;
      h.temp += p.temp;
      o++;
    }
  }
  h.hum /= o;
  h.temp /= o;
  return h;
}


void loop()
{
  // Try to get Updates from my Updateserver (IOBroker)
  ESPhttpUpdate.update(ioBrokerHost, 8082, "/vis.0/firmwares/D1_DH22.ino.d1_mini.bin");

  HTTPClient http;
  ArduinoOTA.handle();

  HumTemp humtemp = getAvgHumTemp();

  // Write this Info to my Raspberry IOBroker
  String url = "http://"+ioBrokerHost+":8087/setBulk/javascript.0.Dry.Mini_0?Humidity=";
  url += humtemp.hum;
  url += "&Temperature=";
  url += humtemp.temp;
  Serial.println(url);
  http.begin(url); //HTTP

  int httpCode = http.GET();
  Serial.println(httpCode);
  String payload = http.getString();
  Serial.println(payload);
  if (httpCode > 0) {
    // If ok, then... ok... thx...
  }
  http.end();
  delay(10000);

// Go to DeepSleep for 10 Minutes
  ESP.deepSleep(sleepMinutes *60 * 1000000);
}
