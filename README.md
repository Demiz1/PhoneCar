# PhoneCar
This project is about controlling an RC car with the gyro of a smartphone.

## Thecnical implementation
All smartphones today have orientation sensors, this data can be used (if allowed) by webpages. The idea is to use this gyro input from a webpage and controll a small remotecontrolled cars acceleration and steering angle.

The common RC vehicle are controlled with pwm signals to the drive motor and to the steering servo. Theses signals are easely controlled with a mictocontroller such as the [ESP-32](http://esp32.net/) which can also handle the task of hosting a small webpage aswell as a wifi access point a smartphone can connect to.

The ESP line of microcontrollers also allows for programming in the [Arduino](https://www.arduino.cc/) enviroment, allowing for a simpler implementation experiance.

## Current functionallity
- [x] Get a access point working.
- [x] Get gyro data from smartphone to ESP, a demo page is currently hosted at [this](https://gate-garden.com/gyroTest) address.
- [x] Get visualization tools for input verification working on the ESP-32.
Im using a small 64×32 OLED display for two bargraphs, showing the inputs the ESP would send to the cars motors.
- [ ] Buy a car and mount the ESP to it

The last thing i did on the project was to implement a queue feature, where multiple users could controll the car in turn. This feature derailed the project totaly as the scope creep was enourmus. I think the next thing to do is:

- [ ] Remove que functionallity
- [ ] Get the dev enviroment up again as a lot probably have been updated since the project stalled.
