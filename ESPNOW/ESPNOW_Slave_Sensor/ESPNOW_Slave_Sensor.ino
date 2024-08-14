#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11
#define BOARD_ID 3

// MAC Address of the ESP32 sender
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
constexpr char WIFI_SSID[] = "NNTruong";

DHT dht(DHTPIN, DHTTYPE);

// Define variables to store readings to be sent
float temperature;
float lastTemperature;
float humidity;
float lastHumidity;

// Structure to send data
typedef struct struct_message {
    int id;
    float temp;
    float hum;
} struct_message;

// Create a struct_message to hold sensor readings
struct_message SensorReadings;

unsigned long lastTime = 0;
const unsigned long timerDelay = 2000;
unsigned long lastTime1 = 0;
const unsigned long timerDelay1 = 20000;


int32_t getWiFiChannel(const char *ssid) {
  int32_t n = WiFi.scanNetworks();
  for (uint8_t i = 0; i < n; i++) {
    if (strcmp(ssid, WiFi.SSID(i).c_str()) == 0) {
      return WiFi.channel(i);
    }
  }
  return 0;
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("________________________________________________________________________________");
  Serial.print("Last Packet Send Status: ");
  Serial.println(sendStatus == 0 ? "Delivery data sensor success" : "Delivery data sensor fail");
}

void getReadings() {
  // Read data from DHT sensor
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  SensorReadings.id = BOARD_ID;
  SensorReadings.temp = temperature;
  SensorReadings.hum = humidity;

  // Check if any reads failed and set default values
  if (isnan(temperature)) {
    Serial.println("Failed to read temperature");
    temperature = 0.0;
  }
  if (isnan(humidity)) {
    Serial.println("Failed to read humidity");
    humidity = 0.0;
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  // Init DHT sensor
  dht.begin();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  int32_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.printDiag(Serial);
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  WiFi.printDiag(Serial);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Register callback for send status
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, channel, NULL, 0);
}

void sendReadings() {
  esp_now_send(NULL, (uint8_t*)&SensorReadings, sizeof(SensorReadings));
  Serial.println("temperature: " + String(SensorReadings.temp));
  Serial.println("humidity: " + String(SensorReadings.hum));
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getReadings();
    if (temperature != lastTemperature || humidity != lastHumidity) {
      lastTemperature = temperature;
      lastHumidity = humidity;
      sendReadings();
    }
    lastTime = millis();
  }

  if ((millis() - lastTime1) > timerDelay1) {
    sendReadings();
    lastTime1 = millis();
  }
}
