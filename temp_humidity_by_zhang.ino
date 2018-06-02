// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************ Adafruit IO Configuration *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    " "         //你的ad账号
#define IO_KEY         " "         //你的ad密码

/******************************* WIFI Configuration **************************************/

#define WIFI_SSID       " "         //你的wifi账号
#define WIFI_PASS       " "          //你的wifi密码

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/************************ Main Program Starts Here *******************************/
#include <ESP8266WiFi.h>
#include <AdafruitIO.h>
#include <Adafruit_MQTT.h>
#include <ArduinoHttpClient.h>

/************************ Example Starts Here *******************************/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// pin connected to DH22 data line
#define DATA_PIN 14

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// set up the 'temperature' and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *command = io.feed("command");
void setup() {
  pinMode(5,OUTPUT);                        //定义继电器控制口
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // initialize dht22
  dht.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = (event.temperature);
  float fahrenheit = (celsius * 1.8) + 32;

  Serial.print(": ");
  Serial.print(celsius);
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(celsius);

  dht.humidity().getEvent(&event);
  float hum = (event.relative_humidity);
  Serial.print("humidity: ");
  Serial.print(hum);
  Serial.println("%");

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);
  if(hum<64){                                 // 加湿器启动循环，因传感器会有延迟度数，可按实际情况自行优化加湿方式。
  digitalWrite(5,HIGH);
  delay(10000);
  digitalWrite(5,LOW);
  delay(10000);
  //Serial.print("on");
  command->save(1);
  }
  else{
  digitalWrite(5,LOW);
  //Serial.print("off");
  command->save(0);
  }

  // wait 10 seconds (5000 milliseconds == 5 seconds)
  delay(10000);

}
