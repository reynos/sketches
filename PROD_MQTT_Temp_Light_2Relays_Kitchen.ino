/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 5     // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


#define LIGHTPIN 4     // what digital pin the light sensor is connected to

// Update these with values suitable for your network.

const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "xx.xx.xx.xx";
char message_buff[100];

WiFiClient espClient;
PubSubClient client(espClient);
// Force loop on first run
long lastMsg = millis() - 600000;

char msg[6];
char msg2[6];
char lightstatemsg[1];

DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
// setup relay pins for output
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  dht.begin();
  
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String stopic = String(topic);
  int i = 0;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message_buff[i] = payload[i];
  }
  Serial.println();

   String msgString = String(message_buff);
    Serial.print("stopic is now:");
    Serial.println(stopic);
    Serial.print("Message String is now:");
    Serial.println(msgString);

  if (stopic == "/water/one") {
    Serial.println("ok1");   
      if(msgString == "1") {
      digitalWrite(12, LOW);   // Turn the LED on (Note that LOW is the voltage level
      Serial.println("Watering 1 ON");
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
      } else if (msgString == "0") {
          digitalWrite(12, HIGH);  // Turn the LED off by making the voltage HIGH
            Serial.println("Watering 1 OFF");
            }
  }
  if (stopic == "/water/two") {
    Serial.println("ok2");   
      if(msgString == "1") {
      digitalWrite(13, LOW);   // Turn the LED on (Note that LOW is the voltage level
      Serial.println("Watering 2 ON");
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
      } else if (msgString == "0") {
          digitalWrite(13, HIGH);  // Turn the LED off by making the voltage HIGH
            Serial.println("Watering 2 OFF");
            }
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", "reynolds", "avoca24")) {
      Serial.print("connected, state = ");
      Serial.println(client.state());
      client.subscribe("/water/one");
      client.subscribe("/water/two");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
// turn relays off first thing!
  digitalWrite(12, HIGH);  // Turn the RELAY off by making the voltage HIGH
  digitalWrite(13, HIGH);  // Turn the RELAY off by making the voltage HIGH

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
    if (!client.connected()) {
        reconnect();
     }

}

void loop() {


  long now = millis();
  if (now - lastMsg > 600000) {
    lastMsg = now;

    if (!client.connected()) {
          reconnect();
   }

    // Read light staus and publish
    int LightState = digitalRead(LIGHTPIN);
    dtostrf(LightState, 1, 0, lightstatemsg);
    client.publish("/Light/Kitchen", lightstatemsg);
    Serial.print("Light staus is: ");
    Serial.println(LightState);

    Serial.println("Checking temp and humidity...");
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    dtostrf(t, 4, 1, msg);
    dtostrf(h, 4, 1, msg2);
    Serial.print("Current temp is: ");
    Serial.println(msg);
    Serial.print("Current humidity is: ");
    Serial.println(msg2);

    client.publish("/Temperature/Kitchen", msg);
    client.publish("/Temperature/Humidity/Kitchen", msg2);
    Serial.println("Published!");
    Serial.println("Won't check temp again for 10mins...");
    Serial.println();
  }
    client.loop();
}
