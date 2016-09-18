#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

const char* ssid     = "dlink";  
const char* password = "oahf-fx9e-vimo";

const char* host     = "www.raghavendramanandi.site88.net"; // Your domain  
String path          = "/request.php";
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

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".---- Inside While in setup -------");
    Serial.println(WiFi.status());
    if(WiFi.status() == WL_NO_SSID_AVAIL){
      Serial.println("########in if block#######");
      if(loadCredentials() == 1){
        ssid     = ssid_file;
        password = password_file;
        WiFi.begin(ssid, password);
      }
    }else{
      errorLights();
    }
  }

  Serial.println("WiFi connected to: ");  
  Serial.print(ssid);
  Serial.print(password);
  Serial.println("IP address: " + WiFi.localIP());
}

void errorLights(){
  Serial.println("errorLights");
}

void successLight(){
  Serial.println("successLight");
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

  if(strlen(password)>0){
    return 1;
  }
  return 0;
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

        //Serial.println(json_parsed["status"]);
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
        }
        else {
          Serial.println("-------no status-------------");
          digitalWrite(pin, LOW);
          Serial.println("led off");
          errorLights();
        }
      }
      loop =0;
    }else{
      delay(500);
    }
  }
  
  Serial.print("closing connection. ");
}