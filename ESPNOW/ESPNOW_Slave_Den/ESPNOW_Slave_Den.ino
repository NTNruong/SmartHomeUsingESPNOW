#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>

constexpr char WIFI_SSID[] = "NNTruong";

// Define variables to store incoming readings
int LEDState = -1; // Initial state to ensure any incoming value is processed

// Structure to receive data
typedef struct struct_message {
  int id;
  float temp;
  float hum;
  int LED;
} struct_message;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Define LED pins
const int LED1 = D1;
const int LED2 = D2;

int32_t getWiFiChannel(const char *ssid) {
  int32_t n = WiFi.scanNetworks();
  for (uint8_t i = 0; i < n; i++) {
    if (strcmp(ssid, WiFi.SSID(i).c_str()) == 0) {
      return WiFi.channel(i);
    }
  }
  return 0;
}

void printMAC(const uint8_t *mac_addr) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
}

// Callback when data is received
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.println("____________________________________________________________");
  Serial.printf("%u bytes of data received from : ", len);
  printMAC(mac_addr);
  Serial.println();
  Serial.printf("Received from board ID %d\n", incomingReadings.id);
  Serial.printf("Temp: %.2f, Hum: %.2f, LED: %d\n", incomingReadings.temp, incomingReadings.hum, incomingReadings.LED);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  EEPROM.begin(1);
  delay(10);

  // Init GPIO
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  incomingReadings.LED = EEPROM.read(0);
  handleLEDs(LEDState);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.begin();

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

  // Register callback for data reception
  esp_now_register_recv_cb(OnDataRecv);

}

void handleLEDs(int LEDState) {
  switch (LEDState) {
    case 1:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      Serial.println("LED1: OFF \t LED2: OFF");
      break;
    case 2:
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      Serial.println("LED1: OFF \t LED2: ON");
      break;
    case 3:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      Serial.println("LED1: ON \t LED2: OFF");
      break;
    case 4:
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      Serial.println("LED1: ON \t LED2: ON");
      break;
    default:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      Serial.println("LED1: OFF \t LED2: OFF");
      break;
    
  }
}

void loop() {
  if (incomingReadings.LED != LEDState) {
    LEDState = incomingReadings.LED;
    EEPROM.write(0, incomingReadings.LED);
    handleLEDs(LEDState);
    EEPROM.commit();
  }
}
