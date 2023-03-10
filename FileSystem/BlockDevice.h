#pragma once
#include <string>
#include <fstream>
#include "InvalidInput.h"
#include "Inode.h"

typedef char byte;

class BlockDevice
{
public:
	// C'tor
	BlockDevice(const std::string& devicePath, const size_t size);

	void Write(const byte* const buff, const size_t offset, const size_t count);
	void Read(byte* const buff, const size_t offset, const size_t count);
	void Format();


private:
	std::string m_deviceName;
	size_t m_size;
	std::fstream m_deviceStream;
};
