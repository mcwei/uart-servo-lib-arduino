// UARTServo.h

#ifndef UARTSERVO_H
#define UARTSERVO_H

// If you want to use software serial port, uncomment the following line.
//#define SOFTWARE_SERIAL

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "ByteBuffer.h"

#ifdef SOFTWARE_SERIAL
#include "SoftwareSerial.h"
#endif

/// Default connecting rate.
#define BAUD_RATE				115200

/// All servos.
#define ALL_SERVOS				0xff

/// Spin direction: counterclockwise.
#define SPIN_CLOCKWISE			0x80
/// Spin direction: clockwise.
#define SPIN_COUNTERCLOCKWISE	0x00

/// Spin behavior: stop spin.
#define SPIN_STOP				0x00
/// Spin behavior: start spin.
#define SPIN_START				0x01 
/// Spin behavior: spin by cycle.
#define SPIN_BY_CYCLE			0x02
/// Spin behavior: spin by time.
#define SPIN_BY_TIME			0x03

/// Custom parameters.
struct UserParameter
{
	/*!
	 * Reserved field, must be set to 1 when write data to the servo.
	 * Data ID: 32.
	 * \sa #UARTServo::writeBatchData
	 */
	byte reserved;
	/*!
	 * In the spin or rotate mode, whether the servo has responded after completing the command.
	 * If the value is set to non-zero(true), the old command cannot be interrupted by the new command. 
	 * The waiting commands will be added to a limited queue, if the queue is full, the newest one is discarded.
	 * If the value is set to zero(false), the past command can be interrupted by the new command 
	 * and the response packet will not be sent back.
	 * The default value is zero (false).
	 * Data ID: 33.
	 */
	byte responsive; 
	/*!
	 * Servo ID.
	 * Data ID: 34.
	 */
	byte id;
	/*!
	 * Control mode
	 * Data ID: 35.
	 */
	byte controlMode;
	/*!
	 * Connecting rate index.
	 * 0x01-9600,
	 * 0x02-19200,
	 * 0x03-38400,
	 * 0x04-57600,
	 * 0x05-115200 (default),
	 * 0x06-250000,
	 * 0x07-500000,
	 * Data ID: 36.
	 */
	byte baudIndex;
	/*!
	 * Stall protection.
	 * Data ID: 37.
	 */
	byte stallProtect;
	/*!
	 * Stall power limit.
	 * Data ID: 38.
	 */
	unsigned int stallPowerLimit;
	/*!
	 *
	 * Data ID: 39.
	 */
	unsigned int overVoltageLowLevel;
	/*!
	 *
	 * Data ID: 40.
	 */
	unsigned int overVoltageHighLevel;
	/*!
	 *
	 * Data ID: 41.
	 */
	unsigned int overTemperatureTriggerLevel;
	/*!
	 *
	 * Data ID: 42.
	 */
	unsigned int overPowerTriggerLevel;
	/*!
	 *
	 * Data ID: 43.
	 */
	unsigned int overCurrentTriggerLevel;
	/*!
	 *
	 * Data ID: 44.
	 */
	byte startupSpeed;
	/*!
	 *
	 * Data ID: 45.
	 */
	byte brakeSpeed;
	/*!
	 *
	 * Data ID: 46.
	 */
	byte powerLockSwitch;
	/*!
	 *
	 * Data ID: 47.
	 */
	byte wheelModeBrakeSwitch;
	/*!
	 *
	 * Data ID: 48.
	 */
	byte angleLimitSwitch;
	/*!
	 *
	 * Data ID: 49.
	 */
	byte softStartSwitch;
	/*!
	 *
	 * Data ID: 50.
	 */
	unsigned int softStartTime;
	/*!
	 *
	 * Data ID: 51.
	 */
	int angleUpperLimit;
	/*!
	 *
	 * Data ID: 52.
	 */
	int angleLowerLimit;
	/*!
	 *
	 * Data ID: 53.
	 */
	int centerPointOffset;
};

/*!
 * UARTServo class
 * This class is mainly used to read and write the parameters of the data area, 
 * and provides related functions of three motion modes for the user to drive the servo(s).
 */
class UARTServo
{
public:
	/*!
	 * Initializes the UART servo library and communication settings.
	 * It should be placed in function setup().
	 * This function is only enable as SOFTWARE_SERIAL be defined.
	 * 
	 * \param rxPin Receive pin number. 
	 * \param txPin Transmit pin number.
	 * \param baud Transmission rate, default value is BAUD_RATE.
	 */
	void begin(unsigned int rxPin, unsigned int txPin, unsigned long baud = BAUD_RATE);

	/*!
	 * Initializes the UART servo library and communication settings.
	 * It should be placed in function setup().
	 * 
	 * \param serial A pointer to the class HardwareSerial, default value is Serial.
	 * \param baud Transmission rate, default value is BAUD_RATE.
	 */
	void begin(HardwareSerial* serial = &Serial, unsigned long baud = BAUD_RATE);

	/*!
	 * Update data of this class.
	 * It should be placed in function loop().
	 */
	void update();

	/*!
	 * Detect status of the specified servo.
	 * 
	 * \param id Servo ID.
	 * \param callback Callback function. The parameter is Servo ID(byte).
	 */
	void ping(byte id, void(*callback)(byte));

	/*!
	 * Reset the parameters of the user area.
	 * 
	 * \param id Servo ID.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
	 */
	void resetUserData(byte id, void(*callback)(byte, byte));
	
	/*!
	 * Read specified data from the specified servo.
	 * 
	 * \param id Servo ID.
	 * \param dataID Data ID.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), data id(byte), and data(A const void pointer, data size is packet length - 2).
	 */
	void readData(byte id, byte dataID, void(*callback)(byte, byte, const void*));

