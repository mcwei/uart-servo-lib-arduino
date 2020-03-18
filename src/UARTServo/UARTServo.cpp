#include "UARTServo.h"

#define BUFFER_SIZE			256

#define REQUEST_HEADER		0x4c12
#define RESPONSE_HEADER		0x1c05

#define PING				1
#define RESET_USER_DATA		2
#define READ_DATA			3
#define WRITE_DATA			4
#define READ_BATCH_DATA		5
#define WRITE_BATCH_DATA	6
#define SPIN				7
#define ROTATE				8
#define DAMPING				9
#define READ_ANGLE			10
#define ROTATE_BY_INTERVAL	11
#define ROTATE_BY_VELOCITY	12

#define CALLBACK(c, ...) if (c != NULL) { c(__VA_ARGS__); c = NULL; }

void UARTServo::begin(unsigned int rxPin, unsigned int txPin, unsigned long baud)
{
#ifdef SOFTWARE_SERIAL
	init();
	_serial = new SoftwareSerial(rxPin, txPin);
	_serial->begin(baud);
#endif
}

void UARTServo::begin(HardwareSerial* serial, unsigned long baud)
{
	init();
	_serial = serial;
	_serial->begin(baud);
}

void UARTServo::init()
{
	_txBuffer.init(BUFFER_SIZE);
	_rxBuffer.init(BUFFER_SIZE);
	_pingCallback = NULL;
	_resetUserDataCallback = NULL;
	_readDataCallback = NULL;
	_writeDataCallback = NULL;
	_readBatchDataCallback = NULL;
	_writeBatchDataCallback = NULL;
	_spinCallback = NULL;
	_rotateCallback = NULL;
	_dampingCallback = NULL;
	_readAngleCallback = NULL;
}

void UARTServo::update()
{
	while (_serial->available() != 0)
	{
		handleByteFromServo(_serial->read());
	}
}

void UARTServo::ping(byte id, void(*callback)(byte))
{
	_pingCallback = callback;
	makeHeader(PING, 1);
	_txBuffer.write(id);
	writeSerialData();
}

void UARTServo::resetUserData(byte id, void(*callback)(byte, byte))
{
	_resetUserDataCallback = callback;
	makeHeader(RESET_USER_DATA, 1);
	_txBuffer.write(id);
	writeSerialData();
}

void UARTServo::readData(byte id, byte dataID, void(*callback)(byte, byte, const void *))
{
	_readDataCallback = callback;
	makeHeader(READ_DATA, 2);
	_txBuffer.write(id);
	_txBuffer.write(dataID);
	writeSerialData();
}

void UARTServo::writeData(byte id, byte dataID, const void * data, size_t size, void(*callback)(byte, byte, byte))
{
	_writeDataCallback = callback;
	makeHeader(WRITE_DATA, size + 2);
	_txBuffer.write(id);
	_txBuffer.write(dataID);
	_txBuffer.write(data, size);
	writeSerialData();
}

void UARTServo::readBatchData(byte id, void(*callback)(byte, const UserParameter *))
{
	_readBatchDataCallback = callback;
	makeHeader(READ_BATCH_DATA, 1);
	_txBuffer.write(id);
	writeSerialData();
}

void UARTServo::writeBatchData(byte id, const UserParameter * parameter, void(*callback)(byte, byte))
{
	_writeBatchDataCallback = callback;
	makeHeader(WRITE_BATCH_DATA, 1 + sizeof(parameter));
	_txBuffer.write(id);
	_txBuffer.write(parameter, sizeof(parameter));
	writeSerialData();
}

void UARTServo::spin(byte id, byte method, unsigned int speed, unsigned int value, void(*callback)(byte, byte))
{
	_spinCallback = callback;
	makeHeader(SPIN, 6);
	_txBuffer.write(id);
	_txBuffer.write(method);
	_txBuffer.writeUInt(speed);
	_txBuffer.writeUInt(value);
	writeSerialData();
}

void UARTServo::stop(byte id)
{
	spin(id, SPIN_STOP);
}

void UARTServo::rotate(byte id, int angle, unsigned int interval, unsigned int power, void(*callback)(byte, byte))
{
	_rotateCallback = callback;
	makeHeader(ROTATE, 7);
	_txBuffer.write(id);
	_txBuffer.writeInt(angle);
	_txBuffer.writeUInt(interval);
	_txBuffer.writeUInt(power);
	writeSerialData();
}

void UARTServo::rotateByInterval(byte id, int angle, unsigned int interval, unsigned int accInterval, unsigned int decInterval, unsigned int power, void(*callback)(byte, byte))
{
	_rotateByIntervalCallback = callback;
	makeHeader(ROTATE_BY_INTERVAL, 11);
	_txBuffer.write(id);
	_txBuffer.writeInt(angle);
	_txBuffer.writeUInt(interval);
	_txBuffer.writeUInt(accInterval);
	_txBuffer.writeUInt(decInterval);
	_txBuffer.writeUInt(power);
	writeSerialData();
}

