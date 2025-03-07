#include <PPP.h>
#include <PubSubClient.h>

#define PPP_MODEM_APN ""
#define PPP_MODEM_PIN NULL  // or NULL

// WaveShare SIM7600 HW Flow Control
#define PPP_MODEM_RST     15
#define PPP_MODEM_RST_LOW false  //active HIGH
#define PPP_MODEM_TX      17
#define PPP_MODEM_RX      16
#define PPP_MODEM_RTS     -1
#define PPP_MODEM_CTS     -1
#define PPP_MODEM_FC      ESP_MODEM_FLOW_CONTROL_NONE
#define PPP_MODEM_MODEL   PPP_MODEM_SIM7600

void onEvent(arduino_event_id_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_PPP_START:        Serial.println("PPP Started"); break;
    case ARDUINO_EVENT_PPP_CONNECTED:    Serial.println("PPP Connected"); break;
    case ARDUINO_EVENT_PPP_GOT_IP:       Serial.println("PPP Got IP"); break;
    case ARDUINO_EVENT_PPP_LOST_IP:      Serial.println("PPP Lost IP"); break;
    case ARDUINO_EVENT_PPP_DISCONNECTED: Serial.println("PPP Disconnected"); break;
    case ARDUINO_EVENT_PPP_STOP:         Serial.println("PPP Stopped"); break;
    default:                             break;
  }
}

void testClient(const char *host, uint16_t port) {
  NetworkClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection Failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    client.read();  //Serial.write(client.read());
  }

  Serial.printf("Connection Success to %s:%d OK\n", host, port);
  client.stop();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "YourDeviceID"
#define MQTT_TOPIC_PUB "test/topic/pub"
#define MQTT_TOPIC_SUB "test/topic/sub"

NetworkClient espClient;
PubSubClient mqttClient(espClient);

void MQTT_Init() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  Serial.print("Attempting MQTT connection to ");
  Serial.print(MQTT_BROKER);
  Serial.print(":");
  Serial.println(MQTT_PORT);
  
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println("Connected to MQTT broker");
    mqttClient.subscribe(MQTT_TOPIC_SUB);
  } else {
    Serial.print("MQTT connection failed, rc=");
    Serial.println(mqttClient.state());
  }  
  
  mqttClient.loop();
}

#define LED_PIN   2

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.begin(115200);

  // Listen for modem events
  Network.onEvent(onEvent);

  // Configure the modem
  PPP.setApn(PPP_MODEM_APN);
  PPP.setPin(PPP_MODEM_PIN);
  PPP.setResetPin(PPP_MODEM_RST, PPP_MODEM_RST_LOW);
  PPP.setPins(PPP_MODEM_TX, PPP_MODEM_RX, PPP_MODEM_RTS, PPP_MODEM_CTS, PPP_MODEM_FC);

  delay(2000);
  Serial.println("\nStarting the modem. It might take a while!");
  PPP.begin(PPP_MODEM_MODEL);

  Serial.print("Manufacturer: ");
  Serial.println(PPP.cmd("AT+CGMI", 10000));
  Serial.print("Model: ");
  Serial.println(PPP.moduleName());
  Serial.print("IMEI: ");
  Serial.println(PPP.IMEI());

  bool attached = PPP.attached();
  if (!attached) {
    int i = 0;
    unsigned int s = millis();
    Serial.print("Waiting to connect to network");
    while (!attached && ((++i) < 60)) {
      Serial.print(".");
      delay(1000);
      attached = PPP.attached();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    Serial.print((millis() - s) / 1000.0, 1);
    Serial.println("s");
    attached = PPP.attached();
  }

  Serial.print("Attached: ");
  Serial.println(attached);
  Serial.print("State: ");
  Serial.println(PPP.radioState());
  if (attached) {
    Serial.print("Operator: ");
    Serial.println(PPP.operatorName());
    Serial.print("IMSI: ");
    Serial.println(PPP.IMSI());
    Serial.print("RSSI: ");
    Serial.println(PPP.RSSI());
    int ber = PPP.BER();
    if (ber > 0) {
      Serial.print("BER: ");
      Serial.println(ber);
      Serial.print("NetMode: ");
      Serial.println(PPP.networkMode());
    }

    Serial.println("Switching to data mode...");
    PPP.mode(ESP_MODEM_MODE_CMUX);  // Data and Command mixed mode
    if (!PPP.waitStatusBits(ESP_NETIF_CONNECTED_BIT, 1000)) {
      Serial.println("Failed to connect to internet!");
    } else {
      Serial.println("Connected to internet!");
    }
  } else {
    Serial.println("Failed to connect to network!");
  }

  MQTT_Init();
}

void loop() {
  static int cnt = 0;
  if(++cnt > 10){
    cnt = 0;
    if (PPP.connected()) {
      testClient("google.com", 80);
    }
  }
  delay(200);
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}
