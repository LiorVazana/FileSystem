#include "BlockDevice.h"

BlockDevice::BlockDevice(const std::string& fileName, const size_t size)
	:	m_fileName(fileName), m_size(size), m_fileStream(fileName, std::fstream::binary)
{
	if (!m_fileStream)
	{
		for (int i = 0; i < size; ++i)
		{
			m_fileStream << '\0';
		}
	}
}

void BlockDevice::Write(const byte* const buff, const size_t offset, const size_t count)
{
	if (offset + count > m_size)
		throw InvalidInput("Invalid memory access to block device in the writing proccess");

	m_fileStream.seekp(offset);

	for (int i = 0; i < count; ++i)
	{
		m_fileStream << buff[i];
	}
}

void BlockDevice::Read(byte* const buff, const size_t offset, const size_t count)
{
	if (offset + count > m_size)
		throw InvalidInput("Invalid memory access to block device in the reading proccess");

	m_fileStream.seekg(offset);

	for (int i = 0; i < count; ++i)
	{
		m_fileStream >> buff[i];
	}
}
