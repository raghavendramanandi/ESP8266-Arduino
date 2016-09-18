#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "dlink";
const char* password = "oahf-fx9e-vimo";
const char* host     = "www.raghavendramanandi.site88.net"; // Your domain
String path          = "/main.php";
const int pin        = 2;

void setup() {
  pinMode(pin, OUTPUT);
  pinMode(pin, HIGH);
  Serial.begin(115200);
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}


void loop() {
  Serial.println("Begin loop");
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  } else {
    Serial.println("connection success");
  }

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n\r\n");

  delay(500); // wait for server to respond

  // read response
  String section = "header";
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println("client.readStringUntil");
    Serial.println(line);

    if (section == "header") { // headers..
      Serial.println("header section");
      Serial.print(".");
      if (line == "\n") { // skips the empty space at the beginning
        section = "json";
      }
    }
    else if (section == "json") { // print the good stuff
      Serial.println("json section");
      section = "ignore";
      String result = line.substring(1);

      // Parse JSON
      int  toggle = 1;
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      Serial.println("json content is:"  );
      Serial.println(json);
      JsonObject& json_parsed = jsonBuffer.parseObject(json);
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        return;
      }
      if (strcmp(json_parsed["status"], "success") == 0) {
        while (1) {
          if (toggle) {
            digitalWrite(pin, HIGH);
            Serial.println("LED ON");
            delay(500);
            toggle = 0;
          } else {
            digitalWrite(pin, LOW);
            Serial.println("LED OFF");
            delay(500);
            toggle = 1;
          }
        }
      }
      else {

        digitalWrite(pin, HIGH);
        Serial.println("Operation failed");
        delay(5000);
      }
    }
  }
  Serial.println("end loop");
}
