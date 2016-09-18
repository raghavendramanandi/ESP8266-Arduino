#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

const char* ssid     = "";  
const char* password = "";

const char* host     = ""; // Your domain  
String path          = "/path/to/light.json";  
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
  
          String ssidString = "Test";
          String passwordString = "Pass123";
          
          Serial.println(ssidString);
          Serial.println(passwordString);
          
          ssidString.toCharArray(ssid_file, ssidString.length() + 1);
          passwordString.toCharArray(password_file, passwordString.length() + 1);
          
          saveCredentials();

          loadCredentials();
  
  Serial.print("closing connection. ");
}
