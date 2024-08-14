#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// Define TFT pins
#define TFT_CS D1
#define TFT_RST D2
#define TFT_DC D3

constexpr char WIFI_SSID[] = "NNTruong";

// Create TFT display object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Define variables to store incoming readings
float temperature;
float humidity;
int LEDState;

// Structure to receive data
typedef struct struct_message {
  int id;
  float temp;
  float hum;
  int LED;
} struct_message;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

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
  Serial.printf("%u bytes of data received from: ", len);
  printMAC(mac_addr);
  Serial.println();
  Serial.printf("Received from board ID %d\n", incomingReadings.id);
  Serial.printf("Temperature: %.2f, Humidity: %.2f, LED: %d\n", incomingReadings.temp, incomingReadings.hum, incomingReadings.LED);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  EEPROM.begin(3);
  delay(10);

  incomingReadings.temp = EEPROM.read(0);
  incomingReadings.hum = EEPROM.read(1);
  incomingReadings.LED = EEPROM.read(2);

  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  // Init TFT display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2, 2);
  displayData();
  
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

void displayData() {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.print("Temp: ");
  tft.setCursor(130, 0);
  tft.println(" C");

  tft.setCursor(0, 25);
  tft.print("Hum: ");
  tft.setCursor(130, 25);
  tft.println(" %");

  tft.setCursor(0, 55);
  tft.print("LED1: ");

  tft.setCursor(0, 85);
  tft.print("LED2: ");
}

void displayTemp() {
  tft.fillRect(70, 0, 60, 16, ST7735_BLACK);
  tft.setCursor(70, 0);
  tft.print(temperature);
}

void displayHum() {
  tft.fillRect(70, 25, 60, 16, ST7735_BLACK);
  tft.setCursor(70, 25);
  tft.print(humidity);
}

void displayLEDs() {
  if (LEDState == 3 || LEDState == 4) {
    tft.fillCircle(95, 60, 10, ST7735_RED);
  } else {
    tft.fillCircle(95, 60, 10, ST7735_CYAN);
  }
  
  if (LEDState == 2 || LEDState == 4) {
    tft.fillCircle(95, 90, 10, ST7735_RED);
  } else {
    tft.fillCircle(95, 90, 10, ST7735_CYAN);
  }
}

void loop() {
  if (incomingReadings.temp != temperature) {
    temperature = incomingReadings.temp;
    EEPROM.write(0, incomingReadings.temp);
    displayTemp();
  }
  if (incomingReadings.hum != humidity) {
    humidity = incomingReadings.hum;
    EEPROM.write(1, incomingReadings.hum); 
    displayHum();
  }
  if (incomingReadings.LED != LEDState) {
    LEDState = incomingReadings.LED;
    EEPROM.write(2, incomingReadings.LED);
    displayLEDs();
  }
    EEPROM.commit();
}
