#include "ByteBuffer.h"

void ByteBuffer::init(unsigned int bufferSize)
{
	_data = new byte[bufferSize];
	_capacity = bufferSize;
	_position = 0;
	_length = 0;
}

void ByteBuffer::destory()
{
	delete[] _data;
}

void ByteBuffer::clear()
{
	_position = 0;
	_length = 0;
}

unsigned int ByteBuffer::getCapacity()
{
	return _capacity;
}

unsigned int ByteBuffer::getLength()
{
	return _length;
}

void ByteBuffer::back(unsigned int step)
{
	if (_length + step < _capacity)
	{
		_position = (_position - step + _capacity) % _capacity;
		_length += step;
	}
}

void ByteBuffer::forward(unsigned int step)
{
	if (_length > step)
	{
		_position = (_position + step) % _capacity;
		_length -= step;
	}
}

byte ByteBuffer::read()
{
	byte val = 0;

	if (_length > 0)
	{
		val = _data[_position];
		_position = (_position + 1) % _capacity;
		_length--;
	}

	return val;
}

void ByteBuffer::read(void* dest, size_t size)
{
	byte* data = (byte*)dest;
	for (size_t i = 0; i < size; i++)
	{
		data[i] = read();
	}
}

int ByteBuffer::readInt()
{
	int val;
	byte *p = (byte *)&val;
	p[0] = read();
	p[1] = read();
	return val;
}

unsigned int ByteBuffer::readUInt()
{
	unsigned int val;
	byte *p = (byte *)&val;
	p[0] = read();
	p[1] = read();
	return val;
}

long ByteBuffer::readLong()
{
	long val;
	byte *p = (byte *)&val;
	p[0] = read();
	p[1] = read();
	p[2] = read();
	p[3] = read();
	return val;
}

unsigned long ByteBuffer::readULong()
{
	unsigned long val;
	byte *p = (byte *)&val;
	p[0] = read();
	p[1] = read();
	p[2] = read();
	p[3] = read();
	return val;
}

float ByteBuffer::readFloat()
{
	float val;
	byte *p = (byte *)&val;
	p[0] = read();
	p[1] = read();
	p[2] = read();
	p[3] = read();
	return val;
}

void ByteBuffer::write(byte data)
{
	if (_length < _capacity)
	{
		_data[(_position + _length) % _capacity] = data;
		_length++;
	}
}

void ByteBuffer::write(const void * src, size_t size)
{
	byte* data = (byte*)src;
	for (size_t i = 0; i < size; i++)
	{
		write(data[i]);
	}
}

void ByteBuffer::writeInt(int data)
{
	byte *p = (byte *)&data;
	write(p[0]);
	write(p[1]);
}

void ByteBuffer::writeUInt(unsigned int data)
{
	byte *p = (byte *)&data;
	write(p[0]);
	write(p[1]);
}

void ByteBuffer::writeLong(long data)
{
	byte *p = (byte *)&data;
	write(p[0]);
	write(p[1]);
	write(p[2]);
	write(p[3]);
}

void ByteBuffer::writeULong(unsigned long data)
{
	byte *p = (byte *)&data;
	write(p[0]);
	write(p[1]);
	write(p[2]);
	write(p[3]);
}

void ByteBuffer::writeFloat(float data)
{
	byte *p = (byte *)&data;
	write(p[0]);
	write(p[1]);
	write(p[2]);
	write(p[3]);
}

bool ByteBuffer::checksum()
{
	byte val = 0;
	for (int i = 0; i < _length - 1; i++)
	{
		val += _data[(_position + i) % _capacity];
	}
	return (val == _data[(_position + _length - 1) % _capacity]);
}

byte ByteBuffer::writeChecksum(unsigned int startIndex = 0, unsigned int endIndex = 0)
{
	byte val = 0;
	unsigned int end = (endIndex > startIndex) ? endIndex : _length;
	for (int i = startIndex; i < end; i++)
	{
		val += _data[(_position + i) % _capacity];
	}
	write(val);
	return val;
}