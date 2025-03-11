#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include<Wire.h>
#include <SPI.h>

Adafruit_BME280 bme_i2c;
Adafruit_BME280 bme_spi (5,23, 19, 18);
float temperature = 0;
float t1, h1, p1 = 0;
float humidity = 0;
float pressure = 0;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  delay(1000);
  if (!bme_spi.begin()){
    Serial.println("SPI Sensor not found");
    while(1);
  }
  // if (!bme_i2c.begin()){
  //   Serial.println("I2C Sensor not found");
  //   while(1);
  // }
  pinMode(LED_BUILTIN, OUTPUT);
   
  
}

void loop() {
  // put your main code here, to run repeatedly:
  temperature = bme_spi.readTemperature();
  Serial.println(temperature);
  humidity = bme_spi.readHumidity();
  Serial.println(humidity);
  pressure = bme_spi.readPressure();
  Serial.println(pressure);
  //delay(60000);

  //.....................................I2C devices Check....................................
  byte error, address;
  int nDevices;
  Serial.println("Scanning for i2c devices...");
  nDevices = 0;
  for (address = 1; address < 127; address ++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address<16){
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error == 4){
      Serial.print("Unknown error at address 0x");
      if (address<16){
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0){
    Serial.println("No i2c devices found\n");
  }
  else{
    Serial.println("done\n");
  }
  delay(100000);
  
}

