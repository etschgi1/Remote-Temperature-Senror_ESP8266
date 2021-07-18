#include <Arduino.h>
#include "DHT.h"
#include <float.h>
#include <secrets.hpp>
//MQTT
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//defines DHT:
#define DHTPIN 5 // Digital pin connected to the DHT sensor (//!in fact D1 on board!!!)

#define DHTTYPE DHT11 // Change for other Sensors DHT11 and DHT 21 should be supported

#define BROADCAST_CHANNEL "/etschgi1/temp/3"
DHT dht(DHTPIN, DHTTYPE);
//setup MQTT: ----
// const char *SSID = ""; //! Better specified in header or compiled file
// const char *PSK = "";
const char *MQTT_BROKER = "192.168.68.143"; //IP Raspberry PI

WiFiClient espClient;
PubSubClient client(espClient);
//end -----
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(SSID);

  WiFi.begin(SSID, PSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Reconnecting...");
    if (!client.connect("ESP8266Client"))
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}
void setup()
{
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  setup_wifi();
  client.setServer(MQTT_BROKER, 1883);
  dht.begin();
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  //MQTT publish
  char msg[15];
  snprintf(msg, 15, "T=%.1f,H=%.0f", t, h);
  client.publish(BROADCAST_CHANNEL, msg);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  // Wait a few seconds between measurements.
  delay(5000);
}