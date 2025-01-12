#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ArduinoJson.h>

// Konstanta untuk AP mode
const char* apSSID = "MyIR-SmartCar";
const char* apPassword = "irfandiproduct";
const char* tokenDevice = "Mw9S2SM";
const char* tipeDevice = "IrCar-Bullseye-1.10";
int threshold = 500;

bool hasConnection = false;

// Web server pada port 80
AsyncWebServer server(80);

// dikralasi pin
const int ENA = D5; // Pin PWM untuk motor 1
const int IN1 = D1; // Kontrol arah motor 1
const int IN2 = D2; // Kontrol arah motor 1
const int IN3 = D3; // kontrol arah motor 2
const int IN4 = D4; // kontrol arah motor 2
const int ENB = D6; // Pin PWM untuk motor 1
const int sensorAnalog = A0;
const int Buzzer = D0;
String carMode = "control";

// Fungsi untuk menulis file konfigurasi
void saveConfig(const char* path, const char* ssid, const char* password, bool internet) {
  StaticJsonDocument<256> doc;
  doc["internet"] = internet;
  doc["ssid"] = ssid;
  doc["password"] = password;

  File file = SPIFFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  serializeJson(doc, file);
  file.close();
  Serial.println("Configuration saved successfully");
}

// Fungsi untuk membaca file konfigurasi
bool loadConfig(const char* path, String& ssid, String& password, bool& internet) {
  File file = SPIFFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return false;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse JSON file");
    file.close();
    return false;
  }

  internet = doc["internet"];
  ssid = String((const char*)doc["ssid"]);
  password = String((const char*)doc["password"]);
  file.close();
  return true;
}

// Fungsi untuk memulai mode Access Point
void startAPMode() {
  WiFi.softAP(apSSID, apPassword);
  Serial.println("AP Mode Started");
  Serial.printf("SSID: %s, Password: %s\n", apSSID, apPassword);
  Serial.println(WiFi.softAPIP());

  // Konfigurasi halaman web
}

// Fungsi untuk menghubungkan ke WiFi
void connectToWiFi(const String& ssid, const String& password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.printf("Connecting to WiFi SSID: %s\n", ssid.c_str());

  unsigned long startAttemptTime = millis();

  // Tunggu hingga koneksi berhasil atau timeout setelah 10 detik
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi, switching to AP mode");
    startAPMode();
  }
}

