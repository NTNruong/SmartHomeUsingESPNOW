#include <esp_now.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Arduino_JSON.h>
#include <esp_task_wdt.h>
#include <EEPROM.h>

#define BOARD_ID 1

uint8_t broadcastAddress1[] = {0x8C, 0xAA, 0xB5, 0x0C, 0x2D, 0xD7};
uint8_t broadcastAddress2[] = {0xB4, 0x8A, 0x0A, 0xF5, 0x88, 0x9D};
uint8_t broadcastAddress3[] = {0xB4, 0xE6, 0x2D, 0x29, 0xCA, 0x4C};

const char* ssid = "NNTruong";
const char* password = "tamsohai";

float lastTemp;
float lastHum;
int LED;
int lastLED;
int LED1;
int LED2;
int lastLED1;
int lastLED2;

typedef struct struct_message {
  int id;
  float temp;
  float hum;
  int LED;
} struct_message;

struct_message SensorReadings;
struct_message incomingReadings;

unsigned long lastTime = 0;
const unsigned long timerUpdate = 1000;
unsigned long lastTimeFB = 0;
const unsigned long timerUpdateFB = 1000;
unsigned long lastTimeSetFB = 0;
const unsigned long timerSetFB = 10000;
const unsigned long timerOut = 8;

esp_now_peer_info_t peerInfo;

#define FIREBASE_HOST "fir-demo-20d39-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyCLYbGuLu_9J6jiNAQ5qdcUWHWLJvuW_WM"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void printMAC(const uint8_t* mac_addr) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success to " : "Delivery Fail to ");
  printMAC(mac_addr);
  Serial.println();
}

void OnDataRecv(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.println("\n________________________________________________________________________________");
  Serial.print(len);
  Serial.print(" bytes of data received from: ");
  printMAC(mac_addr);
  Serial.println();
  Serial.print("Received from board ID: ");
  Serial.println(incomingReadings.id);

  if (memcmp(mac_addr, broadcastAddress2, 6) == 0) {
    lastTemp = incomingReadings.temp;
    lastHum = incomingReadings.hum;
  }
}

void getReadings() {
  SensorReadings.id = BOARD_ID;
  SensorReadings.temp = incomingReadings.temp;
  SensorReadings.hum = incomingReadings.hum;
  SensorReadings.LED = LED;
  EEPROM.write(0, incomingReadings.temp);
  EEPROM.write(1, incomingReadings.hum);
  EEPROM.write(2, LED);
}

bool checkFirebaseConnection() {
  if (!Firebase.ready()) {
    Serial.println("Firebase connection lost. Reconnecting...");
    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);
    delay(1000);
    if (Firebase.ready()) {
      Serial.println("Firebase reconnected successfully");
      return true;
    } else {
      Serial.println("Failed to reconnect to Firebase");
      return false;
    }
  }
  return true;
}

void setupWiFi() {
  WiFi.setSleep(false);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
}

void setupFirebase() {
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void setupESPNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  uint8_t* broadcastAddresses[] = {broadcastAddress1, broadcastAddress2, broadcastAddress3};
  for (int i = 0; i < 3; i++) {
    memcpy(peerInfo.peer_addr, broadcastAddresses[i], 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.printf("Failed to add peer %d\n", i + 1);
    }
  }
}

void sendSensorReadings() {
  if (incomingReadings.temp != lastTemp || incomingReadings.hum != lastHum || LED != lastLED) {
    lastTemp = incomingReadings.temp;
    lastHum = incomingReadings.hum;
    lastLED = LED;
    esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t*)&SensorReadings, sizeof(SensorReadings));
    esp_err_t result2 = esp_now_send(broadcastAddress3, (uint8_t*)&SensorReadings, sizeof(SensorReadings));
    Serial.println("temperature: " + String(incomingReadings.temp));
    Serial.println("humidity: " + String(incomingReadings.hum));
    Serial.print("LED: ");
    Serial.println(LED);
    if (result1 == ESP_OK) {
      Serial.println("Sent to broadcastAddress1 successfully");
    } else {
      Serial.println("Failed to send to broadcastAddress1");
    }
    if (result2 == ESP_OK) {
      Serial.println("Sent to broadcastAddress3 successfully");
    } else {
      Serial.println("Failed to send to broadcastAddress3");
    }
  }

  if ((millis() - lastTimeSetFB) > timerSetFB) {
    if (Firebase.ready()) {
      String path = "/users/OEg1kRxDxzN99aCxxsdmTwDiNt22/sensors/-O1jq8-GMNSh_mqnmJd3/";
      FirebaseJson json;
      json.set("humidity", incomingReadings.hum);
      json.set("temperature", incomingReadings.temp);
      if (Firebase.updateNode(firebaseData, path, json)) {
        Serial.println("Data sent to Firebase successfully");
      } else {
        Serial.println("Failed to send data to Firebase");
        Serial.println(firebaseData.errorReason());
      }
    }
    lastTimeSetFB = millis();
  }
}

bool getFirebaseBool(String path, int& value) {
  if (Firebase.getInt(firebaseData, path)) {
    value = firebaseData.intData();
    return true;
  } else {
    Serial.println("Failed to get data from Firebase");
    Serial.println(firebaseData.errorReason());
    return false;
  }
}

void decoder() {
  if (LED1 == 0 && LED2 == 0) LED = 1;
  if (LED1 == 0 && LED2 == 1) LED = 2;
  if (LED1 == 1 && LED2 == 0) LED = 3;
  if (LED1 == 1 && LED2 == 1) LED = 4;
}

void getFirebaseData() {
  bool updated = false;

  String ledPath1 = "/users/OEg1kRxDxzN99aCxxsdmTwDiNt22/devices/-O1jpIkdL62itBg6e-Dn/status";
  String ledPath2 = "/users/OEg1kRxDxzN99aCxxsdmTwDiNt22/devices/-O1jqCeYuxozYN5n_Dzp/status";          

  if (getFirebaseBool(ledPath1, LED1)) {
    if (LED1 != lastLED1) {
      lastLED1 = LED1;
      Serial.print("LED1 updated from Firebase: ");
      Serial.println(LED1);
      updated = true;
    }
  }

  if (getFirebaseBool(ledPath2, LED2)) {
    if (LED2 != lastLED2) {
      lastLED2 = LED2;
      Serial.print("LED2 updated from Firebase: ");
      Serial.println(LED2);
      updated = true;
    }
  }

  if (updated) {
    decoder();
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(3);
  delay(10);

  incomingReadings.temp = EEPROM.read(0);
  incomingReadings.hum = EEPROM.read(1);
  LED = EEPROM.read(2);

  setupWiFi();
  setupFirebase();
  setupESPNow();
  esp_task_wdt_init(timerOut, true);  // Enable watchdog timer with 10 seconds timeout
  esp_task_wdt_add(NULL);       // Add current thread to watchdog
}

void loop() {
  esp_task_wdt_reset();  // Reset watchdog timer to prevent ESP32 from resetting

  if ((millis() - lastTime) > timerUpdate) {
    getFirebaseData();
    getReadings();
    sendSensorReadings();
    EEPROM.commit();
    lastTime = millis();
    Serial.print(".");
  }
}
