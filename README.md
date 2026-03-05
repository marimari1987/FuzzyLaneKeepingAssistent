I created this project as part of my bachelor's thesis. The goal was a comparison between the two mainly used Fuzzy Control Methods: The Mamdani Method and the Takagi Sugeno Kang (TSK or Sugeno) Method.

# Content
* Mamdani
* Mamdani_v2
* Sugeno
  
## Mamdani
* Fuzzy.cpp
* Fuzzy.h
* GeometricShapes.cpp
* GeometricShapes.h
* Mamdani.ino  
<p>Contains an older Version for the implementation of the Mamdani Mehtod for the Arduino IDE written in C++
The main class is Mamdani.ino where the ESP32 Controller get's setup for inputs, calculation and output of the pwm value. Fuzzy.cpp and Fuzzy.h are classes that use the Mamdani Method to calculate an output value in the shape of a pwm signal for steering control.
GeometricShapes.cpp and GeometricShapes.h are classes that are used for defuzzification without the need of integral calculations.
In this older version of the Mamdani Method only two Outputfunctions were used.
</p>

## Mamdani_v2
* Fuzzy.cpp
* Fuzzy.h
* GeometricShapes.cpp
* GeometricShapes.h
* Mamdani.ino
* fuzzy.py
<p>Contains a fully functioning implementation for the Memdani Method and additional a version written in python for theoretical simulations (fuzzy.py).
The main class is Mamdani.ino where the ESP32 Controller get's setup for inputs, calculation and output of the pwm value. Fuzzy.cpp and Fuzzy.h are classes that use the Mamdani Method to calculate an output value in the shape of a pwm signal for steering control.
GeometricShapes.cpp and GeometricShapes.h are classes that are used for defuzzification without the need of integral calculations.
Contrary to the previous version five output membershipfunctions are used for higher resulution in steering control.
</p>
## Sugeno
* Fuzzy.cpp
* Fuzzy.h
* Sugeno.ino
* fuzzy.py
<p>Contains a fully functioning implementation der the Sugeno Method on Arduino IDE (Sugeno.ino, Fuzzy.cpp, Fuzzy.h) and in python (fuzzy.py)
The main class is Sugeno.ino where the ESP32 Controller get's setup for inputs, calculation and output of the pwm value. Fuzzy.cpp and Fuzzy.h are classes that use the Sugeno Method to calculate an output value in the shape of a pwm signal for steering control.
</p>

# Background: Fuzzy Logic
With fuzzy logic it is possible to model membership degree of elements in a Set. 
e.g. For the question: "Is 15°C cold or hot?" it is possible to describe the containment of "15°C" in the Sets "warm" or "cold" in Membershipdegrees like: "15°C is 70% cold and 30% warm". 
It may be mentioned that the sum of the degrees doesn't neccessarily sum up tp 100%. 
With Fuzzy Logic it is also possible to model a deviation of a Car that tries to follow a Lane. Just with the Sets: "Normal, Small Deviation and Big Deivation" and finally computing it into a 
steering angle that can be used for a microcontroller.
For this experiment the Mamdani Method was used.

# Mamdani Method
E.Mamdani introduced a method to use fuzzy logic for fuzzy input terms as well as fuzzy output terms. From a crisp input value that was measured by a Sensor, the value get's fuzzified which means the membership
degrees for each Set are calculated. 
Following this a Rule Base with if...then statements is applied and the fitting rules are chosen. e.g "if it's too warm then slightly reduce the tmperature". 
With the so found rules, the Outputmembership degrees are calculated and then defuzzified - translated into a crisp output value. 
For the process of defuzzification different Methods can be chosen. 

# Sugeno Method
Contrary to the Mamdani Method the Method introduced by Takagi, Sugeno and Kang (also TSK Method) doesn't need a defuzzyfication with outputmembership functions. Instead rules in the TSK Method are given as "if x_1 then f(x)", "if x_2 then f(x_y).
Functions f(x_i) have a maximum degree of one. Which means they are either constant or linear. By this method it is possible to approximate complex mathemetical models.

# Experiment
For the experimental setup an ESP32 microcontroller was attached to an RC Car. Controllinterfaces between the RC Car and the ESP32 contain of the speed, steering angle and four TCRT 5000L infrared sensors attached to the RC Car.  \todo{Bild vom Auto einfügen + referenz}
In both experiments the speed is constant with an pwm Input of 1560 which is the slowest possible forward speed for this RC Model with a corresponding speed of approximately \todo{versuchen zu messen} cm/s.
To messure the deviation two TCRT Sensors are attached to either side of the Car. The TCRT 5000L Sensors consists of an infrared LED and a phototransistor. The reflected light changes the voltage in the phototransistor and is thus translated into a measurable value. The measured value reflects whether the sensor is located over a white or a black surface. \todo{Grafik einfügen wie die Deviation gemessen wird}
By using two sensors on each side it is possible to detect whether the car deviates slightly or more from the track. 
Accordingly to this input for the deviation the steering angle is adjusted by sending a corresponding pwm signal to the servomotor which is responsible for the steering control.

![1000022815](https://github.com/user-attachments/assets/351e3c19-f353-4960-92b8-b2845b5c4295)
(TCRT picture: https://eckstein-shop.de/10StkTCRT5000InfrarotReflektierendeSensorTCRT5000LIR950mm5V3A)

## Input Membershipfunctions
These Input Membership functions are used in both approaches:
<img width="702" height="600" alt="image" src="https://github.com/user-attachments/assets/299cdd2d-c915-4f78-ad2e-b6beb827f461" />

## Output Membershipfunctions (Mamdani)
<img width="444" height="347" alt="image" src="https://github.com/user-attachments/assets/21cbf4b3-05e0-45f3-8211-f85614e71a4e" />

## Defuzzyification Center of Area (Mamdani)
For the Defuzzification the Area was separated into geometric figures. The Center of Area represents the crips Output value and is calculated as \sum{}{n/m}
<img width="550" height="759" alt="image" src="https://github.com/user-attachments/assets/9a394d94-4342-4de0-90eb-c30cd9c15f2b" />

## Rules and Defuzzification for Sugeno
* Regel 1 : keiner der Sensoren berührt die Linie
  o.rule1 = (1.0 - m.outerLine) * (1.0 - m.innerLine);
* Regel 2: Nur der äußere Sensor berührt die Linie
  o.rule2 = (1.0 - m.outerLine) * m.innerLine;
* Regel 3: beide Sensoren berühren die Linie
  o.rule3 = m.outerLine * m.innerLine;
* Regel 4: nur der innere Sensor berührt die Linie
  o.rule4 = m.outerLine * (1.0 - m.innerLine);

Resulting in the defuzzyfication function:

output = $\frac{o.rule1 * noSteering + o.rule2 * lightSteering + o.rule3 * mediumSteering + o.rule4 * maxSteering}{o.rule1+o.rule2+o.rule3+o.rule4}$

with 

* noSteering = 0.0
* lightSteering = 60.0
* mediumSteering = 160.0
* maxSteering = 200.0