void setup() {
  Serial.begin(9600);
  // Atur pin sebagai output
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  // Inisialisasi SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }

  // Cek apakah file konfigurasi ada
  String ssid, password;
  bool internet;

  if (loadConfig("/config.txt", ssid, password, internet)) {
    Serial.println("Configuration loaded:");

    Serial.println("Internet: " + String(internet ? "true" : "false"));
    Serial.println("SSID: " + ssid);
    Serial.println("Password: " + password);

    if (internet) {
      hasConnection = true;
      connectToWiFi(ssid, password);
    } else {
      hasConnection = false;
      startAPMode();
    }
  } else {
    Serial.println("Failed to load configuration, starting AP mode");
    saveConfig("/config.txt", "", "", false);
    startAPMode();
  }


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html lang="en">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Wifi Setup - MyIR SmartCar Configuration</title>
        <style>
          body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            color: #333;
            text-align: center;
            padding: 20px;
          }
          h1 {
            color: #444;
          }
          form {
            display: inline-block;
            background: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
          }
          input[type="text"], input[type="password"], input[type="checkbox"] {
            width: 95%;
            padding: 10px;
            margin: 10px 0;
            border: 1px solid #ddd;
            border-radius: 4px;
          }
          button {
            background-color: #007BFF;
            color: white;
            padding: 10px 15px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
          }
          button:hover {
            background-color: #0056b3;
          }
        </style>
      </head>
      <body>
        <h1>WiFi Configuration - Please set up your wifi at here</h1>
        <form action="/setwifi" method="POST">
          <label for="ssid">SSID:</label>
          <input type="text" id="ssid" name="ssid" required><br>
          <label for="password">Password:</label>
          <input type="password" id="password" name="password" required><br>
          <label for="internet">Enable Internet:</label>
          <input type="checkbox" id="internet" name="internet"><br>
          <button type="submit">Save</button>
        </form>
      </body>
      </html>
    )rawliteral");
  });

  server.on("/setwifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();
    bool changeInternet = request->hasParam("internet", true);

    saveConfig("/config.txt", ssid.c_str(), password.c_str(), changeInternet);

    request->send(200, "text/plain", "WiFi credentials saved! please Reboot the ESP8266.");
  });

  server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    String tokenClient = request->getParam("token", true)->value();
    String deviceClient = request->getParam("device", true)->value();
    Serial.println("Ada Device mencoba untuk connect!");

    if (deviceClient == tipeDevice) {
      if (tokenClient == tokenDevice){
        request->send(200, "text/plain", "Connected!");
        Serial.println("Device berhasil connect dengan token yang sesuai");
        carMode = "control";
      } else  {
        request->send(401, "text/plain", "Your token is Wrong!");
        Serial.println("Device gagal connect karena token salah!");
      }
    } else {
      request->send(401, "text/plain", "The device type you selected is not appropriate!");
      Serial.println("Device gagal connect karena token salah!");
    }
  });

  server.on("/maju", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (carMode == "control"){
      String speedParam = request->getParam("speed", true)->value();
      int speed = speedParam.toInt();

      if (speed <= 150 || speed > 255) { // Validasi nilai PWM
        request->send(400, "text/plain", "Invalid speed value!");
        return;
      } else {
        maju(speed); // Fungsi untuk menggerakkan motor maju
        request->send(200, "text/plain", "Connected! Motor is moving forward at speed: " + String(speed));
      }
    }
  });
  server.on("/mundur", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (carMode == "control"){
      String speedParam = request->getParam("speed", true)->value();
      int speed = speedParam.toInt();

      if (speed <= 150 || speed > 255) { // Validasi nilai PWM
        request->send(400, "text/plain", "Invalid speed value!");
        return;
      } else {
        mundur(speed); // Fungsi untuk menggerakkan motor maju
        request->send(200, "text/plain", "Connected! Motor is moving forward at speed: " + String(speed));
      }
    }
  });
  server.on("/kanan", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (carMode == "control"){
      String speedParam = request->getParam("speed", true)->value();
      int speed = speedParam.toInt();

      if (speed <= 150 || speed > 255) { // Validasi nilai PWM
        request->send(400, "text/plain", "Invalid speed value!");
        return;
      } else {
        belokKanan(speed); // Fungsi untuk menggerakkan motor maju
        request->send(200, "text/plain", "Connected! Motor is moving forward at speed: " + String(speed));
      }
    }
  });
  server.on("/kiri", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (carMode == "control"){
      String speedParam = request->getParam("speed", true)->value();
      int speed = speedParam.toInt();

      if (speed <= 150 || speed > 255) { // Validasi nilai PWM
        request->send(400, "text/plain", "Invalid speed value!");
        return;
      } else {
        belokKiri(speed); // Fungsi untuk menggerakkan motor maju
        request->send(200, "text/plain", "Connected! Motor is moving forward at speed: " + String(speed));
      }
    }
  });
  server.on("/buzzer", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("state", true)) { // Periksa apakah parameter "state" ada
        String state = request->getParam("state", true)->value(); // Ambil nilainya sebagai string
        if (state == "true") { // Bandingkan dengan "true" (bukan boolean)
            digitalWrite(Buzzer, HIGH);
        } else {
            digitalWrite(Buzzer, LOW);
        }
        request->send(200, "text/plain", "Buzzer state updated");
    } else {
        request->send(400, "text/plain", "Missing parameter 'state'");
    }
  });
  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    stopMotors();
    request->send(200, "text/html", "Stoped");
  });
  server.on("/line", HTTP_GET, [](AsyncWebServerRequest *request) {
    carMode = "line";
    request->send(200, "text/html", "changed");
  });
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request) {
    carMode = "control";
    request->send(200, "text/html", "changed");
  });

  server.begin();
  // Set arah motor awal (maju)
  stopMotors();
  digitalWrite(Buzzer, LOW);
}

void loop() {
  // Tidak diperlukan loop khusus karena AsyncWebServer menangani semuanya
  // Tingkatkan kecepatan motor secara bertahap
  // for (int speed = 0; speed <= 200; speed += 10) {
  //   analogWrite(ENA, speed); // Atur kecepatan motor (0 - 1023)
  //   delay(500);              // Tunggu 0.5 detik
  // }

  // // Kurangi kecepatan motor secara bertahap
  // for (int speed = 200; speed >= 0; speed -= 10) {
  //   analogWrite(ENA, speed); // Atur kecepatan motor (0 - 1023)
  //   delay(500);              // Tunggu 0.5 detik
  // }
  // int sensorValue = analogRead(sensorAnalog); // Baca nilai dari sensor
  // Serial.print("Nilai Sensor: ");
  // Serial.println(sensorValue);

  // // Deteksi warna berdasarkan nilai ambang
  // if (sensorValue < threshold) {
  //   Serial.println("Warna Hitam (Garis Terdeteksi)");
  // } else {
  //   Serial.println("Warna Putih (Tidak Ada Garis)");
  // }

  // delay(100);
  if (carMode == "line"){
    int garisValue = analogRead(sensorAnalog);
    if (garisValue < threshold){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENA, 230);
      analogWrite(ENB, 200);
    } else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENA, 0);
      analogWrite(ENB, 210);
    }
  }
}

void maju(int pwm) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
}

// Fungsi untuk bergerak mundur
void mundur(int pwm) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
}

void belokKanan(int pwm) {
  // Motor kiri maju, motor kanan berhenti
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm); // Motor kanan berhenti
}

void belokKiri(int pwm) {
  // Motor kanan maju, motor kiri berhenti
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, pwm); // Motor kiri berhenti
  analogWrite(ENB, pwm);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}




