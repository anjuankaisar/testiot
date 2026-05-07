#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

// =====================
// WIFI
// =====================
const char* ssid = "Janji Jiwa Culture";
const char* password = "Joeseries";

// =====================
// OTA URL
// =====================
// GANTI dengan file .bin hasil export sketch
const char* firmwareURL =
"https://raw.githubusercontent.com/anjuankaisar/testiot/main/firmware.bin";


// =====================
// PIN LAMPU
// =====================
#define A_RED 15
#define A_YELLOW 2
#define A_GREEN 4

#define B_RED 16
#define B_YELLOW 17
#define B_GREEN 5

#define C_RED 18
#define C_YELLOW 19
#define C_GREEN 21

#define D_RED 13
#define D_YELLOW 12
#define D_GREEN 14


// =====================
// OTA TIMER
// =====================
unsigned long prevCheck = 0;
const long intervalCheck = 30000; // 30 detik


// =====================
// OTA FUNCTION
// =====================
void checkUpdate() {

  Serial.println("Checking firmware update...");

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  http.begin(client, firmwareURL);

  int httpCode = http.GET();

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {

    int contentLength = http.getSize();

    Serial.print("File Size: ");
    Serial.println(contentLength);

    if (contentLength <= 0) {

      Serial.println("Invalid content length");
      http.end();
      return;
    }

    bool canBegin = Update.begin(contentLength);

    if (canBegin) {

      Serial.println("Start OTA Update");

      WiFiClient* stream = http.getStreamPtr();

      size_t written = Update.writeStream(*stream);

      Serial.print("Written: ");
      Serial.println(written);

      if (written == contentLength) {

        Serial.println("Written successfully");
      }

      else {

        Serial.println("Written only partially");
      }

      if (Update.end()) {

        Serial.println("OTA Done!");

        if (Update.isFinished()) {

          Serial.println("Update successfully completed");
          Serial.println("Rebooting...");

          ESP.restart();
        }

        else {

          Serial.println("Update not finished");
        }
      }

      else {

        Serial.print("Update Error #: ");
        Serial.println(Update.getError());
      }
    }

    else {

      Serial.println("Not enough space for OTA");
    }
  }

  else {

    Serial.println("Firmware download failed");
  }

  http.end();
}


// =====================
// SMART DELAY
// =====================
void smartDelay(int ms) {

  int step = ms / 10;

  for (int i = 0; i < 10; i++) {

    delay(step);

    if (millis() - prevCheck >= intervalCheck) {

      prevCheck = millis();

      checkUpdate();
    }
  }
}


// =====================
// FUNCTION LAMPU
// =====================
void jalanLampu(int red, int yellow, int green) {

  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);

  smartDelay(1000);

  digitalWrite(green, LOW);
  digitalWrite(yellow, HIGH);

  smartDelay(1000);

  digitalWrite(yellow, LOW);
  digitalWrite(red, HIGH);

  smartDelay(1000);
}


// =====================
// SETUP
// =====================
void setup() {

  Serial.begin(115200);

  int pins[] = {
    A_RED, A_YELLOW, A_GREEN,
    B_RED, B_YELLOW, B_GREEN,
    C_RED, C_YELLOW, C_GREEN,
    D_RED, D_YELLOW, D_GREEN
  };

  for (int i = 0; i < 12; i++) {

    pinMode(pins[i], OUTPUT);
  }

  // Semua merah awal
  digitalWrite(A_RED, HIGH);
  digitalWrite(B_RED, HIGH);
  digitalWrite(C_RED, HIGH);
  digitalWrite(D_RED, HIGH);


  // =====================
  // WIFI CONNECT
  // =====================
  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  // =====================
  // CHECK UPDATE SAAT BOOT
  // =====================
  checkUpdate();
}


// =====================
// LOOP
// =====================
void loop() {

  jalanLampu(A_RED, A_YELLOW, A_GREEN);
  jalanLampu(B_RED, B_YELLOW, B_GREEN);
  jalanLampu(C_RED, C_YELLOW, C_GREEN);
  jalanLampu(D_RED, D_YELLOW, D_GREEN);
}