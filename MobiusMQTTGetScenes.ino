/*!
 * Discover Mobius Devices
 *
 * Establishes a wifi and MQTT connection. 
 * Scans for Mobius devices and updates MQTT with a json message for Home Assistant auto discovery.
 * Creates devices in HA with entities for HA Current Scene sensor and HA Set Scene Control.
 * The current scene for each device is found and an update sent to MQTT. 
 * Stores found scenes in MQTT and retrieves them. Note the scenes will be reset on reboot of MQTT server
 * 
 * PREREQUISITES:
 * 1. MQTT server (tested working with Mosquitto v2.0.18)
 * 2. Home Assistant with MQTT integration and auto discovery enabled
 * 3. Arduino IDE
 * 4. esp32 (tested working with: ...)
 * 
 * INSTALL:
 * 1. Download (link to github that includes MobiusMQTTGetScenes.ino, MobiusMQTTSetScene.ino, secrets.h, ArduinoSerialDeviceEventListner.cpp, MobiusSerialDecoder.h)
 * 2. Install EspMQTTClient by Patrick Lapointe in Arduino IDE
 * 3. Install ArduinoJson by Beniot Blanchon in Arduino IDE
 * 4. Install AntiDelay by Martin Vichnal in Arduino IDE
 * 5. Open MobiusMQTTGetScenes.ino in Arduino IDE
 * 6. Edit secrets.h to insert your WiFi and MQTT values
 * 7. Ensure the board specs match the Arduino IDE settings in Tools > CPU Frequency : Flash Frequency : Upload Speed
 * 8. Compile and upload using the -> button in the Arduino IDE
 * 9. Monitor serial output in the Arduino IDE
 * 
 * USING:
 * 1. Ensure your esp32 is powered and near your Mobius devices
 * 2. Wait for around 10 min for all your Mobius devices to be discovered
 * 3. Rename your devices as required in HA Settings > Devices & services > MQTT > x devices > Edit Settings pencil icon top right > Update Name and Area
 * 4. Use the Mobius app to create scenes that you want to control from HA. Note when creating scenes to select the devices that apply to the scene. You can create scenes that apply to only one device.
 * 5. Execute the scenes from the Mobius app that you want to control from HA. The scenes will be discovered and added to the devices Current Scene sensor and Set Scene Input Select.
 * 
 * TODO:
 *  1. Requires no later version than 2.0.17 of the esp32 by Espressif Systems board library in Arduino IDE
 *  2. Speed up discovery to counter MQTT timeouts
 *  3. Consider design enhancement to include the default scenes 0 through 9 and then custom discovered scenes added to those
 *
 * This example code is released into the public domain.
 */

// Uncoment below to set scene auto discover to ON on boot
//#define DISCOVER_ON             

#include <esp_log.h>
#include <ESP32_MobiusBLE.h>
#include "ArduinoSerialDeviceEventListener.h"
#include "EspMQTTClient.h"
#include "WiFi.h"
#include <string>
#include "secrets.h"
#include <ArduinoJson.h>
#include <AntiDelay.h>
#include "MobiusSerialDecoder.h"

AntiDelay scanMobius(0);             // Create an AntiDelay object with initial 0 millis (run on first execution)
MobiusDevice deviceBuffer[30];       // Define a device buffer to hold found Mobius devices
JsonDocument mainJsonDoc;            // JSON Document for the main Serial#, Scenes
JsonDocument deviceSelectDoc;        // JSON Document to build the select
bool storedScenes = false;           // To check for retained Scenes JSON stored in the broker
unsigned long countMillis = 0;       // Counter for the retainet Scenes wait time
static long retainedTimeout = 90000; // Wait for up to 90 seconds for retained scenes before starting from scratch
float minutes = 0;                   // set minutes to 0 for continuous scan
char jsonOutput[1024];               //output variable to serialize the json
char jsonOutputFlash[1024];

#if defined(DISCOVER_ON)
  char discoverSwitch[4] = "ON";   
  bool SceneDiscFlag = true;
#else
  char discoverSwitch[4] = "OFF";   // Set default state for the scene discovery switch
  bool SceneDiscFlag = false;
#endif

