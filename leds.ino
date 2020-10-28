#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your ssid";           // SSID of your home WiFi
const char* password = "your password";            // password of your home WiFi
int GreenLED = 15;
int YellowLED = 13;
int RedLED = 12;
int flashing = 0;


const char* mqtt_server = "your pi IP address";
const char* clientID = "lights";

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {

  if ((char)payload[0] == '0'){
    digitalWrite(GreenLED, HIGH);
    digitalWrite(YellowLED, LOW);
    digitalWrite(RedLED, LOW);
    flashing = 0;
  }
  else if ((char)payload[0] == '1') {
    digitalWrite(GreenLED, LOW);
    digitalWrite(YellowLED, HIGH);
    digitalWrite(RedLED, LOW);
    flashing = 0;
  }
  else if ((char)payload[0] == '2') {
    digitalWrite(GreenLED, LOW);
    digitalWrite(YellowLED, LOW);
    digitalWrite(RedLED, HIGH);
    flashing = 0;
  }
  else if ((char)payload[0] == '3') {
    flashing = 1;
    digitalWrite(GreenLED, LOW);
    digitalWrite(YellowLED, LOW);
    digitalWrite(RedLED, LOW);
  }
  if (flashing == 1) {
    digitalWrite(RedLED, LOW);
    delay(300);
    digitalWrite(RedLED, HIGH);
    delay(300);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(clientID)) {
      Serial.println("connected");
      client.subscribe("distance_status");
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
  Serial.begin(9600);              
  WiFi.begin(ssid, password);             // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 Serial.println("Connected to wifi");
  Serial.print("IP: ");     
  Serial.println(WiFi.localIP());
  pinMode(GreenLED, OUTPUT);
  pinMode(YellowLED, OUTPUT);
  pinMode(RedLED, OUTPUT);

  client.setCallback(callback);
}

void loop () {
    if (!client.connected()) {
    reconnect();
  }

  client.loop();
 
}