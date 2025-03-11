#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* wifi_ssid = "jtr.";
const char* wifi_password = "G11cliffside!";
const char* mqtt_server = "10.0.0.186";

WiFiClient esp_client;
PubSubClient mqtt_client (esp_client);

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

}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  wifi_setup();
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(callback);
}



void loop() {
 if (!mqtt_client.connected()) {
  mqtt_reconnect();
 }
 
 mqtt_client.loop();
 delay(1000);
 Serial.println(mqtt_client.state());
 mqtt_client.publish ("esp_out", "hello from esp32");
 digitalWrite(LED_BUILTIN, HIGH);
 delay(5000);
}