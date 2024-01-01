# Cyclops: ESP-CAM Security Camera Project

Cyclops is an ESP-CAM-based security camera project with pan-tilt capabilities, a RESTful API, and various features for home security.

## Table of Contents

1. [EYE: ESP-CAM Overview](#eye)
   - [ESP-CAM Introduction](#esp-cam-introduction)
   - [Connection and Configuration](#connection-and-configuration)
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

### | ESP-CAM Introduction <a name="esp-cam-introduction"></a>

Ever wished your home security could be as sharp as the mythical Cyclops’ gaze? Well, say hello to Project Cyclops! This isn’t your run-of-the-mill surveillance setup; we’re talking about a smart, sassy security camera system that’s about to rock your world.
Cyclops isn’t just a name; it’s a vibe — a one-eyed wonder ready to redefine how we keep our spaces safe. Picture this: ESP-CAM, some nifty servo action, and a sprinkle of tech magic. Split into three cool sections: EYE, SKELETON, and BRAIN. Cyclops isn’t just a camera; it’s a whole security experience.
Let’s kick things off with the EYE. We’re diving deep into ESP-CAM territory, where technical specs meet image processing genius. This baby doesn’t just capture moments; it understands them. And yeah, we’re tossing those visuals your way, whether you’re sipping coffee at home or globe-trotting.
So, buckle up for a ride with Project Cyclops. It’s not just a security thing; it’s a lifestyle. Get ready for the future of surveillance, where tech meets street smarts. Welcome to the Cyclops crew — your personal guardians in the digital wild west!

[Read details from my Medium](https://medium.com/@atacanymc/home-security-cam-api-with-esp-part-1-dfaf1aa60980)

### | Connection and Configuration <a name="connection-and-configuration"></a>

In the Cyclops project, establishing a reliable connection for the ESP32-CAM module and configuring its settings are crucial steps to ensure seamless operation. This section provides a detailed overview of how to connect the ESP32-CAM to your system and configure essential parameters.

#### - Hardware Setup

Before configuring software settings, make sure the ESP32-CAM module is correctly connected to the pan-tilt mechanism and any additional components, such as servo motors and the buzzer. Check the wiring and power connections to ensure stability during operation.

#### - Wi-Fi Configuration

To enable the ESP32-CAM to connect to your local network, follow these steps:

1. Open the Cyclops web interface or use the RESTful API to access Wi-Fi configuration settings.
2. Scan for available Wi-Fi networks and select the desired network.
3. Enter the network's SSID (Service Set Identifier) and, if applicable, the password.
4. Save the configuration to allow the ESP32-CAM to establish a connection to the selected Wi-Fi network.

#### - Capture Preferences

Adjusting capture preferences is essential for tailoring the Cyclops camera to your specific needs. These preferences may include:

- **Capture Resolution:** Set the resolution for images captured by the ESP32-CAM.
- **Flash Control:** Configure whether the flash should be activated during image capture.
- **Auto-Save:** Choose whether captured images should be automatically saved to local storage.

#### - Additional Configuration Options

Explore additional configuration options based on the requirements of your security camera system. These may include:

- **Device Identification:** Assign a unique name or identifier to distinguish between multiple Cyclops devices on the same network.
- **Security Features:** Implement security measures, such as password protection or encryption, to secure the communication between Cyclops and the connected devices.

By carefully configuring these settings, you can optimize the performance of your ESP32-CAM-based Cyclops security camera and tailor it to meet specific project requirements. Always refer to the project documentation for any additional configuration options and best practices.

### | Image Transmission <a name="image-transmission"></a>

#### - Base64 Usage

Base64 is an encoding method that allows data to be safely represented within the ASCII character set. In devices like ESP32-CAM, it is commonly used to transmit or store image or file data securely.

#### - Encoding Process:

The ESP32 Arduino library includes the `base64` function for encoding data into Base64 format. Here's an example:

## 2. SKELETON: Pan-Tilt and Motion <a name="skeleton"></a>

### | Pan-Tilt Introduction <a name="pan-tilt-introduction"></a>

As we delve deeper into the core of Cyclops, our Home Security Cam API with Esp, the “Skeleton” segment emerges as the beating heart of motion and adaptability. In this installment, we take a closer look at how the pan-tilt mechanism and servo motors seamlessly integrate, transforming our security camera into an agile sentinel. Also I suggest you read the other parts too.

[Read details from my Medium](https://medium.com/@atacanymc/home-security-cam-api-with-esp-part-2-skeleton-e7110a234ea9)

### | Servo Motors <a name="servo-motors"></a>

In the part of Skeleton, servo motors emerge as the unsung architects of Cyclops’ motion repertoire. Their ability to intricately control the pan-tilt dance ensures a level of responsiveness that transcends the ordinary. These motors, while driving our camera’s mechanical choreography, also lay the groundwork for interactive functionalities. From subtle adjustments in response to detected motion to dynamic repositioning based on user commands, the versatility of servo motors adds an extra layer of intelligence to our security system.

### | ESP32Servo Library <a name="esp32servo-library"></a>

ESP32Servo.h is a library designed to facilitate the integration and control of servo motors specifically tailored for ESP32 microcontrollers. Servo motors, known for their precision in angular motion, are commonly employed in various projects ranging from robotics to DIY electronics.

```
#include <Arduino.h>
#include <ESP32Servo.h>

#define HORIZONTAL_PIN 12
#define VERTICAL_PIN 13

class Skeleton { // For 2-axis pan-tilt
    public:
      int horizontalAxisAngle = 0; // X
      int verticalAxisAngle = 0; // Y
      Servo horizontalAxis;
      Servo verticalAxis;

      Skeleton();
      void setAxis(char axis, int angle);
      int addAxis(char axis, int addAngle);

    private:
      void calibrate();
      void setHorizontalAngle(int angle);
      void setVerticalAngle(int angle);
};
```

## 3. BRAIN: RESTful API and System Control <a name="brain"></a>

### | WebServer Library <a name="webserver-library"></a>

 `<WebServer.h>` is a crucial header file used for developing web server applications on the ESP32 microcontroller platform. This library enables the deployment of your device as a web server, leveraging the built-in Wi-Fi capabilities of the ESP32. Web server applications are commonly employed in Internet of Things (IoT) projects and scenarios requiring remote device control. `<WebServer.h>` is designed to handle HTTP requests, serve dynamic content, create APIs, and, in general, make ESP32-based projects accessible over the web. This header file, combined with the robust processing capabilities of the ESP32, provides users with flexibility and customization options, allowing projects to become more interactive and accessible.

```
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "your-ssid";
const char *password = "your-password";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Hello, World!");
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Define server route
  server.on("/", HTTP_GET, handleRoot);

  // Start server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
```

### | mDNS (Multicast DNS) <a name="mdns-multicast-dns"></a>

`<mDNS.h>` is a library in the Arduino framework for ESP32 that allows you to implement mDNS (Multicast DNS) in your ESP32 projects. mDNS is a protocol that enables you to resolve domain names to IP addresses within a small local network without the need for a central DNS server. With mDNS, devices on the same network can discover and communicate with each other using human-readable domain names.

```
#include <WiFi.h>
#include <ESPmDNS.h>

const char *ssid = "your-ssid";
const char *password = "your-password";
const char *hostname = "esp32-mdns-example";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up mDNS
  if (MDNS.begin(hostname)) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up mDNS responder!");
  }
}

void loop() {
  // Your main code goes here

  delay(1000);
}
```

### | Memory and SPIFFS <a name="memory-and-spiffs"></a>

As we delve into the development phase, the SPIFFS library takes the center stage for managing two key aspects of our security system. Firstly, it serves as the repository for Wi-Fi configurations, ensuring seamless connectivity and adaptability to diverse network environments. The SPIFFS library allows us to securely store and retrieve these configurations, providing a flexible and user-friendly means for users to tailor the security camera to their network preferences.
In addition to configurations, SPIFFS plays a crucial role in managing the images captured by our security camera. By storing base64-encoded versions of these images, SPIFFS enhances accessibility and usability. Retrieving and presenting images in base64 format streamlines the integration with various applications and platforms, allowing users to effortlessly view and share captured moments from their security camera.

```
#include <FS.h>
#include <SPIFFS.h>

void setup() {
  Serial.begin(115200);
  if (SPIFFS.begin(true)) {
    Serial.println("SPIFFS mounted successfully");
  } else {
    Serial.println("SPIFFS failed");
  }
}
```

### | Voice Notifications with Buzzer <a name="voice-notifications-with-buzzer"></a>

In the vibrant symphony of our Home Security Cam API with ESP, Part 4 introduces a new crescendo in the form of “Voice.” This installment explores the power of sound through the utilization of the `tone()` function, as we integrate a buzzer to compose audible melodies of security. By harmonizing tones, our security system transcends the visual realm, now capable of delivering dynamic alerts, alarms, and notifications through distinct sound patterns. Join us on this melodic journey as we orchestrate an innovative approach to security, transforming our intelligent surveillance into a symphony of safety where the language of tones speaks volumes.

## 4. Project Setup <a name="project-setup"></a>

### | Dependencies <a name="dependencies"></a>

To set up and run this project, ensure that you have the following dependencies installed:

- [Arduino IDE](https://www.arduino.cc/en/software)
- [ESP32 board support](https://github.com/espressif/arduino-esp32#installation-instructions)

Make sure to install the ESP32 board support through the Arduino Board Manager.

Additionally, you may need specific libraries depending on the features you are implementing. Refer to the project documentation and relevant sections for detailed information on dependencies.

### | How to Run <a name="how-to-run"></a>

1. Open the Arduino IDE.
2. Set up your ESP32 board in the Arduino IDE by selecting the appropriate board and port.
3. Clone or download the project repository from GitHub.
4. Open the project in the Arduino IDE.
5. Configure any project-specific settings or constants.
6. Compile and upload the code to your ESP32 board.
7. Monitor the serial console for debugging and status messages.

Refer to the project documentation for any additional steps or specific instructions related to your project.

## 5. Conclusion <a name="conclusion"></a>

In conclusion, this project provides a foundation for implementing various functionalities on the ESP32-CAM platform, including but not limited to image capture, server communication, and device configuration. The modular structure allows for easy expansion and customization based on specific project requirements.

As you explore and extend the project, consider documenting any modifications, improvements, or unique use cases. This documentation will serve as a valuable resource for both your future reference and for anyone else interested in understanding or contributing to the project.

Feel free to contribute, share, and adapt this project to suit your needs. Happy coding!

