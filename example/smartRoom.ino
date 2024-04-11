/*

                                            Smart Home (mqtt)
                Disclaimer: This code is for hobbyists for learning purposes. Not recommended for production use!!

                            # Dashboard Setup
                             - create account and login to the dashboard
                             - Create project.
                             - Create a node (e.g., for home- Room1 or study room).
                             - Create variables: temperature and humidity.
                            Note: Variable Identifier is essential; fill it accurately.

                            # Hardware Setup
                            - connect Led at GPIO pin 5(Marked D1 on the nodeMCU)

                  Note: The code is tested on the NodeMCU 1.0 board (ESP12E-Module)

                                                                                           Dated: 28-March-2024

*/
#include <Arduino.h>

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
const char *ssid = "Invesun_2.4GHz";  // Replace with your WiFi name
const char *pass = "Invesun123com#";  // Replace with your WiFi password

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
String fanStatus = "on";   // variable to store the status of the fan
String bulbStatus = "off";  // variable to store the status of the bulb

// Function Declarations
void connectToMQTT();                                                // function to connect with the anedya broker
void mqttCallback(char *topic, byte *payload, unsigned int length);  // funstion to handle call back
void setDevice_time();    
void beep();                                           // Function to configure the device time with real-time from ATS (Anedya Time Services)

// WiFi and MQTT client initialization
WiFiClientSecure esp_client;
PubSubClient mqtt_client(esp_client);

void setup() {
  Serial.begin(115200);  // Initialize serial communication with  your device compatible baud rate
  delay(1500);           // Delay for 1.5 seconds

  // Connect to WiFi network
    WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println();
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
        ESP.restart();
  }

  Serial.println();
  Serial.print("Connected, IP address: ");
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
      beep();
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + fan_commandId + "\",\"status\": \"success\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
    } else if (fanStatus == "off" || fanStatus == "OFF") {

      digitalWrite(fanPin, LOW);
      Serial.println("fan OFF");
       beep();
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
       beep();
      Serial.println("Bulb ON");
      String statusSuccessPayload = "{\"reqId\": \"\",\"commandId\": \"" + bulb_commandId + "\",\"status\": \"success\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
      mqtt_client.publish(statusTopic.c_str(), statusSuccessPayload.c_str());
    } else if (bulbStatus == "off" || bulbStatus == "OFF") {
      digitalWrite(bulbPin, LOW);
      Serial.println("Bulb OFF");
       beep();
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
//<---------------------------------------------------------------------------------------------------------------------------->
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
  Serial.println(str_res);

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
    }else{
          String statusReceivedPayload = "{\"reqId\": \"\",\"commandId\": \"" + commandId + "\",\"status\": \"failure\",\"ackdata\": \"\",\"ackdatatype\": \"\"}";
    mqtt_client.publish(statusTopic.c_str(), statusReceivedPayload.c_str());
    anedya_submitLog("","unknown command!!");
    }
  } else if (String(Response["errCode"]) == "0") {
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

void beep(){
  digitalWrite(buzzerPin, HIGH);
  delay(200);
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
