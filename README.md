# George the robot

This is the final project for the subject Digital Logic 2 in semester 5 in University of Alexandria Faculty of Engineering Computer and Communications major.

The project definitely could be refined better especially as we learnt more throughout the years.

George is a joke we had going in the team, so it felt fitting to name our robot George.

It is one of the hardest projects we had to do, because we had zero prior knowledge of ESP32, Arduino IDE, or anything.

I hope the documentation is satisfactory!!

## Table of Contents

1. [Materials Used](#materials-used)
2. [RemoteXY](#remotexy)
3. [Arduino IDE Code](#arduino-ide-code)

## Materials Used

- Robot platform 4WD: the robot body in black
  
- 4 Wheels: 4 wheels came with the platforms
  
- 4 DC 10A PWM Motor Controllers: motor for each wheel
  
- 2 Cytron SHIELD-MD10 DC Motor Driver Shields: 1 motor driver for the front 2 wheels and 1 for the back 2 wheels
  
- 2 ESP 32-S: one was used for all the main functions and one was used to connect to wifi
  
- HC-SR04 Ultrasonic Sensor Detector: gave the robot "eyes" and was used to measure distance to the lcd screen
  
- Character LCD Green 2×16 with Serial interface IIC/I2C Module: was used for the ultrasonic sensor, the smoke detector, or just to display a cute message
  
- Serial I2C LCD Daughter Board Module: used with the lcd screen
  
- MQ-2 Smoke Sensor Module: simple smoke detector
  
- AA Batteries and Battery holders: to power the entire robot
  
- 2 SG92R Micro Servo 180 Degree: we moved around the ultrasonic detector in all directions using a specific holder

## RemoteXY

To control our intricate little robot, we discovered a bluetooth/wifi connected application that we can use.

I unfortunately don't have the app right now, as I am adding this read me a year after the completion of this project, but I will do my best to explain how we've used it.

The mini app had a menu, including multiple options:
- smoke detector: turned on the smoke detector and displayed its readings on the lcd screen
- distance sensor: turned on the ultrasonic sensor and displayed its readings on the lcd screen
- dance!: walk around in squiggly lines
- walk: normal joy stick configurations
- talk: connects to the internet, input a sentence, and the speaker says out loud the sentence

## Arduino IDE Code

I won't go into much details, but I'll just explain our thought process through out the project.

In the main loop, each button had a number, and we used it to access each page accordingly (since remotexy technically doesnt have pages).

For the motors, since we are using a joy stick, we mapped each direction to apply into each of the motors for all the walking and dancing.

The lcd screen prints Hi! I'm George ^^ as soon as the robot starts up. If you happen to use any of the other detectors the screen will switch to their output and back again once you're done.

That is all mostly for the complicated uncommented parts of the code, the rest is pretty simple for anyone who has worked with arduino before.
