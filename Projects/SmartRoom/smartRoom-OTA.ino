

#include <ESP8266WiFi.h>       // Ensure to include the ESP8266Wifi.h library, not the common library WiFi.
#include <PubSubClient.h>      //Include PubSubClient library to handle mqtt
#include <WiFiClientSecure.h>  // include WiFiClientSecure to establish secure connect .. anedya only allow secure connection
#include <ArduinoJson.h>       // Include the Arduino library to make json or abstract the value from the json
#include <TimeLib.h>           // Include the Time library to handle time synchronization with ATS (Anedya Time Services)
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

String regionCode = "ap-in-1";                                   // Anedya region code (e.g., "ap-in-1" for Asia-Pacific/India) | For other country code, visity [https://docs.anedya.io/device/intro/#region]
const char *deviceID = "2d5aa9ee-b68e-43a4-9821-430fe461d638";   // Fill your device Id , that you can get from your node description
const char *connectionkey = "e7633fe7c6810f3cb9365aec82e21fae";  // Fill your connection key, that you can get from your node description
// WiFi credentials

#ifndef STASSID
#define STASSID "Invesun_2.4GHz"
#define STAPSK "Invesun123com#"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// MQTT connection settings
const char *mqtt_broker = "device.ap-in-1.anedya.io";                       // MQTT broker address
const char *mqtt_username = deviceID;                                       // MQTT username
const char *mqtt_password = connectionkey;                                  // MQTT password
const int mqtt_port = 8883;                                                 // MQTT port
String responseTopic = "$anedya/device/" + String(deviceID) + "/response";  // MQTT topic for device responses
String errorTopic = "$anedya/device/" + String(deviceID) + "/errors";       // MQTT topic for device errors
String commandTopic = "$anedya/device/" + String(deviceID) + "/commands";   // MQTT topic for device commands

String statusTopic = "$anedya/device/" + String(deviceID) + "/commands/updateStatus/json";  // MQTT topic update status of the command
String timeRes,submitRes,commandId, fan_commandId, bulb_commandId;                                              // varibale to time response and store command Id
long long fan_responseTimer = 0, bulb_responseTimer = 0,submitLogTimer;                                    // timer to control flow
bool fan_processCheck = false;                                                              // check's, to make sure publish for process topic , once.
bool bulb_processCheck = false;

#define buzzerPin 12
#define fanPin 14
#define bulbPin 16
String fanStatus = "off";   // variable to store the status of the fan
String bulbStatus = "off";  // variable to store the status of the bulb

// Function Declarations
void connectToMQTT();                                                // function to connect with the anedya broker
void mqttCallback(char *topic, byte *payload, unsigned int length);  // funstion to handle call back
void setDevice_time();    
void beep(int DELAY);                                           // Function to configure the device time with real-time from ATS (Anedya Time Services)

// WiFi and MQTT client initialization
WiFiClientSecure esp_client;
PubSubClient mqtt_client(esp_client);

//-----------------------------------------------------------------------------//
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


// Notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4, 
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4,
  NOTE_F4, NOTE_G4, NOTE_F4
};

// Note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8,
  4, 4, 4
};

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(tone);
  }
}