	/*!
	 * Write specified data to the specified servo.
	 * If there are a lot of fields to be written at one time, this function is not recommended, please use writeBatchData() instead.
	 * \sa #UARTServo::writeBatchData
	 * 
	 * \param id Servo ID.
	 * \param dataID Data ID.
	 * \param data Written data, a const void pointer point to it.
	 * \param size Data size.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), data id(byte), and result(byte; 1:success, 0:fail).
	 */
	void writeData(byte id, byte dataID, const void* data, size_t size, void(*callback)(byte, byte, byte));

	/*!
	 * Read batch data from the specified servo.
	 * 
	 * \param id Servo ID.
	 * \param callback Callback function. The parameters in order are Servo ID(byte) and data(const UserParameter*).
	 */
	void readBatchData(byte id, void(*callback)(byte, const UserParameter*));

	/*!
	 * Write batch data to the specified servo.
	 * 
	 * \param id Servo ID.
	 * \param parameter Servo batch data.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
	 */
	void writeBatchData(byte id, const UserParameter* parameter, void(*callback)(byte, byte));

	/*!
	 * Set the servo to spin mode, and spin by given parameters.
	 * \sa SPIN_CLOCKWISE, SPIN_COUNTERCLOCKWISE, SPIN_STOP, SPIN_START, SPIN_BY_CYCLE, SPIN_BY_TIME.
	 * 
	 * \param id Servo ID.
	 * \param method Spin direction | Spin behavior.
	 * \param speed Spin speed(unit: degree/sec.).
	 * \param value By cycle: The number of turns to be spined(unit: rounds.); by time: The scheduled time to spin(unit: micro second).
	 * \param callback Callback function. The parameters in order are servo ID(byte), and result(byte; 1:success, 0:fail).
	 */
	void spin(byte id, byte method, unsigned int speed = 0, unsigned int value = 0, void(*callback)(byte, byte) = NULL);

	/*!
	 * Stop All actions of the specified servo and release it. 
	 * 
	 * \param id Servo ID.
	 */
	void stop(byte id);

	/*!
	 * Set the servo to rotate mode, and roate by given parameters.
	 * 
	 * \param id Servo ID.
	 * \param angle Specify the angle of rotation(unit: 0.1 degree)
	 * \param interval Motion interval.(unit: micro second)
	 * \param power Power output of the servo in this mode, if it is zero or greater than the power protection value, adjust to the power protection value.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
	 */
	void rotate(byte id, int angle, unsigned int interval, unsigned int power = 0, void(*callback)(byte, byte) = NULL);

	/*!
	 * Set the servo to rotate mode, and roate by given parameters.
	 * 
	 * \param id Servo ID.
	 * \param angle Specify the angle of rotation(unit: 0.1 degree)
	 * \param interval Motion interval.(unit: micro second)
	 * \param accInterval Acceleration interval.
	 * \param decInterval Deceleration interval.
	 * \param power Power output of the servo in this mode, if it is zero or greater than the power protection value, adjust to the power protection value.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
	 */
	void rotateByInterval(byte id, int angle, unsigned int interval, unsigned int accInterval, unsigned int decInterval, unsigned int power = 0, void(*callback)(byte, byte) = NULL);
	
	/*!
	 *  Set the servo to rotate mode, and roate by given parameters.
	 * 
	 * \param id Servo ID.
	 * \param angle Specify the angle of rotation(unit: 0.1 degree)
	 * \param targetVelocity Target velocity.
	 * \param accInterval Acceleration interval.
	 * \param decInterval Deceleration interval.
	 * \param power Power output of the servo in this mode, if it is zero or greater than the power protection value, adjust to the power protection value.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
	 */
	void rotateByVelocity(byte id, int angle, unsigned int targetVelocity, unsigned int accInterval, unsigned int decInterval, unsigned int power = 0, void(*callback)(byte, byte) = NULL);

	/*!
	 * Set the servo to damping mode.
	 * Specify the power output to the servo to resist external force.
	 * 
	 * \param id Servo ID.
	 * \param power Power output of the servo in this mode, if it is zero or greater than the power protection value, adjust to the power protection value.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), and result(byte; 1:success, 0:fail).
	 */
	void damping(byte id, unsigned int power = 0, void(*callback)(byte, byte) = NULL);
	
	/*!
	 * Read the current angle of the servo.
	 * 
	 * \param id Servo ID.
	 * \param callback Callback function. The parameters in order are Servo ID(byte), and degree(int; unit is 0.1 degree).
	 */
	void readAngle(byte id, void(*callback)(byte, int));

private:

#ifdef SOFTWARE_SERIAL
	SoftwareSerial* _serial;
#else
	HardwareSerial* _serial;
#endif

	ByteBuffer _txBuffer;
	ByteBuffer _rxBuffer;

	void(*_pingCallback)(byte);
	void(*_resetUserDataCallback)(byte, byte);
	void(*_readDataCallback)(byte, byte, const void*);
	void(*_writeDataCallback)(byte, byte, byte);
	void(*_readBatchDataCallback)(byte, const UserParameter*);
	void(*_writeBatchDataCallback)(byte, byte);
	void(*_spinCallback)(byte, byte);
	void(*_rotateCallback)(byte, byte);
	void(*_dampingCallback)(byte, byte);
	void(*_readAngleCallback)(byte, int);
	void(*_rotateByIntervalCallback)(byte, byte);
	void(*_rotateByVelocityCallback)(byte, byte);

	void init();
	void handleByteFromServo(byte data);
	void makeHeader(byte number, byte size);
	void writeSerialData();
};

#endif