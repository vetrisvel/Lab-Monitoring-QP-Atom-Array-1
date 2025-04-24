#include <WiFiMulti.h>
#include <WiFi.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <Wire.h>
#include "Adafruit_HTU31D.h"

// Fields to fill:
// SENSOR_NAME, WIFI_SSID/WIFI_PASSWORD, INFLUXDB_ORG, INFLUXDB_TOKEN

// Name the sensor
#define SENSOR_NAME ""    // e.g. TH_Node_1

// Define InfluxDB credentials and data location
#define WIFI_SSID ""      // Wifi Name 
#define WIFI_PASSWORD ""  // Wifi Password (if needed)
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com" // InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_ORG ""     // InfluxDB v2 organization id, e.g. qatomarray1 (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_TOKEN ""   // InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_BUCKET "Temperature_Humidity" // InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define TZ_INFO "EST-5"     // Time zone

Point sensor(SENSOR_NAME);
WiFiMulti wifiMulti;
Adafruit_HTU31D htu = Adafruit_HTU31D();
float temp;
float humidity;
uint16_t loopCounter = 0;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);                                        // Set WiFi to station mode and disconnect from an AP if it was previously connected
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);                  // Set the WiFI

  Serial.print("Connecting to wifi");                         // Connect to WiFi
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    Serial.print("*");
    delay(500);
  }
  Serial.println();
  delay(500);
  Serial.println("Connected to wifi!");
  delay(2000);

  timeSync("EST-5", "pool.ntp.org", "time.nis.gov");          // Accurate time is necessary for certificate validation and writing in batches
  
  if (client.validateConnection())                            // Check server connection
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } 
  else 
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  // connect to the HTU31D sensor
  if (!htu.begin(0x40)) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
  // make sure the heater is off
  htu.enableHeater(false);
}

void loop() {
  sensors_event_t humidity_obj, temp_obj;
  htu.getEvent(&humidity_obj, &temp_obj);                     // Populate temp and humidity objects with fresh data
  temp = temp_obj.temperature;
  humidity = humidity_obj.relative_humidity;

  sensor.clearFields();                                       // Clear fields for reusing the point. Tags will remain untouched
  sensor.addField("temperature", temp, 3);                    // Store measured value into point
  sensor.addField("humidity", humidity, 3);                   // Store measured value into point

  if (wifiMulti.run() != WL_CONNECTED)                        // Check WiFi connection and reconnect if needed
  {
    Serial.println("Wifi connection lost");
    delay(500);
  }

  if (client.validateConnection()){
    if (!client.writePoint(sensor))                           // Write data point to InfluxDB
    {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  } else {
    Serial.println("InfluxDB client is not connected");
  }
  Serial.print("Temp (Degrees C): ");                         // Display readings on serial monitor
  Serial.println(temp);
  Serial.print("Humidity (rH): ");
  Serial.println(humidity);

  loopCounter++;
  if (loopCounter >= 8640){                                   // Turn on the heater for a minute approximately once a day
    if (! htu.enableHeater(true)){
      Serial.println("Turning heater on failed");
    } else {
      Serial.println("Turning heater on");
    }
    delay(60000);                                             // Turn on for 60 seconds
    if (! htu.enableHeater(false)){
      Serial.println("Turning heater off failed");
    } else {
      Serial.println("Turning heater off");
    }
    delay(20000);                                             // Wait 20 seconds after turning off
    Serial.println("Resuming data collection");
    loopCounter = 0;
  }
  delay(10000);                                               // Wait 10 seconds between writes
}
