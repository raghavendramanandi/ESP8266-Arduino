#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

const char* ssid         = "dlink";  
const char* password     = "oahf-fx9e-vimo";
const char* host         = "www.raghavendramanandi.site88.net";  
String path              = "/request.php?id=0001";

const int pin            = 2;
char ssid_file[32]       = "";
char password_file[32]   = "";

void setup() {
  int loadCred = 0;
  pinMode(pin, OUTPUT); 
  pinMode(pin, HIGH);
  Serial.begin(115200);
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(WiFi.status() == WL_NO_SSID_AVAIL && loadCred == 0){
      if(loadCredentials() == 1){
        ssid     = ssid_file;
        password = password_file;
        Serial.println("Using secondary credentials to connect");
        WiFi.begin(ssid, password);
        loadCred =1;
      }
    }else if(WiFi.status() == WL_NO_SSID_AVAIL && loadCred != 0){
      errorLights(1);
    }
  }

  Serial.print("WiFi connected to: ");  
  Serial.print(ssid);
  Serial.println(password);
}

void errorLights(int errorCode){
  Serial.print("Error occured: ");
  Serial.println(errorCode);
  int value = -1;
  int count =10;
  while(count > 0){
    count--;
    if(value > 0){
      digitalWrite(pin, LOW);
      delay(1000);
    }else{
      digitalWrite(pin, HIGH);
      delay(1000);
    }
    value= value*-1;
  }
}

void successLight(){
  Serial.println("successLight");
  int count =10;
  while(count > 0){
    count --;
    digitalWrite(pin, HIGH);
    delay(1000);
  }
}

int loadCredentials() {
  int retVal = 1;
  EEPROM.begin(512);
  EEPROM.get(0, ssid_file);
  EEPROM.get(0+sizeof(ssid_file), password_file);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid_file)+sizeof(password_file), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid_file[0] = 0;
    password_file[0] = 0;
    retVal = 0;
  }
  Serial.print("Recovered credentials:");
  Serial.print(ssid_file);
  Serial.println(password_file);
  return retVal;
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
  Serial.println("New Credentials saved");
}

void loop() {  
  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  int loop =1;
  
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

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\n\r\n");

  delay(500); // wait for server to respond

  // read response
  String section="header";
  loop =1;
  while(loop == 1){
    if(client.available()){
      String line = client.readStringUntil('\r');
      if (section=="header") {
        Serial.print(".");
        if (line=="\n") { 
          section="json";
        }
      }
      else if (section=="json") {
        section="ignore";
        String result = line.substring(1);
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
          if (strcmp(json_parsed["hasPassword"], "true") == 0) {
            Serial.println("***********Password is available :**************");
            String newSSID = json_parsed["ssid"];
            String newPassword = json_parsed["password"];
            Serial.println(newSSID);
            Serial.println(newPassword);
            Serial.println("-----------Cred Before saving--------------");
            newSSID.toCharArray(ssid_file, newSSID.length() + 1);
            newPassword.toCharArray(password_file, newPassword.length() + 1);
            Serial.println(ssid_file);
            Serial.println(password_file);
            saveCredentials();
            Serial.println("-------Cred after saving-------------");
            loadCredentials();
          }
          successLight();
          loop =0;
        }
        else {
          Serial.println("---------Status in not success----------");
          errorLights(2);
          loop =0;
        }
      }
    }else{
      delay(500);
    }
  }
  
  Serial.println("closing connection. ");
}
