#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "your _ssid";
const char* password = "your _password";

//your MQTT broker details
const char* mqtt_server = "your mqtt server";
const int mqtt_port = 8883;
const char* mqtt_topic = "topic ";
//optional
const char* mqtt_user = "your user";
const char* mqtt_pass = "";


WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// function for print msg coming from topics that you are subscibe at it 
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);
}
//function for connecting to wifi
void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println(" connected!");
}

// Connect to MQTT broker 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected.");
      //to subscribe to topic
      client.subscribe("your/topic"); 
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  wifiClient.setInsecure(); 
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
 }

void loop() {
  if (!client.connected()) {
    reconnect();
  }
    client.loop();
//for publishing msg to topic
client.publish(mqtt_topic,"what you want to publish");
delay(5000);}
  

  
  

