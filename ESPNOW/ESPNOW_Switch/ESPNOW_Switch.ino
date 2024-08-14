#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Arduino_JSON.h>
#include <esp_task_wdt.h>

// WiFi credentials
const char* ssid = "NNTruong";
const char* password = "tamsohai";

// Switch GPIO pins
const int switchPin1 = 32;
const int switchPin2 = 34;

// Variables for sensor readings
bool LEDfb1;
bool LEDfb2;
bool lastLED1;
bool lastLED2;
bool LED1;
bool LED2;
bool LED11;
bool LED22;

unsigned long lastTimeGet = 0;
const unsigned long timerGetFB = 1200;
unsigned long lastTimeSet = 0;
const unsigned long timerSetFB = 200;
const unsigned long timerOut = 8;

// Firebase credentials
#define FIREBASE_HOST "fir-demo-20d39-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyCLYbGuLu_9J6jiNAQ5qdcUWHWLJvuW_WM"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

String ledPath1 = "/users/OEg1kRxDxzN99aCxxsdmTwDiNt22/devices/-O1jpIkdL62itBg6e-Dn/status";
String ledPath2 = "/users/OEg1kRxDxzN99aCxxsdmTwDiNt22/devices/-O1jqCeYuxozYN5n_Dzp/status";

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

bool updateFirebaseBool(String path, bool value) {
  FirebaseJson json;
  json.set(path.c_str(), value);
  if (Firebase.updateNode(firebaseData, path, json)) {
    return true;
  } else {
    Serial.println("Failed to update data to Firebase");
    Serial.println(firebaseData.errorReason());
    return false;
  }
}

bool getFirebaseBool(String path, bool& value) {
  if (Firebase.getInt(firebaseData, path)) {
    value = firebaseData.boolData();
    return true;
  } else {
    Serial.println("Failed to get data from Firebase");
    Serial.println(firebaseData.errorReason());
    return false;
  }
}

void getFirebaseData() {
  if (getFirebaseBool(ledPath1, LEDfb1)) {
    if (LEDfb1 != LED1) {
      LED1 = LEDfb1;
      Serial.print("LEDfb1 updated from Firebase: ");
      Serial.println(LEDfb1);
    }
  }
  if (getFirebaseBool(ledPath2, LEDfb2)) {
    if (LEDfb2 != LED2) {
      LED2 = LEDfb2;
      Serial.print("LEDfb2 updated from Firebase: ");
      Serial.println(LEDfb2);
    }
  }
}

void setFirebaseBool() {
  LED11 = digitalRead(switchPin1);
  LED22 = digitalRead(switchPin2);
  if(LED11 != lastLED1) {
    lastLED1 = LED11;
    LED1 = !LED1;
  }
  if(LED22 != lastLED2) {
    lastLED2 = LED22;
    LED2 = !LED2;
  }
  if (Firebase.ready() && (LED11 != lastLED1 || LED22 != lastLED2 || LEDfb1 != LED1 || LEDfb2 != LED2)) {
    String ledPath11 = "/users/OEg1kRxDxzN99aCxxsdmTwDiNt22/devices/-O1jpIkdL62itBg6e-Dn";  
    String ledPath22 = "/users/OEg1kRxDxzN99aCxxsdmTwDiNt22/devices/-O1jqCeYuxozYN5n_Dzp";
    FirebaseJson json;  
    json.set("status", LED1);
    if (Firebase.updateNode(firebaseData, ledPath11, json)) {
      Serial.println("lastLED1 sent to Firebase successfully");
    } else {
      Serial.println("Failed to send lastLED1 to Firebase");
      Serial.println(firebaseData.errorReason());
    }
    json.set("status", LED2);
    if (Firebase.updateNode(firebaseData, ledPath22, json)) {
      Serial.println("lastLED2 sent to Firebase successfully");
    } else {
      Serial.println("Failed to send lastLED2 to Firebase");
      Serial.println(firebaseData.errorReason());
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  setupWiFi();
  setupFirebase();
  esp_task_wdt_init(timerOut, true);
  esp_task_wdt_add(NULL);
  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);
}

void loop() {
  esp_task_wdt_reset();
  if ((millis() - lastTimeSet) > timerSetFB) {
    setFirebaseBool();
    lastTimeSet = millis();
  }
  if ((millis() - lastTimeGet) > timerGetFB) {
    getFirebaseData();
    lastTimeGet = millis();
    Serial.print(".");
  }
}
