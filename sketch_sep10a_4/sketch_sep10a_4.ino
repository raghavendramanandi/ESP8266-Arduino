#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

const char* ssid     = "dlink";  
const char* password = "oahf-fx9e-vimo";

const char* host     = "www.theikk.com"; // Your domain  
String path          = "/ikk/Request.php";  
const int pin        = 2;
char ssid_file[32] = "";
char password_file[32] = "";

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
  Serial.println("IP address: " + WiFi.localIP());
}

int loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid_file);
  EEPROM.get(0+sizeof(ssid_file), password_file);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid_file)+sizeof(password_file), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid_file[0] = 0;
    password_file[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid_file);
  Serial.println(password_file);
  return 1;
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, ssid_file);
  EEPROM.put(0+sizeof(ssid_file), password_file);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid_file)+sizeof(password_file), ok);
  EEPROM.commit();
  EEPROM.end();
}

void loop() {  
  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  int loop =1;
  
  Serial.println("Entering the loop, Not coming out till its connected");
  while(loop == 1){
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      delay(500);
      loop =1;
    }else{
      Serial.println("connection success");
      loop = 0;
    }
  }
  Serial.println("connected");

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\n\r\n");

  delay(500); // wait for server to respond

  // read response
  String section="header";
  while(client.available()){
    String line = client.readStringUntil('\r');
    // Serial.print(line);
    // we’ll parse the HTML body here
    if (section=="header") { // headers..
      Serial.print(".");
      if (line=="\n") { // skips the empty space at the beginning 
        section="json";
      }
    }
    else if (section=="json") {  // print the good stuff
      section="ignore";
      String result = line.substring(1);

      // Parse JSON
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        return;
      }

      if (strcmp(json_parsed["status"], "success") == 0) {
        digitalWrite(pin, HIGH); 
        Serial.println("LED ON");
        Serial.println("Status is success");
        
        if (strcmp(json_parsed["hasPassword"], "true") == 0) {
          Serial.println("Password is available");
          Serial.println("*************************");
          String newSSID = json_parsed["ssid"];
          String newPassword = json_parsed["password"];
          Serial.println(newSSID);
          Serial.println(newPassword);
          Serial.println("-------------------------");
        }
        
      }
      else {
        Serial.println("Status is failed");
        digitalWrite(pin, LOW);
        Serial.println("led off");
      }
    }
  }
  Serial.print("closing connection. ");
}
