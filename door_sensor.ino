#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your network ssid";
const char* wifi_password = "your password";
const char* mqtt_server = "ip address on the pi";
const char* clientID = "door_sensor";
const char* mqtt_topic = "door_status";

const int DOOR_SENSOR_PIN = 0;  // Arduino pin connected to door sensor's pin

int currentDoorState; // current state of door sensor
int lastDoorState;    // previous state of door sensor

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);


void setup() {
  Serial.begin(9600);                     // initialize serial
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(LED_BUILTIN, OUTPUT);

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


  currentDoorState = digitalRead(DOOR_SENSOR_PIN); // read state

    // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
  
}

void loop() {

  
  lastDoorState = currentDoorState;              // save the last state
  currentDoorState  = digitalRead(DOOR_SENSOR_PIN); // read new state

  if (lastDoorState == LOW && currentDoorState == HIGH) { // state change: LOW -> HIGH
    Serial.println("The door-opening event is detected");
    digitalWrite(LED_BUILTIN, LOW);
    if (client.publish(mqtt_topic, "1")) {
      Serial.println("door closed and message sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(mqtt_topic, "1");
    }
  }
  else
  if (lastDoorState == HIGH && currentDoorState == LOW) { // state change: HIGH -> LOW
    Serial.println("The door-closing event is detected");
    digitalWrite(LED_BUILTIN, HIGH);
    if (client.publish(mqtt_topic, "0")) {
      Serial.println("door open and message sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(mqtt_topic, "0");
    }
  }
}