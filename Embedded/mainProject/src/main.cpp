#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* wifi_ssid = "jtr.";
const char* wifi_password = "G11cliffside!";
const char* mqtt_server = "10.0.0.186";

WiFiClient esp_client;
PubSubClient mqtt_client (esp_client);

Adafruit_BME280 bme_i2c;
Adafruit_BME280 bme_spi (5,23, 19, 18);
long lastMsg;

void wifi_setup(){
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connection Established");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_reconnect(){
  //setting mqtt connection and looping until connection established
  while (!mqtt_client.connected()){
    Serial.print("Attempting MQTT Connection...");
    // creating random Client ID
    String clientId = "ESP32client - ";
    clientId += String(random(0xffff), HEX);
    //Attempting to connect
    if (mqtt_client.connect(clientId.c_str())) {
      Serial.println("Mqtt Connected");
      //Publish announcement once connected
      mqtt_client.publish ("esp_out", "hello from esp32");
      //... and resubscribe
      mqtt_client.subscribe("esp_in");
    }
    else{
      Serial.print("failed, rc = ");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      //waiting 5s before retrying
      delay(5000);
    }

  }
}

void callback (char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++){
    message += (char) payload[i];
  }
  Serial.println(topic);
  Serial.print(":");
  Serial.print(message);
  if (String (topic) == "esp_in"){
    if (message == "ON"){digitalWrite(LED_BUILTIN, HIGH);}
    else if (message == "OFF") {digitalWrite(LED_BUILTIN, LOW);}
    
  }

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  wifi_setup();
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(callback);
  if (!bme_spi.begin()){
    Serial.println("SPI Sensor not found");
    while(1);
  }
  if (!bme_i2c.begin(0x76)){
    Serial.println("I2C Sensor not found");
    while(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }
  mqtt_client.loop();
  StaticJsonDocument<80> doc;
  char output[80];

  long now = millis();

  if (now - lastMsg > 60000) {
    lastMsg = now;
    
    Serial.println(mqtt_client.state());
    float t1 = bme_i2c.readTemperature();
    float p1 = bme_i2c.readPressure()/100.0;
    float h1 = bme_i2c.readHumidity();
    doc["t1"] = t1;
    doc["p1"] = p1;
    doc["h1"] = h1;
    float t2 = bme_spi.readTemperature();
    float p2 = bme_spi.readPressure()/100.0;
    float h2 = bme_spi.readHumidity();
    doc["t2"] = t2;
    doc["p2"] = p2;
    doc["h2"] = h2;
    

    serializeJson(doc, output);
    Serial.println(output);
    mqtt_client.publish ("esp_out", output);
    mqtt_client.publish("esp_out", "Sent sensor data");
  }

   
}

