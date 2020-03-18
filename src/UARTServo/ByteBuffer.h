#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class ByteBuffer
{
public:
	void init(unsigned int bufferSize);
	void destory();
	void clear();
	unsigned int getCapacity();
	unsigned int getLength();
	void back(unsigned int step = 1);
	void forward(unsigned int step = 1);
	byte read();
	void read(void* dest, size_t size);
	int readInt();
	unsigned int readUInt();
	long readLong();
	float readFloat();
	unsigned long readULong();
	void write(byte data);
	void write(const void* src, size_t size);
	void writeInt(int data);
	void writeUInt(unsigned int data);
	void writeLong(long data);
	void writeULong(unsigned long data);
	void writeFloat(float data);
	bool checksum();
	byte writeChecksum(unsigned int startIndex = 0, unsigned int endIndex = 0);

private:
	byte* _data;
	unsigned int _capacity;
	unsigned int _position;
	unsigned int _length;
};

#endif