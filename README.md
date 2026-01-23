# Fuzzy Logic
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

# Experiment
For the experimental setup an ESP32 microcontroller was attached to an RC Car. Controllinterfaces between the RC Car and the ESP32 contain of the speed, steering angle and four TCRT 5000L infrared sensors attached to the RC Car.  \todo{Bild vom Auto einfügen + referenz}
In both experiments the speed is constant with an pwm Input of 1560 which is the slowest possible forward speed for this RC Model with a corresponding speed of approximately \todo{versuchen zu messen} cm/s.
To messure the deviation two TCRT Sensors are attached to either side of the Car. The TCRT 5000L Sensors consists of an infrared LED and a phototransistor. The reflected light changes the voltage in the phototransistor and is thus translated into a measurable value. The measured value reflects whether the sensor is located over a white or a black surface. \todo{Grafik einfügen wie die Deviation gemessen wird}
By using two sensors on each side it is possible to detect whether the car deviates slightly or more from the track. 
Accordingly to this input for the deviation the steering angle is adjusted by sending a corresponding pwm signal to the servomotor which is responsible for the steering control.

![1000022815](https://github.com/user-attachments/assets/351e3c19-f353-4960-92b8-b2845b5c4295)
(TCRT picture: https://eckstein-shop.de/10StkTCRT5000InfrarotReflektierendeSensorTCRT5000LIR950mm5V3A)


## Input Membershipfunctions
<img width="702" height="600" alt="image" src="https://github.com/user-attachments/assets/299cdd2d-c915-4f78-ad2e-b6beb827f461" />

## Output Membershipfunctions
<img width="681" height="436" alt="image" src="https://github.com/user-attachments/assets/2786b50e-4b5f-4899-98c8-8735bb6e862c" />

## Defuzzyification Center of Area
For the Defuzzification the Area was separated into geometric figures. The Center of Area represents the crips Output value and is calculated as \sum{}{n/m}
<img width="521" height="679" alt="image" src="https://github.com/user-attachments/assets/14a36008-1bc5-4f44-8bcd-d86d8fba2a4f" />


