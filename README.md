# UART Servo Library for Arduino
## Installation
[Here](https://www.arduino.cc/en/Guide/Libraries "Here") is a reference to install a third-party library into our Arduino IDE.
## How to Use It?
### Initialization
#### Create an Object
Create an object of UARTServo after include [UARTServo.h](./src/UARTServo/UARTServo.h).
```cpp
#include "UARTServo.h"

UARTServo servo;
```
#### Initialize This Object
Initialize this Object in setup(), we use the begin() function:
```cpp
void begin(HardwareSerial* serial = &Serial, unsigned long baud = BAUD_RATE);
```
Initializes the UART servo library and communication settings, it  should be placed in function setup().
The first parameter is a pointer to the class HardwareSerial, default value is Serial.
The second parameter is transmission rate, default value is [BAUD_RATE](./doc/html/_u_a_r_t_servo_8h.html#ad4455691936f92fdd6c37566fc58ba1f).
```cpp
void setup()
{
    servo.begin(&Serial, 115200);
}
```
* Polling
Update the data of the UARTServo object.
```cpp
void loop()
{
    servo.update();
}
```
### Servo Detection
#### Ping
Detect status of the specified servo, if the servo is online, it will send back its number.
```cpp
// Ping the servo with id 1, and response id into pingCallback();
servo.ping(1, pingCallback);

// Ping the servo with id 2 without callback function,
// but it makes no sense to do so.
servo.ping(2, NULL);

// Callback function for ping(), 
// if the specified servo exists, its id will be treated as a 
// parameter passed into this function.
void pingCallback(byte id)
{
    // TODO: Do something after ping command responsed.
}
```
#### Read Angle
Read the current angle of the servo.
```cpp
// Read angle from the servo with id 1, and response parameters into readAngleCallback();
servo.readAngle(1, readAngleCallback);

// Callback function for readAngle().
void readAngleCallback(byte id, int degree)
{
    // TODO: Do something after readAngle command responsed.
}
```
### Data I/O

### Movement
The UART servo has three motion modes, which are wheel mode, angle mode, and damping mode.
#### Wheel Mode (Spin)
The function [spin()](./doc/html/class_u_a_r_t_servo.html#acfa1fe57b07beab41688fa8d22eab906) set the servo to spin mode, and spin by given parameters. It has the following parameters in order:
* **id** : Servo ID.
* **method** : Spin direction | Spin behavior.
	* **Spin direction** : See the macros [SPIN_CLOCKWISE](./doc/html/_u_a_r_t_servo_8h.html#ac6ad56ecb499a71d3caf3f30d99bd2a7) and [SPIN_COUNTERCLOCKWISE](./doc/html/_u_a_r_t_servo_8h.html#aee7cd3c44a226e6a4149b7a52b3edfa9).
	* **Spin behavior** : See the macros [SPIN_STOP](./doc/html/_u_a_r_t_servo_8h.html#a861e8926bf1f5348eb35d9e6d335e001), [SPIN_START](./doc/html/_u_a_r_t_servo_8h.html#a010c11d92f4ef774cfa76e2698f777ba), [SPIN_BY_CYCLE](./doc/html/_u_a_r_t_servo_8h.html#aefc0fb10212a339b9660c9037d79b842), and [SPIN_BY_TIME](./doc/html/_u_a_r_t_servo_8h.html#abd46aa1c2b185b3aabecacb533ffda91).
* **speed** : Spin speed(unit: degree/sec.).
* **value** : 
	* **By cycle** : The number of turns to be spined(unit: rounds.); 
	* **By time** : The scheduled time to spin(unit: micro second).
* **callback** : Callback function. The parameters in order are servo ID(byte), and result(byte; 1:success, 0:fail).
```cpp
void spin(byte id, byte method, unsigned int speed = 0, unsigned int value = 0, void(*callback)(byte, byte) = NULL);
```
For example:
```cpp
// Specify #3 servo to start turning clockwise, 360 degrees per second.
servo.spin(3, SPIN_CLOCKWISE | SPIN_START, 360);

// Specify #5 servo to start turning counterclockwise 10 times, 90 degrees per second.
servo.spin(5, SPIN_COUNTERCLOCKWISE | SPIN_BY_CYCLE, 90, 100);

// Specify #9 servo to start turning clockwise, 180 degrees per second for five seconds.
servo.spin(9, SPIN_CLOCKWISE | SPIN_BY_TIME, 180, 5000);
```
But how can we stop the servo?
```cpp
// Use the following code to stop the #1 servo.
servo.spin(1, SPIN_STOP);

// We can also use the function stop() to stop the #1 servo.
servo.stop(1);
```
In fact, the above methods can stop the servos in all three modes.
#### Angle Mode (Rotate)
The function [rotate()](./doc/html/class_u_a_r_t_servo.html#a59dcf41d44e58edf8bf8fb44286ed95e) set the servo to rotate mode, and roate by given parameters. It has the following parameters in order:
* **id** : Servo ID.
* **angle** : Specify the angle of rotation(unit: 0.1 degree)
* **interval** : Motion interval.(unit: micro second)
* **power** : Power output of the servo in this mode, if it is zero or greater than the power protection value, adjust to the power protection value.
* **callback** : Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
```cpp
void rotate(byte id, int angle, unsigned int interval, unsigned int power = 0, void(*callback)(byte, byte) = NULL);
```
For example:
```cpp
// It takes two seconds to turn the #1 servo to -135 degree position.
servo.rotate(1, -1350, 2000);
```
#### Damping Mode
The function [damping()](./doc/html/class_u_a_r_t_servo.html#afefe7fd3e16ed9ad7e96e65686eb8a4c) set the servo to damping mode, and specify the power output to the servo to resist external force. It has the following parameters in order:
* **id** : Servo ID.
* **power** : Power output of the servo in this mode, if it is zero or greater than the power protection value, adjust to the power protection value.
* **callback** : Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
```cpp
void damping(byte id, unsigned int power = 0, void(*callback)(byte, byte) = NULL);
```
## Online Manual
[Online manual](./doc/html/index.html)(HTML format).