// Configuration for wifi and mqtt
EspMQTTClient mqttClient(
  mySSID,                // Your Wifi SSID (see secrets.h)
  myPassword,            // Your WiFi key (see secrets.h)
  mqttServer,            // MQTT Broker server ip (see secrets.h)
  mqttUser,              // mqtt username Can be omitted if not needed (see secrets.h)
  mqttPass,              // mqtt pass Can be omitted if not needed (see secrets.h)
  "Mobius",              // Client name that uniquely identify your device
  1883                   // MQTT Broker server port
);

// Json mqtt template for home assistant auto discovery of mobius devices
char *jsonDiscoveryDevice = "{\"name\": \"%s\",\
  \"unique_id\": \"%s\",\
  \"icon\": \"%s\",\
  \"state_topic\": \"homeassistant/%s/mobius/%s/%s/state\",\
  \"force_update\": \"true\",\
  \"device\" : {\
  \"identifiers\" : [ \"%s\" ],\
  \"name\": \"%s\",\
  \"model\": \"%s\",\
  \"manufacturer\": \"%s\",\
  \"serial_number\": \"%s\"}\
}";

// Json mqtt template for device discovery switch
char* jsonSwitchDiscovery =  "{\
    \"name\":\"Discover Scenes\",\
    \"command_topic\":\"homeassistant/switch/mobiusBridge/set\",\
    \"state_topic\":\"homeassistant/switch/mobiusBridge/state\",\
    \"unique_id\":\"mobius01BLEBdge\",\
    \"device\":{\
      \"identifiers\":[\
        \"mobridge01ad\"\
      ],\
      \"name\":\"Mobius\",\
      \"manufacturer\": \"Team Down Under\",\
      \"model\": \"Mobius BLE Bridge\",\
      \"sw_version\": \"2024.05.03\"\
         }}";

// Json mqtt template for text input of scene
char *jsonDiscoveryDeviceText = "{\"name\": \"Enter Scene\",\
  \"unique_id\": \"%s_text\",\
  \"command_topic\": \"homeassistant/text/mobius/scene/%s\",\
  \"force_update\": \"true\",\
  \"device\": {\
    \"identifiers\": [\"%s\"],\
    \"name\": \"%s\",\
    \"model\": \"%s\",\
    \"manufacturer\": \"%s\",\
    \"serial_number\": \"%s\",\
    \"sw_version\": \"%s\"}\
}";

// Json mqtt template for folding device scenes
char* jsonTextDiscovery =  "{\"name\":\"Scenes JSON\",\"command_topic\":\"homeassistant/text/mobiusBridge/set\",\"state_topic\":\"homeassistant/text/mobiusBridge/state\",\"unique_id\":\"mobius01BLEBdge\",\
\"device\":{\"identifiers\":[\"mobridge01ad\"],\"name\":\"Mobius\",\"manufacturer\": \"Team Down Under\",\"model\": \"Mobius BLE Bridge\",\"sw_version\": \"2024.05.03\"}}";

// Callback for when wifi and mqtt connection established
void onConnectionEstablished()
{
  // Set keepalive (default is 15sec)
  mqttClient.setKeepAlive(120);

  // Set mqtt to be persistent
  mqttClient.enableMQTTPersistence();

  Serial.println("MQTT Connection established...\nSubscribing to topics...");

  // Listen to the retained Scenes JSON 
  mqttClient.subscribe("homeassistant/text/mobiusBridge/state", mobiusGetScene);

  // Listen to device discovery switch changes
  mqttClient.subscribe("homeassistant/switch/mobiusBridge/set", mobiusSceneSwitch);

}

/********************************************************************************
 *
 *  Retrieve the retained Scenes JSON file from MQTT Broker, ideally just after boot.
 *
 ********************************************************************************/
void mobiusGetScene(const String& topic, const String& message) {
  if (message.length() > 0) {
    strcpy(jsonOutput, message.c_str());

    DeserializationError error = deserializeJson(mainJsonDoc, jsonOutput);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
    } else {
      storedScenes = true;
      char jsonOutputHA[1024];
      if (!mainJsonDoc.isNull()){
        serializeJson(mainJsonDoc, jsonOutputHA);
        // MarkW commented this out as it creates an mqtt loop. Ask Paulo
        //mqttClient.publish("homeassistant/text/mobiusBridge/state", jsonOutputHA, true);
      }
      Serial.println("************************************************************");
        Serial.print("***         RECEIVED Scenes JSON from MQTT BROKER        ***\n***                         ");
      Serial.println(jsonOutput);
      Serial.println("************************************************************");
    }
  }
  else {
    storedScenes = false;
  }
}