void guitar(){
    for (int thisNote = 0; thisNote < sizeof(melody) / sizeof(melody[0]); thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    playTone(melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");


  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
  submitLogTimer=millis();

  esp_client.setInsecure();
  mqtt_client.setServer(mqtt_broker, mqtt_port);  // Set the MQTT server address and port for the MQTT client to connect to anedya broker
  mqtt_client.setKeepAlive(60);                   // Set the keep alive interval (in seconds) for the MQTT connection to maintain connectivity
  mqtt_client.setCallback(mqttCallback);          // Set the callback function to be invoked when MQTT messages are received
  connectToMQTT();                                // Attempt to establish a connection to the anedya broker
  mqtt_client.subscribe(responseTopic.c_str());   // subscribe to get response
  mqtt_client.subscribe(errorTopic.c_str());      // subscibe to get error
  mqtt_client.subscribe(commandTopic.c_str());

  setDevice_time();  // function to sync the the device time

  pinMode(fanPin, OUTPUT);
  pinMode(bulbPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  anedya_submitLog("", "Device Restarted!");

}

void loop() {
    if (!mqtt_client.connected()) {
    connectToMQTT();
  }
  if (millis() - fan_responseTimer > 100 && fan_processCheck && fan_commandId != "") {                                                                                 // condition block to publish the command processing message
    String statusProcessingPayload = "{\"reqId\": \"\",\"commandId\": \"" + fan_commandId + "\",\"status\": \"processing\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";  // payload
    //mqtt_client.publish(statusTopic.c_str(), statusProcessingPayload.c_str());                                                                                         // publish the command processing message
    fan_processCheck = false;
  } else if (millis() - fan_responseTimer >= 200 && fan_commandId != "") {  // condition block to publish the command success or failure message
    if (fanStatus == "on" || fanStatus == "ON") {
      digitalWrite(fanPin, HIGH);
      Serial.println("Fan ON");
      anedya_submitLog("", "FAN ON");
      beep(250);
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + fan_commandId + "\",\"status\": \"success\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
    } else if (fanStatus == "off" || fanStatus == "OFF") {

      digitalWrite(fanPin, LOW);
      Serial.println("Fan OFF");
      anedya_submitLog("", "FAN OFF");
       beep(250);
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + fan_commandId + "\",\"status\": \"success\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
    } else {
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + fan_commandId + "\",\"status\": \"failure\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
      Serial.println("Invalid Command");
    }
    fan_commandId = "";  // checks
  }

  //command to bulb
  if (millis() - bulb_responseTimer > 100 && bulb_processCheck && bulb_commandId != "") {                                                                               // condition block to publish the command processing message
    String statusProcessingPayload = "{\"reqId\": \"\",\"commandId\": \"" + bulb_commandId + "\",\"status\": \"processing\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";  // payload
    //mqtt_client.publish(statusTopic.c_str(), statusProcessingPayload.c_str());                                                                                          // publish the command processing message
    bulb_processCheck = false;
  } else if (millis() - bulb_responseTimer >= 200 && bulb_commandId != "") {  // condition block to publish the command success or failure message
    if (bulbStatus == "on" || bulbStatus == "ON") {
      digitalWrite(bulbPin, HIGH);
       beep(250);
      Serial.println("Bulb ON");
      anedya_submitLog("", "Bulb ON");
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + bulb_commandId + "\",\"status\": \"success\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
    } else if (bulbStatus == "off" || bulbStatus == "OFF") {
      digitalWrite(bulbPin, LOW);
      Serial.println("Bulb OFF");
      anedya_submitLog("", "Bulb OFF");
       beep(250);
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + bulb_commandId + "\",\"status\": \"success\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
    } else {
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + bulb_commandId + "\",\"status\": \"failure\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
      Serial.println("Invalid Command");
    }
    bulb_commandId = "";  // checks
  }
  mqtt_client.loop();
  ArduinoOTA.handle();

}
void connectToMQTT() {
  while (!mqtt_client.connected()) {
    const char *client_id = deviceID;
    Serial.print("Connecting to Anedya Broker....... ");
    if (mqtt_client.connect(client_id, mqtt_username, mqtt_password))  // checks to check mqtt connection
    {
      Serial.println("Connected to Anedya broker");
    } else {
      Serial.print("Failed to connect to Anedya broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" Retrying in 5 seconds.");
      delay(5000);
    }
  }
}
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  // Serial.print("Message received on topic: ");
  // Serial.println(topic);
  char res[150] = "";
  for (unsigned int i = 0; i < length; i++) {
    // Serial.print((char)payload[i]);
    //   Serial.print(payload[i]);
    res[i] = payload[i];
  }
  String str_res(res);
 // Serial.println(str_res);

  JsonDocument Response;
  deserializeJson(Response, str_res);
  if (Response["deviceSendTime"])  // block to get the device send time
  {
    timeRes = str_res;
  } else if (Response["command"])  // block to get the command
  {

    commandId = String(Response["id"]);
    String equipment = Response["command"].as<String>();
    String statusReceivedPayload = "{\"reqId\": \"\",\"commandId\": \"" + commandId + "\",\"status\": \"received\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
    mqtt_client.publish(statusTopic.c_str(), statusReceivedPayload.c_str());
    if (equipment == "fan" or equipment == "Fan") {

      fanStatus = String(Response["data"]);
      fan_responseTimer = millis();
      fan_commandId = String(Response["id"]);
      fan_processCheck = true;

    } else if (equipment == "bulb" or equipment == "Bulb") {
      bulbStatus = String(Response["data"]);
      bulb_responseTimer = millis();
      bulb_commandId = String(Response["id"]);
      bulb_processCheck = true;
    }else {
          String statusReceivedPayload = "{\"reqId\": \"\",\"commandId\": \"" + commandId + "\",\"status\": \"failure\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
    mqtt_client.publish(statusTopic.c_str(), statusReceivedPayload.c_str());
   // beep(1500);
   guitar();
    anedya_submitLog("","unknown command!!");
    }
  } else if (String(Response["errCode"]) == "0") {
        submitRes = str_res;
  } else  // block to debug errors
  {
    Serial.println(str_res);
  }
}
// Function to configure time synchronization with Anedya server
// For more info, visit [https://docs.anedya.io/devicehttpapi/http-time-sync/]
void setDevice_time() {
  String timeTopic = "$anedya/device/" + String(deviceID) + "/time/json";  // time topic wil provide the current time from the anedya server
  const char *mqtt_topic = timeTopic.c_str();
  // Attempt to synchronize time with Anedya server
  if (mqtt_client.connected()) {

    Serial.print("Time synchronizing......");

    boolean timeCheck = true;  // iteration to re-sync to ATS (Anedya Time Services), in case of failed attempt
    // Get the device send time

    long long deviceSendTime;
    long long timeTimer = millis();
    while (timeCheck) {
      mqtt_client.loop();

      unsigned int iterate = 2000;
      if (millis() - timeTimer >= iterate)  // time to hold publishing
      {
        Serial.print(".");
        timeTimer = millis();
        deviceSendTime = millis();

        // Prepare the request payload
        StaticJsonDocument<200> requestPayload;             // Declare a JSON document with a capacity of 200 bytes
        requestPayload["deviceSendTime"] = deviceSendTime;  // Add a key-value pair to the JSON document
        String jsonPayload;                                 // Declare a string to store the serialized JSON payload
        serializeJson(requestPayload, jsonPayload);         // Serialize the JSON document into a string
        // Convert String object to pointer to a string literal
        const char *jsonPayloadLiteral = jsonPayload.c_str();
        mqtt_client.publish(mqtt_topic, jsonPayloadLiteral);

      }  // if end

      if (timeRes != "")  // processed it got response
      {
        String strResTime(timeRes);

        // Parse the JSON response
        DynamicJsonDocument jsonResponse(100);      // Declare a JSON document with a capacity of 200 bytes
        deserializeJson(jsonResponse, strResTime);  // Deserialize the JSON response from the server into the JSON document

        long long serverReceiveTime = jsonResponse["serverReceiveTime"];  // Get the server receive time from the JSON document
        long long serverSendTime = jsonResponse["serverSendTime"];        // Get the server send time from the JSON document

        // Compute the current time
        long long deviceRecTime = millis();                                                                 // Get the device receive time
        long long currentTime = (serverReceiveTime + serverSendTime + deviceRecTime - deviceSendTime) / 2;  // Compute the current time
        long long currentTimeSeconds = currentTime / 1000;                                                  // Convert current time to seconds

        // Set device time
        setTime(currentTimeSeconds);  // Set the device time based on the computed current time
        Serial.println("\n synchronized!");
        timeCheck = false;
      }  // response check
    }
    // while loop end
  } else {
    connectToMQTT();
  }  // mqtt connect check end
}  // set device time function end

void beep(int DELAY){
  digitalWrite(buzzerPin, HIGH);
  delay(DELAY);
  digitalWrite(buzzerPin,LOW);
}


void anedya_submitLog(String reqID, String Log)
{
  boolean check = true;

  String strSubmitTopic = "$anedya/device/" + String(deviceID) + "/logs/submitLogs/json";
  const char *submitTopic = strSubmitTopic.c_str();
  while (check)
  {
    if (mqtt_client.connected())
    {

      if (millis() - submitLogTimer >= 2000)
      {

        submitLogTimer = millis();
        // Get current time and convert it to milliseconds
        long long current_time = now();                     // Get the current time
        long long current_time_milli = current_time * 1000; // Convert current time to milliseconds

        // Construct the JSON payload with sensor data and timestamp

        String strLog = "{\"reqId\":\"" + reqID + "\",\"data\":[{\"timestamp\":" + String(current_time_milli) + ",\"log\":\"" + Log + "\"}]}";
        const char *submitLogPayload = strLog.c_str();
        mqtt_client.publish(submitTopic, submitLogPayload);
      }
      mqtt_client.loop();
      if (submitRes != "")
      {
        // Parse the JSON response
        DynamicJsonDocument jsonResponse(100);    // Declare a JSON document with a capacity of 200 bytes
        deserializeJson(jsonResponse, submitRes); // Deserialize the JSON response from the server into the JSON document
        int errorCode = jsonResponse["errCode"];  // Get the server receive time from the JSON document
        if (errorCode == 0)
        {
          Serial.println("Log pushed to Anedya!!");
        }
        else
        {
          Serial.println("Failed to push!");
          Serial.println(submitRes);
        }
        check = false;
        submitLogTimer=5000;
      }
    }
    else
    {
      connectToMQTT();
    } // mqtt connect check end
  }
}

