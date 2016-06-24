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

// Update these with values suitable for your network.

const char* ssid = "nuttynut";
const char* password = "islandsinthestream";
const char* mqtt_server = "10.0.26.45";
char message_buff[100];
//int s;

WiFiClient espClient;
PubSubClient client(espClient);
// Force loop on first run
long lastMsg = millis() - 600000;
char msg[6];
char msg2[6];

void setup_wifi() {
// setup relay pins for output
  pinMode(12, OUTPUT);
//  pinMode(03, OUTPUT);
//  pinMode(04, OUTPUT);
//  pinMode(05, OUTPUT);
  
//  delay(10);
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
  //message_buff[i] = '\0';
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
      Serial.println("Relay 1 ON");
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
      } else if (msgString == "0") {
          digitalWrite(12, HIGH);  // Turn the LED off by making the voltage HIGH
            Serial.println("Relay 1 OFF");
            }
  }
  /*if (stopic == "/relays/two") {
    Serial.println("ok2");   
      if(msgString == "1") {
      digitalWrite(03, LOW);   // Turn the LED on (Note that LOW is the voltage level
      Serial.println("Relay 2 ON");
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
      } else if (msgString == "0") {
          digitalWrite(03, HIGH);  // Turn the LED off by making the voltage HIGH
            Serial.println("Relay 2 OFF");
            }
  }
  if (stopic == "/relays/three") {
    Serial.println("ok1");   
      if(msgString == "1") {
      digitalWrite(04, LOW);   // Turn the LED on (Note that LOW is the voltage level
      Serial.println("Relay 3 ON");
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
      } else if (msgString == "0") {
          digitalWrite(04, HIGH);  // Turn the LED off by making the voltage HIGH
            Serial.println("Relay 3 OFF");
            }
  }
  if (stopic == "/relays/four") {
    Serial.println("ok1");   
      if(msgString == "1") {
      digitalWrite(05, LOW);   // Turn the LED on (Note that LOW is the voltage level
      Serial.println("Relay 4 ON");
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
      } else if (msgString == "0") {
          digitalWrite(05, HIGH);  // Turn the LED off by making the voltage HIGH
            Serial.println("Relay 4 OFF");
            }
  }
*/
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", "reynolds", "avoca24")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("/water/one");
/*      client.subscribe("/relays/two");
      client.subscribe("/relays/three");
      client.subscribe("/relays/four");
*/
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
//  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
    if (!client.connected()) {
        reconnect();
     }
    digitalWrite(12, HIGH);  // Turn the RELAY off by making the voltage HIGH
/*    digitalWrite(03, HIGH);  // Turn the RELAY off by making the voltage HIGH
    digitalWrite(04, HIGH);  // Turn the RELAY off by making the voltage HIGH
    digitalWrite(05, HIGH);  // Turn the RELAY off by making the voltage HIGH
*/
}

void loop() {
    if (!client.connected()) {
        reconnect();
     }

  client.loop();
  

}
