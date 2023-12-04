#include <ESP8266WiFi.h>
#include <PubSubClient.h>
int place = 0, last = 0,fren_switch,gaz_output,guc_output;

const char* ssid = "Tenda_MT";
const char* password = "mekatek2021";
const char* mqtt_server = "192.168.0.125";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

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
  String konu = String((char*)topic);
  if (konu == "fren") {
        String input = "";
    for (int i = 0; i < length; i++)
    {
      input += (char)payload[i];
    }
    place = input.toInt();
  }
    if (konu == "guc") {
              String guc_input = "";
    for (int i = 0; i < length; i++)
    {
      guc_input += (char)payload[i];
    }
    guc_output = guc_input.toInt();
  }
   //if (konu == "gaz") {
            //String gaz_input = "";
    //for (int i = 0; i < length; i++)
    //{
      //gaz_input += (char)payload[i];
    //}
    //gaz_output = gaz_input.toInt();


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
      client.publish("outTopic", "hello world");
      // ... and resubscri
      client.subscribe("fren");
      client.subscribe("guc");
      //client.subscribe("");
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
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7, INPUT);// Initialize the BUILTIN_LED pin as an output
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
 fren();
 guc();
}

void fren(){
      if (place == 1) {
      digitalWrite(D2,LOW);
      digitalWrite(D5, HIGH);
      fren_switch = digitalRead(D7);
      if(fren_switch == 0){
      digitalWrite(D5, LOW);
    }
    }
   if(place == 0) {
      digitalWrite(D2, HIGH);// D4 Fren Çeker
    }
}

  void guc(){
      if (guc_output == 1) {
      digitalWrite(D6,LOW);
    }
   if(guc_output == 0) {
      digitalWrite(D6,HIGH);// D4 Fren Çeker
    }
  }

  
