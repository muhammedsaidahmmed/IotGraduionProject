#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Update these with values suitable for your network.
const char* ssid = "D";
const char* password = "55555555";
const char* mqtt_server = "test.mosquitto.org";  // test.mosquitto.org

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

const int servoPin = 2;  // Servo pin
const int mq4Pin = 34;   // MQ4 sensor pin

Servo myServo;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Check which servo action is being controlled based on the topic
  if (strcmp(topic, "device/servo") == 0) {
    // Control the servo
    if ((char)payload[0] == '1') {
      myServo.write(180);   // Move servo to 90 degrees
    } else {
      myServo.write(0);    // Move servo to 0 degrees
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("device/temp", "MQTT Server is Connected");
      // ... and resubscribe
      client.subscribe("device/servo");
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
  myServo.attach(servoPin); // Initialize the servo pin
  pinMode(mq4Pin, INPUT);   // Initialize MQ4 sensor pin as input
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    int mq4Value = analogRead(mq4Pin);
    Serial.print("MQ4 Sensor Value: ");
    Serial.println(mq4Value);
    char msg[50];
    snprintf(msg, sizeof(msg), "MQ4: %d", mq4Value);
    client.publish("device/mq4", msg);  // Publish MQ4 sensor data to MQTT topic
  }
}
