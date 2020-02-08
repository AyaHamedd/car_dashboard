# car_dashboard
A simple formula car dashboard reading values from sensors and displaying them on 16x2 character display 
using Atemga328p and Proteus simulation tool.


Sensors:

**1 - Speed**
    
The speed sensor is a hall effect sensor generating a variable frequency square wave with each **12** pulses representing a complete revolution
of the wheel. Max speed is 140 kilometers per hour.

**2 - RPM**

The RPM sensor (crankshaft sensor) is a hall effect sensor generating a variable frequency square wave with each **36** pulses representing
a complete revolution of the crankshaft. Max rpm is 15000 revolutions per minute.

**3 - Throttle Position**

The throttle Position sensor is a rotary potentiometer.

**4 - Steering Offset**

The Steering sensor is rotary potentiometer with the normal position of the steering (wheel straight ahead) at the middle of the travel.

**5 - Engine Temperature**
    
The engine temperature sensor is a linear sensor generating a voltage between 0v and 2.5v, with 0v representing -25 degrees C, and 2.5v 
representing +125 degrees C.



Circuit :

(/circuit.png)




