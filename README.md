This project is a modular weather station built using an ESP32. It is designed to measure various environmental data such as temperature, humidity, and any additional parameters supported by the connected sensors.
The project is organized into several independent modules, making it easy to extend, modify, or integrate new functionalities.
```
les_capteurs: handles the physical sensors and data acquisition.
```
```
MQTT: enables sending and receiving weather data to an MQTT broker for real-time monitoring.
```
```
mail sender: manages sending data reports or alert notifications via email.
```
```
interfacage: provides the interface between the sensors and the system’s processing logic.
```
The Qt interface we developed contains two tabs and allows users to load previously stored data, which can be visualized both as plotted curves and in table form. It also includes buttons to open and close the serial port, and provides the ability to select the desired port when multiple serial devices are connected.
<details> <summary><strong> the Qt interface</strong></summary> <br/>
<img width="881" height="631" alt="courbe" src="https://github.com/user-attachments/assets/b2ad3540-bdfa-4c8d-9463-b31cac4acb0d" />

<img width="884" height="634" alt="interface" src="https://github.com/user-attachments/assets/be3971a3-065e-4302-9a80-bfb87c8465c7" />



