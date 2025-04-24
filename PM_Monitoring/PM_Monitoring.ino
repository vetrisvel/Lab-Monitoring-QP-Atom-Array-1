#include <WiFiMulti.h>
#include <WiFi.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <Wire.h>
#include <RoomWeather.h>

// Fields to fill:
// SENSOR_NAME, WIFI_SSID/WIFI_PASSWORD, INFLUXDB_TOKEN, INFLUXDB_ORG

// Name the sensor
#define SENSOR_NAME ""    // e.g. PM_Node_1

// Define InfluxDB credentials and data location
#define WIFI_SSID ""      // Network Name 
#define WIFI_PASSWORD ""  // Network Password (if needed)
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com" // InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_TOKEN ""   // InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_ORG ""     // InfluxDB v2 organization id, e.g. qatomarray1 (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_BUCKET "Temperature_Humidity" // InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define TZ_INFO "EST-5"     // Time zone          

// Name of Sensor
Point sensor("SENSOR_NAME");
WiFiMulti wifiMulti;

// Micrograms per cubic meter
float PM10Env;
float PM25Env;
float PM100Env;

// Counts per 0.1L air
int Count03;
int Count05;
int Count10;
int Count25;
int Count50;
int Count100;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
RoomWeather *rw;                                      // Intialize Room Weather to read PM sensor

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);                                // Set WiFi to station mode and disconnect from an AP if it was previously connected
  wifiMulti.addAP(WIFI_SSID);                         // If a password is needed: wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");                 // Connect to WiFi
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    Serial.print("*");
    delay(500);
  }
  Serial.println();
  delay(500);
  Serial.println("Connected to wifi!");
  delay(2000);

  timeSync("EST-5", "pool.ntp.org", "time.nis.gov");  // Accurate time is necessary for certificate validation and writing in batches
  
  if (client.validateConnection())                    // Check server connection
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } 
  else 
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  rw = new RoomWeather("PM_1");                       // Set up Room Weather

  rw->Detect();
  rw->Read();
  delay(1000);
}

void loop() {
  rw->Read();
  PM10Env = rw->Values->Pmsa003i.Pm10Env;
  PM25Env = rw->Values->Pmsa003i.Pm25Env;
  PM100Env = rw->Values->Pmsa003i.Pm100Env;

  int Count03 = rw->Values->Pmsa003i.Particles03um;   // Count of size >0.3um
  //int Count05 = rw->Values->Pmsa003i.Particles05um;
  int Count10 = rw->Values->Pmsa003i.Particles10um;
  int Count25 = rw->Values->Pmsa003i.Particles25um;
  //int Count50 = rw->Values->Pmsa003i.Particles50um;
  int Count100 = rw->Values->Pmsa003i.Particles100um; // Count of size >10.0um

  sensor.clearFields();                               // Clear fields for reusing the point. Tags will remain untouched
  sensor.addField("PM 1.0", PM10Env, 1);                              
  sensor.addField("PM 2.5", PM25Env, 1);       
  sensor.addField("PM 10.0", PM100Env, 1);

  sensor.addField("0.3 - 1.0 Count", Count03-Count10); 
  sensor.addField("0.3 - 2.5 Count", Count03-Count25); 
  sensor.addField("0.3 - 10.0 Count", Count03-Count100);

  if (wifiMulti.run() != WL_CONNECTED)                // Check WiFi connection and reconnect if needed
  {
    Serial.println("Wifi connection lost");
    delay(500);
  }

  if (client.validateConnection()){
    if (!client.writePoint(sensor))                   // Write data point
    {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  } else {
    Serial.println("InfluxDB client is not connected");
  }
  
  // Display readings on serial monitor
  Serial.print("PM1.0: ");
  Serial.println(PM10Env);
  Serial.print("PM2.5: ");
  Serial.println(PM25Env);
  Serial.print("PM10.0: ");
  Serial.println(PM100Env);
  
  Serial.print("0.3 - 1.0 Count: ");
  Serial.println(Count03-Count10);
  Serial.print("0.3 - 2.5 Count: ");
  Serial.println(Count03-Count25);
  Serial.print("0.3 - 10.0 Count: ");
  Serial.println(Count03-Count100);
  Serial.print("Total Count: ");
  Serial.println(Count03);

  delay(20000);                                       // Wait 20 seconds between writes
}