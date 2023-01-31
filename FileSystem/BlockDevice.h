#pragma once
#include <string>
#include <fstream>
#include "InvalidInput.h"

typedef char byte;

class BlockDevice
{
public:
	// C'tor
	BlockDevice(const std::string& fileName, const size_t size);

	void Write(const byte* const buff, const size_t offset, const size_t count);
	void Read(byte* const buff, const size_t offset, const size_t count);


private:
	std::string m_fileName;
	size_t m_size;
	std::fstream m_fileStream;
};
