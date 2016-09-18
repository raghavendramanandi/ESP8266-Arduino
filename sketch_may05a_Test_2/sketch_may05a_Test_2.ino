#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "dlink";
const char* password = "oahf-fx9e-vimo";
const char* host     = "www.theikk.com"; // Your domain
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
  
  Serial.println(WiFi.status());
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".---- Inside While in setup -------");
    Serial.println(WiFi.status());
    if(WiFi.status() == WL_NO_SSID_AVAIL){
      Serial.println("########in if block#######");
      ssid     = "dlink";
      password = "oahf-fx9e-vimo";
      WiFi.begin(ssid, password);
    }
  }

  Serial.println(WiFi.status());
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
  }

  Serial.println("end loop");
}
