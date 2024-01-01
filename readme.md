# Cyclops: ESP-CAM Security Camera Project

Cyclops is an ESP-CAM-based security camera project with pan-tilt capabilities, a RESTful API, and various features for home security.

## Table of Contents

1. [EYE: ESP-CAM Overview](#eye)
   - [ESP-CAM Introduction](#esp-cam-introduction)
   - [Connection and Configuration](#connection-and-configuration)
   - [Image Processing](#image-processing)
   - [Image Transmission](#image-transmission)

2. [SKELETON: Pan-Tilt and Motion](#skeleton)
   - [Pan-Tilt Introduction](#pan-tilt-introduction)
   - [Servo Motors](#servo-motors)
   - [ESP32Servo Library](#esp32servo-library)

3. [BRAIN: RESTful API and System Control](#brain)
   - [WebServer Library](#webserver-library)
   - [mDNS (Multicast DNS)](#mdns-multicast-dns)
   - [Memory and SPIFFS](#memory-and-spiffs)
   - [Voice Notifications with Buzzer](#voice-notifications-with-buzzer)

4. [Project Setup](#project-setup)
   - [Dependencies](#dependencies)
   - [How to Run](#how-to-run)

5. [Conclusion](#conclusion)

## 1. EYE: ESP-CAM Overview <a name="eye"></a>

### ESP-CAM Introduction <a name="esp-cam-introduction"></a>

Ever wished your home security could be as sharp as the mythical Cyclops’ gaze? Well, say hello to Project Cyclops! This isn’t your run-of-the-mill surveillance setup; we’re talking about a smart, sassy security camera system that’s about to rock your world.
Cyclops isn’t just a name; it’s a vibe — a one-eyed wonder ready to redefine how we keep our spaces safe. Picture this: ESP-CAM, some nifty servo action, and a sprinkle of tech magic. Split into three cool sections: EYE, SKELETON, and BRAIN. Cyclops isn’t just a camera; it’s a whole security experience.
Let’s kick things off with the EYE. We’re diving deep into ESP-CAM territory, where technical specs meet image processing genius. This baby doesn’t just capture moments; it understands them. And yeah, we’re tossing those visuals your way, whether you’re sipping coffee at home or globe-trotting.
So, buckle up for a ride with Project Cyclops. It’s not just a security thing; it’s a lifestyle. Get ready for the future of surveillance, where tech meets street smarts. Welcome to the Cyclops crew — your personal guardians in the digital wild west!

[Read details from my Medium](https://medium.com/@atacanymc/home-security-cam-api-with-esp-part-1-dfaf1aa60980)

### Connection and Configuration <a name="connection-and-configuration"></a>

In the Cyclops project, establishing a reliable connection for the ESP32-CAM module and configuring its settings are crucial steps to ensure seamless operation. This section provides a detailed overview of how to connect the ESP32-CAM to your system and configure essential parameters.

## 1.1. Hardware Setup

Before configuring software settings, make sure the ESP32-CAM module is correctly connected to the pan-tilt mechanism and any additional components, such as servo motors and the buzzer. Check the wiring and power connections to ensure stability during operation.

## 1.2. Wi-Fi Configuration

To enable the ESP32-CAM to connect to your local network, follow these steps:

1. Open the Cyclops web interface or use the RESTful API to access Wi-Fi configuration settings.
2. Scan for available Wi-Fi networks and select the desired network.
3. Enter the network's SSID (Service Set Identifier) and, if applicable, the password.
4. Save the configuration to allow the ESP32-CAM to establish a connection to the selected Wi-Fi network.

## 1.3. Capture Preferences

Adjusting capture preferences is essential for tailoring the Cyclops camera to your specific needs. These preferences may include:

- **Capture Resolution:** Set the resolution for images captured by the ESP32-CAM.
- **Flash Control:** Configure whether the flash should be activated during image capture.
- **Auto-Save:** Choose whether captured images should be automatically saved to local storage.

## 1.4. Additional Configuration Options

Explore additional configuration options based on the requirements of your security camera system. These may include:

- **Device Identification:** Assign a unique name or identifier to distinguish between multiple Cyclops devices on the same network.
- **Security Features:** Implement security measures, such as password protection or encryption, to secure the communication between Cyclops and the connected devices.

By carefully configuring these settings, you can optimize the performance of your ESP32-CAM-based Cyclops security camera and tailor it to meet specific project requirements. Always refer to the project documentation for any additional configuration options and best practices.

### Image Processing <a name="image-processing"></a>

...

### Image Transmission <a name="image-transmission"></a>

...

## 2. SKELETON: Pan-Tilt and Motion <a name="skeleton"></a>

### Pan-Tilt Introduction <a name="pan-tilt-introduction"></a>

...

### Servo Motors <a name="servo-motors"></a>

...

### ESP32Servo Library <a name="esp32servo-library"></a>

...

## 3. BRAIN: RESTful API and System Control <a name="brain"></a>

### WebServer Library <a name="webserver-library"></a>

...

### mDNS (Multicast DNS) <a name="mdns-multicast-dns"></a>

...

### Memory and SPIFFS <a name="memory-and-spiffs"></a>

...

### Voice Notifications with Buzzer <a name="voice-notifications-with-buzzer"></a>

...

## 4. Project Setup <a name="project-setup"></a>

### Dependencies <a name="dependencies"></a>

...

### How to Run <a name="how-to-run"></a>

...

## 5. Conclusion <a name="conclusion"></a>

...

