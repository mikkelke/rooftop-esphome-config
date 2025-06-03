# Rooftop ESPHome Configuration

This repository contains my ESPHome configurations for my smart home, "Rooftop". 

## Devices

### Living Room TV Lift
- **File:** `living-room-tv-lift.yaml`
- **Hardware:** ESP32
- **Device:** Vogel's MotionMount (NEXT 7355)
- **Purpose:** Controls the TV lift mechanism in the living room
- **Features:**
  - Motor control for smooth TV movement
  - Position feedback
  - Safety limits
- **Inspiration:** Based on [howm/homebridge-vogels-motionmount](https://github.com/howm/homebridge-vogels-motionmount)

### Intercomproxy
- **File:** `intercomproxy.yaml`
- **Hardware:** ESP32
- **Purpose:** Manages intercom system integration
- **Features:**
  - Intercom communication
  - Home Assistant integration
  - Status monitoring
- **Inspiration:** Based on [Mat931/esp32-doorbell-bus-interface](https://github.com/Mat931/esp32-doorbell-bus-interface)

### Smart Plugs
- **Files:** `athom-smart-plug-v3-*.yaml`
- **Hardware:** ESP8266 (Athom Smart Plug V3)
- **Purpose:** Power control for various devices
- **Features:**
  - Power monitoring
  - Energy tracking
  - Remote control

