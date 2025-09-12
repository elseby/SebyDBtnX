SebyDBtnX
====================

This is an open-source home automation project. The device allows you to control actuators on the Bticino myhome system. It allows you to send commands to the bus for lights, roller shutters, and CEN. Up to 10 commands can be set for each button. The button's LED indicates the status of the first command, and different color combinations can be selected.
No external software is required for programming, as it is accessible via a web page after activating the internal Wi-Fi server. In addition to the web page, a websocket channel is also available, allowing you to view frame traffic on the bus.
The board features the ESP32-S3FH4R2 SoC, which has 4MB of flash, Wi-Fi for programming and/or OTA firmware updates, and all the other peripherals needed for the project (UART, IRDA, RMT, etc.).
The bus interface is implemented using the STKNX chip, which also contains the switching power supply for the circuit.