void UARTServo::rotateByVelocity(byte id, int angle, unsigned int targetVelocity, unsigned int accInterval, unsigned int decInterval, unsigned int power, void(*callback)(byte, byte))
{
	_rotateByVelocityCallback = callback;
	makeHeader(ROTATE_BY_VELOCITY, 11);
	_txBuffer.write(id);
	_txBuffer.writeInt(angle);
	_txBuffer.writeUInt(targetVelocity);
	_txBuffer.writeUInt(accInterval);
	_txBuffer.writeUInt(decInterval);
	_txBuffer.writeUInt(power);
	writeSerialData();
}

void UARTServo::damping(byte id, unsigned int power, void(*callback)(byte, byte))
{
	_dampingCallback = callback;
	makeHeader(DAMPING, 3);
	_txBuffer.write(id);
	_txBuffer.writeUInt(power);
	writeSerialData();
}

void UARTServo::readAngle(byte id, void(*callback)(byte, int))
{
	_readAngleCallback = callback;
	makeHeader(READ_ANGLE, 1);
	_txBuffer.write(id);
	writeSerialData();
}

void UARTServo::handleByteFromServo(byte data)
{
	_rxBuffer.write(data);

	while (_rxBuffer.getLength() >= 5) // 5 for header & checksum.
	{
		if (_rxBuffer.readUInt() == RESPONSE_HEADER)
		{
			byte packetNumber = _rxBuffer.read();
			byte packetLength = _rxBuffer.read();

			_rxBuffer.back(4);

			if (_rxBuffer.getLength() >= packetLength + 5) // + 5 for header & checksum.
			{
				// Do checksum.
				if (_rxBuffer.checksum())
				{
					// Pass header.
					_rxBuffer.forward(4);

					switch (packetNumber)
					{
						case PING:
						{
							byte id = _rxBuffer.read();
							CALLBACK(_pingCallback, id);
							break;
						}
						case RESET_USER_DATA:
						{
							byte id = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_resetUserDataCallback, id, result);
							break;
						}
						case READ_DATA:
						{
							size_t size = packetLength - 2;
							byte id = _rxBuffer.read();
							byte dataID = _rxBuffer.read();
							byte* value = new byte[size];
							_rxBuffer.read(value, size);
							CALLBACK(_readDataCallback, id, dataID, value);
							delete[] value;
							break;
						}
						case WRITE_DATA:
						{
							byte id = _rxBuffer.read();
							byte dataID = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_writeDataCallback, id, dataID, result);
							break;
						}
						case READ_BATCH_DATA:
						{
							UserParameter p;
							byte id = _rxBuffer.read();
							_rxBuffer.read(&p, sizeof(p));
							CALLBACK(_readBatchDataCallback, id, &p);
							break;
						}
						case WRITE_BATCH_DATA:
						{
							byte id = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_writeBatchDataCallback, id, result);
							break;
						}
						case SPIN:
						{
							byte id = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_spinCallback, id, result);
							break;
						}
						case ROTATE:
						{
							byte id = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_rotateCallback, id, result);
							break;
						}
						case DAMPING:
						{
							byte id = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_dampingCallback, id, result);
							break;
						}
						case READ_ANGLE:
						{
							byte id = _rxBuffer.read();
							int angle = _rxBuffer.readInt();
							CALLBACK(_readAngleCallback, id, angle);
							break;
						}
						case ROTATE_BY_INTERVAL:
						{
							byte id = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_rotateByIntervalCallback, id, result);
							break;
						}
						case ROTATE_BY_VELOCITY:
						{
							byte id = _rxBuffer.read();
							byte result = _rxBuffer.read();
							CALLBACK(_rotateByVelocityCallback, id, result);
							break;
						}
						default:
						{
							// Destroy all content.
							_rxBuffer.forward(packetLength);
							break;
						}
					}

					// Pass tail.
					_rxBuffer.forward(1);
				}
				else
				{
					// As checksum error occurs, destroy all content.
					_rxBuffer.forward(packetLength + 5);
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			// Shift 1 byte(2 bytes for header).
			_rxBuffer.back(1);
		}
	}
}

void UARTServo::makeHeader(byte number, byte size)
{
	_txBuffer.clear();
	_txBuffer.writeUInt(REQUEST_HEADER);
	_txBuffer.write(number);
	_txBuffer.write(size);
}

void UARTServo::writeSerialData()
{
	_txBuffer.writeChecksum();
	while (_txBuffer.getLength() > 0)
	{
		_serial->write(_txBuffer.read());
	}
}
