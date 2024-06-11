/*!
 * Control Mobius Devices
 *
 * Establishes a wifi and mqtt connection. 
 * Sets Mobius device scene using the device serial number and the Mobius scene
 * Mobius scenes are: int 0 though 65535
 * Default scenes are: "No Scene: 0", "Feed Mode: 1", "Battery Backup: 2", "All Off: 3", "Colour Cycle: 4", "Disco :5", "Thunderstorm: 6", "Cloud Cover: 7", "All On: 8", "All 50%: 9"
 * 
 * PREREQUISITES:
 * 1. MQTT server (tested working with Mosquitto v2.0.18)
 * 2. Home Assistant with MQTT integration and auto discovery enabled
 * 3. Arduino IDE
 * 4. esp32 (tested working with: ...)
 * 
 * INSTALL:
 * 1. Download package (link to github that includes MobiusMQTTGetScenes.ino, MobiusMQTTSetScene.ino, secrets.h, ArduinoSerialDeviceEventListner.cpp, MobiusSerialDecoder.h)
 * 2. Install EspMQTTClient by Patrick Lapointe in Arduino IDE
 * 5. Open MobiusMQTTSetScene.ino in Arduino IDE
 * 6. Edit secrets.h to insert your WiFi and MQTT values
 * 7. Ensure the board specs match the Arduino IDE settings in Tools > CPU Frequency : Flash Frequency : Upload Speed
 * 8. Compile and upload using the -> button in the Arduino IDE
 * 9. Monitor serial output in the Arduino IDE
 * 
 * USING:
 * 1. Change Control Set Scene in HA device to desired scene
 * 
 * This example code is released into the public domain.
 */

#include <esp_log.h>
#include <ESP32_MobiusBLE.h>
#include "ArduinoSerialDeviceEventListener.h"
#include "EspMQTTClient.h"
#include <string>
#include "secrets.h"

MobiusDevice deviceBuffer[30];       // Define a device buffer to hold found Mobius devices

// Configuration for wifi and mqtt
EspMQTTClient mqttClient(
  mySSID,                // Your Wifi SSID (in secrets.h)
  myPassword,            // Your WiFi key (in secrets.h)
  mqttServer,            // MQTT Broker server ip (in secrets.h)
  mqttUser,              // mqtt username Can be omitted if not needed (in secrets.h)
  mqttPass,              // mqtt pass Can be omitted if not needed (in secrets.h)
  "MobiusSetScene",      // Client name that uniquely identify your device
  1883                   // MQTT Broker server port
);

// wifi and mqtt connection established
void onConnectionEstablished()
{
  mqttClient.enableDebuggingMessages(); // Enable debugging messages sent to serial output

  // Set mqtt to be persistent
  mqttClient.enableMQTTPersistence();

  Serial.println("MQTT Connection established...\nSubscribing to topics...");

  // Listen to wildcard topic for scene changes to HA
  mqttClient.subscribe("homeassistant/select/mobius/scene/#", mobiusSetScene);
}


void mobiusSetScene(const String& topic, const String& message) {
  Serial.println("************************************************************");
  Serial.print("Called mobiusSetScene with Topic: ");
  Serial.print(topic);
  Serial.print(" and Scene: ");
  Serial.print(message);
  //for some reason topic.substring(34, 48).c_str() do not work in a single step but works if splitted
  String sub_S = topic.substring(34, 48);
  const char* serialNumber = sub_S.c_str();
  
  Serial.print(" and Serial: ");
  Serial.println(serialNumber);
  Serial.println("************************************************************");
   // Loop through devices till we match the serial number
  MobiusDevice device = deviceBuffer[0];
  int discovered = 0;
  while (discovered == 0){
    int count = 0;
    int scanDuration = 5; // in seconds
    while (!count) {
      count = MobiusDevice::scanForMobiusDevices(scanDuration, deviceBuffer);
    }

    for (int i = 0; i < count; i++) {
      device = deviceBuffer[i];
      Serial.printf("INFO: Checking device: %i", i);
      Serial.printf(" of %i\n", count);
      // Get manufacturer info
      std::string manuData = device._device->getManufacturerData();

      // Don't connect unless we have a serial number
      if (manuData.length() > 1){
        // serialNumber is from byte 11
        std::string serialNumberString = manuData.substr(11, manuData.length());
        char serialNumberIn[serialNumberString.length() + 1] = {};
        strcpy(serialNumberIn, serialNumberString.c_str());
        Serial.printf("INFO: Device serial number: %s", serialNumberIn);
        Serial.printf(" Serial number given: %s\n", serialNumber);
        if (strcmp(serialNumberIn, serialNumber) == 0){
          Serial.printf("INFO: MATCH!!\n");

          // Connect device
          if (!device.connect()){
            Serial.println("ERROR: Failed to connect to device");
            break;
          }
          else {
            Serial.println("INFO: Connected to device");
          }

          // Set scene
          if (!device.setScene(message.toInt())){
            Serial.println("ERROR: Failed to set device scene");
            break;
          }
          else {
            Serial.println("INFO: Successfully set device scene");
            // Disconnect
            device.disconnect();

            // Mark done
            discovered = 1;
          }
        }
      }
    }
  }
}

/*!
 * Main Setup method
 */
void setup() {
  // Connect the serial port for logs
  Serial.begin(115200);

  // Initialize the library with a useful event listener
  MobiusDevice::init(new ArduinoSerialDeviceEventListener());
}

/*!
 * Main Loop method
 */
void loop() {
  // Loop mqtt
  mqttClient.loop();
}