void mobiusSceneSwitch(const String& sceneDiscovery) {
  if (sceneDiscovery.length() > 0) {
    if (sceneDiscovery == "ON") {
      SceneDiscFlag = true;
      storedScenes = true;  //If triggering scene discovery manually, set storedScenes to true to allow logic to run and not wait for timeout

      waitFunction(1000);

      mqttClient.publish("homeassistant/switch/mobiusBridge/state", "ON", true);
    } else {
      SceneDiscFlag = false;

      waitFunction(1000);

      mqttClient.publish("homeassistant/switch/mobiusBridge/state", "OFF", true);
    }
    //reset timer
    scanMobius.setInterval(0);
    scanMobius.reset();
  }
};

/*!
 * Main Setup method
 */
void setup() {
  // Connect the serial port for logs
  Serial.begin(115200);

  // Connect to wifi and mqtt server
  while(!mqttClient.isConnected()){mqttClient.loop();};

  mqttClient.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  mqttClient.enableLastWillMessage("homeassistant/mobius/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  
  // Increase default packet size for HA mqtt json messages
  mqttClient.setMaxPacketSize(10000);

  // Initialize the library with a useful event listener
  MobiusDevice::init(new ArduinoSerialDeviceEventListener());

  //Publish basic MQTT controls once after boot
  mqttClient.publish("homeassistant/switch/mobiusBridge/config", jsonSwitchDiscovery);

  // delaying without sleeping
  waitFunction(1000);

  mqttClient.publish("homeassistant/text/mobiusBridge/config", jsonTextDiscovery);

  // delaying without sleeping
  waitFunction(1000);

  mqttClient.publish("homeassistant/switch/mobiusBridge/state", discoverSwitch);

  char jsonOutputHA[1024];
  if (!mainJsonDoc.isNull())
    serializeJson(mainJsonDoc, jsonOutputHA);

  // delaying without sleeping
  waitFunction(1000);
 
  if (sizeof(jsonOutputHA) > 0)
    mqttClient.publish("homeassistant/text/mobiusBridge/state", jsonOutputHA, true);
  else
    mqttClient.publish("homeassistant/text/mobiusBridge/state", "");
}

/*!
 * Main Loop method
 */
void loop() {
  // Loop mqtt and wifi
  mqttClient.loop();

  if (storedScenes){
    //If retained Scenes retrieved from broker, proceed
    if (scanMobius) {
      //Run Mobius routine every x minutes defined below in [float minutes = ]
      if (!scanMobius.isRunning()){
        scanMobius.setInterval(minutes*60000);
      }

      // Get number of mobius devices
      MobiusDevice mobiusBLEdevice = deviceBuffer[0];
      int count = 0;

      int scanDuration = 2; // in seconds
      while (!count) {
        count = MobiusDevice::scanForMobiusDevices(scanDuration, deviceBuffer);
      }

      // Loop through each device found, autoconfigure home assistant with the device, and update the current scene of the device
      for (int i = 0; i < count; i++) {
        mobiusBLEdevice = deviceBuffer[i];

        // Get manufacturer info
        std::string manuData = mobiusBLEdevice._device->getManufacturerData();

        // Connect, get serialNumber and current scene
        Serial.printf("INFO: Connect to device number: %i of ", i);
        Serial.printf("%i \n", count);

        // Connect always fails if no manuData.
        if (manuData.length() > 1){
        if(mobiusBLEdevice.connect()) {
          //Serial.printf("INFO: Connected to: %s\n", mobiusBLEdevice._device->toString().c_str());
        
          // Get the devices mac address. Note that this changes every reboot so likely not useful
          // Removing this addressString seems to crash the esp32...
          std::string addressString = mobiusBLEdevice._device->getAddress().toString();
          //char deviceAddress[addressString.length() + 1] = {};
          //strcpy(deviceAddress, addressString.c_str());
          //Serial.printf("INFO: Device mac address is: %s\n", deviceAddress);

          // Get device details
          const char* fwRev = mobiusBLEdevice.getFWRev().c_str();
          const char* Manufa = mobiusBLEdevice.getManufName().c_str();
          const char* serialNumber = mobiusBLEdevice.getSerialNumber().c_str();
          const char* modelName = getModelName(mobiusBLEdevice.getSerialNumber());
          
          // Home Assistant autodiscovery
          // Text input for scene
          char jsonText[512];
          sprintf(jsonText, jsonDiscoveryDeviceText, serialNumber, serialNumber, serialNumber, serialNumber, modelName, Manufa, serialNumber, fwRev);
          Serial.printf("INFO: Device Text Discovery Message:%s\n", jsonText);
          char deviceTextDiscoveryTopic[100];
          sprintf(deviceTextDiscoveryTopic, "homeassistant/text/mobius/%s/config", serialNumber);
          Serial.printf("INFO: Device Text Discovery Topic: %s\n", deviceTextDiscoveryTopic);
          mqttClient.publish(deviceTextDiscoveryTopic, jsonText);

          // Substitute serialNumber into jsonDiscoveryDevice
          // Substitutions in order: name, unique_id, "icon", discovery_component (e.g. sensor ), sensor_topic (e.g. scene), serialNumber, "sensorType"
          char json[512];
          sprintf(json, jsonDiscoveryDevice, "Current Scene", serialNumber, "mdi:pump", "sensor", serialNumber, "scene", serialNumber, serialNumber, modelName, Manufa, serialNumber, fwRev);
          //Serial.println(json);
          //sprintf(json, jsonDiscoveryDevice, serialNumber, serialNumber, serialNumber, deviceAddress, serialNumber);
          //Serial.printf("INFO: Device discovery message:%s\n", json);
          char deviceDiscoveryTopic[100];
          sprintf(deviceDiscoveryTopic, "homeassistant/sensor/mobius/%s/config", serialNumber);
          //Serial.printf("INFO: Device Discovery Topic: %s\n", deviceDiscoveryTopic);
          //mqttClient.publish(deviceDiscoveryTopic, json);

          char deviceTopic[100];
          sprintf(deviceTopic, "homeassistant/sensor/mobius/%s/scene/state", serialNumber);

          // Create scene select input
          deviceSelectDoc["name"] = "Set Scene";

          char uniqueID[25];
          strcpy(uniqueID, serialNumber);
          strcat(uniqueID, "_select");

          deviceSelectDoc["unique_id"] = uniqueID;

          char cmdTopic[50];
          strcpy(cmdTopic, "homeassistant/select/mobius/scene/");
          strcat(cmdTopic, serialNumber);

          deviceSelectDoc["command_topic"] = cmdTopic;

          deviceSelectDoc["force_update"] = "true";

          JsonArray devOptions = deviceSelectDoc["options"].to<JsonArray>();
          if (!mainJsonDoc.isNull()){
            //if Scenes JSON is not empty, check if serial exists
            if (mainJsonDoc.containsKey(serialNumber)) {
              //If serial exists, get the scenes array
              Serial.println("Serial Exists, moving to check scene");

              Serial.println("Existing scenes for this Serial");

              //create an array with all scenes for the Serial
              JsonArray scenesJson = mainJsonDoc[serialNumber].as<JsonArray>();

              for (JsonVariant value : scenesJson) {
                //loop all scenes already in json and look for current scene
                char intToChar[6];
                itoa(value.as<int>(), intToChar, 10);   // value.as<const char*>() wasn't working properly so changed to itoa
                devOptions.add(intToChar);
              }
            }
          }
        
          if (devOptions.size() == 0) {
            //if Scenes JSON has no scenes for this serial, add default scenes
            for (int i = 1; i <= 9; i++) {
              char intToChar[6];
              itoa(i, intToChar, 10);
              devOptions.add(intToChar);
            }          
          }

          JsonObject deviceJson = deviceSelectDoc["device"].to<JsonObject>();
          deviceJson["identifiers"][0] = serialNumber;
          deviceJson["name"] = serialNumber;

          char jsonSelect[512];

          deviceSelectDoc.shrinkToFit();  // optional

          serializeJson(deviceSelectDoc, jsonSelect);

   
          // Create device discovery topic using serialNumber as unique key
          char deviceSelectDiscoveryTopic[100];
          sprintf(deviceSelectDiscoveryTopic, "homeassistant/select/mobius/%s/config", serialNumber);
          
          // Get current scene
          uint16_t sceneId = mobiusBLEdevice.getCurrentScene();
          char sceneString[8];
          dtostrf(sceneId, 2, 0, sceneString);

          if (SceneDiscFlag) {
            mainJsonDoc.shrinkToFit();  // optional
            serializeJson(mainJsonDoc, jsonOutput);

            //search for the serial in the jsondocument
            bool hasSerial = mainJsonDoc.containsKey(serialNumber);

            if (hasSerial) {
              //create an array with all scenes for the Serial
              JsonArray scenesJson = mainJsonDoc[serialNumber].as<JsonArray>();
              bool hasScene = false;
              for (JsonVariant value : scenesJson) {
                //loop all scenes already in json and look for current scene
                Serial.println(value.as<int>());
                if (value.as<int>() == sceneId) {
                  Serial.println("Scene Exists, exiting the loop");
                  //Scene exists, set hasScene to true and break from loop
                  hasScene = true;
                  break;
                }
              }
              if (!hasScene) {
                Serial.println("Scene does NOT Exist, adding scene");
                //If current scene not in json document for the serial, add scene 
                scenesJson.add(sceneId);
              }
            }
            else {
              Serial.println("Serial Does NOT Exist, adding Serial and scene");

              //if serial does not exist, add serial and an empty scene array
              JsonArray newSceneJson = mainJsonDoc[serialNumber].to<JsonArray>();
              //Then add scene to the array
              newSceneJson.add(sceneId);
              //newSceneJson.add(1974);
              Serial.println("Serial and scene added..");
            }            

            //Process below to serialize and deserialize every each Mobius device to ensure the jsonDoc has all the items (Serial and its scenes)
            mainJsonDoc.shrinkToFit();  // optional
            serializeJson(mainJsonDoc, jsonOutput);
            DeserializationError error = deserializeJson(mainJsonDoc, jsonOutput);

            if (error) {
              Serial.print("deserializeJson() failed: ");
              Serial.println(error.c_str());
            }

            /****************************************************************************************************
            *  TO DO: 
            *
            *      Validate the json output with flash and only publish to MQTT Broker if different
            *
            *      jsonOutputFlash is representing the data stored inESP32 flash memory
            *
            *
            ****************************************************************************************************/
            /* if (jsonOutputFlash != jsonOutput) {
              Serial.print("Updated JSON from: ");
              Serial.println(jsonOutputFlash);
              Serial.print(" to : ");
              Serial.println(jsonOutput);

              strcpy(jsonOutputFlash, jsonOutput);
              mqttClient.publish("homeassistant/text/mobiusBridge/state", jsonOutputFlash, true);
            }
            */
          }

          // Do the publish last and delay execution 2 seconds
          mqttClient.publish(deviceDiscoveryTopic, json);
          mqttClient.publish(deviceSelectDiscoveryTopic, jsonSelect);
          mqttClient.publish(deviceTopic, sceneString);

          // Disconnect
          mobiusBLEdevice.disconnect();

          if (SceneDiscFlag) {
            Serial.println("****************************************");
            Serial.println("Print Json Document again, after processing Serial#/Scene");
            Serial.println(jsonOutput);
          }
        }
        }
        else {
          Serial.println("ERROR: Failed to connect to device");
        }

      }
      Serial.println("================================================================================");
      Serial.println("Print full Json Document, after processing all devices");
      if (!mainJsonDoc.isNull()){
        mainJsonDoc.shrinkToFit();  // optional
        serializeJson(mainJsonDoc, jsonOutput);
      }
      Serial.println(jsonOutput);  
    }
  } else {
    //If not retrieved, wait for retrieved scenes before resuming activities
    if (countMillis == 0) {
      countMillis = millis();
    } else {
      if ((retainedTimeout < (millis() - countMillis)) ) {
        Serial.println("Did not received message from broker yet, timeout reached.");
        storedScenes = true;
      }
    }
  }
}

void waitFunction(int intTimer){
  unsigned long startMillis = millis();
  while (intTimer > (millis() - startMillis)) {}  
}