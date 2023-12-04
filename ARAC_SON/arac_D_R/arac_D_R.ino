#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AccelStepper.h>
int place = 0, last = 0;
const char* ssid = "Tenda_MT";
const char* password = "mekatek2021";
const char* mqtt_server = "192.168.0.125";
AccelStepper stepper(1, D2, D5);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
volatile long enc_val = 0, last_enc_val = 0;

void ICACHE_RAM_ATTR i6()
{
  if(digitalRead(D7)==LOW){enc_val++;}
  else{enc_val--;}
}

void ICACHE_RAM_ATTR i7()
{
  if(digitalRead(D6)==LOW){enc_val--;}
  else{enc_val++;}
}


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
    String input = "";
    for (int i = 0; i < length; i++)
    {
      input += (char)payload[i];
    }
    place = input.toInt();
    Serial.print("step is moving to ");
    Serial.println(place);
  }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "");
      // ... and resubscribe
      client.subscribe("direksiyon2");
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
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  stepper.setMaxSpeed(600);
  stepper.setAcceleration(250);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  attachInterrupt(D6, i6, RISING);
  attachInterrupt(D7, i7, RISING);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
    if (enc_val != last_enc_val) 
  {
    last_enc_val = enc_val;
    msg= enc_val;
    client.publish("outTopic", msg);
  }
  client.loop();
  if (last != place)
  {
    stepper.moveTo(place);
    last = place;
    Serial.println("Step is moving");
  }
  stepper.run();
}
