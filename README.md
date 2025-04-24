We use the [HTU31](https://www.adafruit.com/product/4832) temperature and humidity sensor, and the [PMSA003I](https://www.adafruit.com/product/4632) particulate matter sensor from Adafruit. To read data from the sensors, we use I2C connections via Adafruit [STEMMA QT](https://www.adafruit.com/product/4210) 4-pin cables to interface with the [SparkFun Thing Plus](https://www.sparkfun.com/sparkfun-thing-plus-esp32-wroom-u-fl.html), which has an ESP32 microcontroller and sends data over the internet via a Taoglas [FXP70](https://www.taoglas.com/product/freedom-fxp70-2-4ghz-flex-pcb-antenna/) antenna to an InfluxDB cloud database.

## HTU31D Temperature and Humidity Sensor

1. Download and Install Arduino IDE
2. Go to boards manager and install "esp32" by Espressif Systems
3. Go to library manager
	1. Install "ESP8266 Influxdb"
	2. Install "Adafruit HTU31D Library"
#### Live monitoring:

1. Clone this repo: [https://github.com/vetrisvel/Lab-Monitoring-QP-Atom-Array-1](https://github.com/vetrisvel/Lab-Monitoring-QP-Atom-Array-1)
2. Go to Temp_Humidity_Monitoring and open Temp_Humidity_Monitoring.ino
3. Fill in the fields SENSOR_NAME (e.g. TH_Node_X), WIFI_SSID/WIFI_PASSWORD, INFLUXDB_TOKEN, and INFLUXDB_ORG
4. Select the SparkFun ESP32 Thing Plus board
5. Upload    

## PMSA003I Particulate Matter Sensor

1. Download and Install Arduino IDE
2. Go to boards manager and install "esp32" by Espressif Systems
3. Go to library manager
	1. Install "ESP8266 Influxdb"
	2. Install  "RoomWeather" by Gobbo Lab
#### Live monitoring:

1. Clone this repo: [https://github.com/vetrisvel/Lab-Monitoring-QP-Atom-Array-1](https://github.com/vetrisvel/Lab-Monitoring-QP-Atom-Array-1)
2. Go to PM_Monitoring and open PM_Monitoring.ino
3. Fill in the fields SENSOR_NAME (e.g. PM_Node_X), WIFI_SSID/WIFI_PASSWORD, INFLUXDB_TOKEN, and INFLUXDB_ORG
4. Select the SparkFun ESP32 Thing Plus board
5. Upload
