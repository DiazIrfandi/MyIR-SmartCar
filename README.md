# 🌟 MyIR-SmartCar

[![Project Status](https://img.shields.io/badge/Project-Active-brightgreen?style=for-the-badge)](https://github.com/your-repo) [![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)](LICENSE) [![Platform](https://img.shields.io/badge/Platform-IoT-orange?style=for-the-badge)](https://www.espressif.com/en/products/socs/esp8266)

MyIR-SmartCar is an innovative IoT-based smart remote-controlled car that can be operated through an online app, providing convenience and fun in remote vehicle control. This project integrates Wi-Fi connectivity to seamlessly link the car to your home network.

---

## 🚗 **Features**

- Remote-controlled car operation via the **IrControl** mobile app.
- Initial configuration through a built-in hotspot.
- Wi-Fi network setup using a simple web interface.
- Support for multiple device types and configurations.

---

## 🧰 **Hardware Requirements**

[![ESP8266](https://img.shields.io/badge/ESP8266-Supported-blue?style=for-the-badge)](https://www.espressif.com/en/products/socs/esp8266) [![Li-ion Battery](https://img.shields.io/badge/Battery-Li--ion%203.7V-red?style=for-the-badge)](https://example.com)

- **ESP8266**: Main microcontroller for Wi-Fi and control logic.
- **Li-ion Batteries (3.7V, 2 units)**: Power supply for the system.
- **Motor Driver L298N**: Motor control.
- **2 GearBox Motors with Wheels**: For movement.
- **Buzzer (1 unit)**: Optional alert system.
- **Infrared Sensor - TCRT5000 (1 unit)**: For obstacle detection or advanced functionalities.

![image](https://raw.githubusercontent.com/DiazIrfandi/MyIR-SmartCar/refs/heads/main/SmartCar.png)

---

## 🚀 **Initial Setup**

### 1. **Powering the Car**
Ensure the car is powered by connecting the Li-ion batteries correctly to the ESP8266 and motor driver.

### 2. **Connecting to the Car's Hotspot**
- Turn on the car. The ESP8266 will start in hotspot mode.
- Connect your smartphone to the hotspot using the SSID displayed.

### 3. **Configuring Wi-Fi**
- Open a web browser and go to `http://192.168.4.1`.
- Enter your home Wi-Fi SSID and password.
- Save the configuration.

### 4. **Restarting the Car**
Press the reset button on the ESP8266 to connect the car to your home Wi-Fi.

---

## 📱 **Using the IrControl App**

[![Download App](https://img.shields.io/badge/Download-IrControl-blue?style=for-the-badge)](https://github.com/DiazIrfandi/MyIR-SmartCar/releases/tag/v1.0.0)

### 1. **Download the App**
- Go to the **Releases** section of this repository to download the latest version of the IrControl app.

### 2. **App Setup**
- Open the app and enter the car's IP address obtained after Wi-Fi configuration.
- Enter the **Device Token** provided during setup.
- Select the appropriate **Device Type** matching the release you downloaded.

---

## 🛠️ **Troubleshooting**

- **Car Does Not Connect to Wi-Fi**:
  - Ensure the SSID and password are entered correctly.
  - Check that the home Wi-Fi network is active and within range.

- **Unable to Control the Car**:
  - Confirm that the correct IP address is entered in the app.
  - Ensure the ESP8266 is powered and functioning properly.

- **Device Token Issues**:
  - Verify that the token entered in the app matches the one configured for the device.

---

## 🤝 **Contributions**
Contributions are welcome! Please feel free to fork this repository and submit pull requests.

---

## 📜 **License**
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

---

**Happy Driving with MyIR-SmartCar!** 🚗✨

