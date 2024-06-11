# MobiusMQTT
Control Echotech Mobius devices from Home Assistant.

Based on [esp32-MobiusBLE by treesta1ker <link>](https://github.com/treesta1ker/esp32-MobiusBLE)

Discover and control any device available in the Mobius app from Home Assistant.

Tested Echotech devices include:
  * Vortech
  * Versa
  * Radion
  * AI Prime
  * Nero

Typical use cases include:
  * Trigger feeding scenes
  * Auto water change with Versas
  * Supplment control using Versas from manual or automated testing (e.g. Reefbot)
  * Lighting scenes

Currently requires two esp32 devices. One to discover and one to control. Intending to consolidate functionality on one esp32.

Discover Mobius Devices
  * Establishes a wifi and MQTT connection. 
  * Scans for Mobius devices and updates MQTT with a json message for Home Assistant auto discovery.
  * Creates devices in HA with entities for HA Current Scene sensor and HA Set Scene Control.
  * The current scene for each device is found and an update sent to MQTT. 
  * Stores learned scenes in MQTT and retrieves them. Note the learned scenes will be reset on reboot of MQTT server

Control Mobius Devices
  * Sets Mobius device scene using the device serial number and the Mobius scene
  * Mobius scenes are: int 0 though 65535
  * Default scenes are: "No Scene: 0", "Feed Mode: 1", "Battery Backup: 2", "All Off: 3", "Colour Cycle: 4", "Disco :5", "Thunderstorm: 6", "Cloud Cover: 7", "All On: 8", "All 50%: 9"

### PREREQUISITES:
  1. MQTT server (tested working with Mosquitto v2.0.18)
  2. Home Assistant with MQTT integration and auto discovery enabled
  3. Arduino IDE
  4. 2 x esp32 (tested working with: ESP32-WROOM-32D). One esp32 for GetScenes and one for SetScene.
  
### INSTALL:
  GetScenes:
  1. Download this archive and open MobiusMQTTGetScenes.ino in Arduino IDE.
  2. Install EspMQTTClient by Patrick Lapointe in Arduino IDE.
  3. Install ArduinoJson by Beniot Blanchon in Arduino IDE.
  4. Install AntiDelay by Martin Vichnal in Arduino IDE.
  5. Install NimBLE-Arduino by h2zero in Arduino IDE.
  6. Edit secrets.h to insert your WiFi and MQTT values.
  7. Ensure the board specs match the Arduino IDE settings in Tools > CPU Frequency : Flash Frequency : Upload Speed.
  8. Install esp32 by Espressif System board library version 2.0.17. Later versions won't work.
  9. Compile and upload using the -> button in the Arduino IDE.
  10. Monitor serial output in the Arduino IDE.

  SetScene:
  1. Plug in the second esp32
  2. Open MobiusMQTTSetScene.ino
  3. Complie and upload
     
### USING:
  1. Ensure your esp32 is powered and near your Mobius devices.
  2. Wait for around 10 min for all your Mobius devices to be discovered in HA.
  3. Rename your devices as required in HA Settings > Devices & services > MQTT > x devices > Edit Settings pencil icon top right > Update Name and Area. Rename all entities.
  4. Use the Mobius app to create scenes that you want to control from HA. Note when creating scenes to select the devices that apply to the scene. You can create scenes that apply to only one device. As an example, you could create a scene for one Versa dosing head to deliver 5ml of Calcium over 5min. 
  5. Execute the scenes from the Mobius app that you want to control from HA. The scenes will be discovered and added to the devices Current Scene sensor and Set Scene Input Select.
  6. Control Mobius devices from HA using the Set Scene Controls.
  7. Create HA Automations to automate your aquarium.
