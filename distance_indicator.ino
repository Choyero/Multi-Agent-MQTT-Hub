

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.

const char* ssid = "Your ssid";
const char* wifi_password = "your password";
const char* mqtt_server = "your pi IP address";
const char* clientID = "distance_sensor";

int Echo = 12;
int Trig = 14;
long duration;
int distance;

String state;


WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    state = "open";
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    state = "closed";
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(clientID)) {
      Serial.println("connected");
      client.subscribe("door_status");
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
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  Serial.begin(9600);

  
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (state == "open") {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(200);
  digitalWrite(Trig, LOW);

  duration = pulseIn(Echo, HIGH);

  distance = duration * 0.034/2;
  if (distance >= 20 ) { //far away still
    client.publish("distance_status", "0"); // tell client to turn on its green led. 
  } 
  else if (distance > 10 && distance < 20) { //getting close but still not there
    client.publish("distance_status", "1"); // tell client to turn on its yellow led.
  }
  else if (distance > 7 && distance <= 10) { // good distance between 4cm and 7cm
    client.publish("distance_status", "2"); // tell clien to turn on its yellow led.
  }
  else if (distance <= 7) { // too close back up;
    client.publish("distance_status", "3");
  } 
  }
  
}