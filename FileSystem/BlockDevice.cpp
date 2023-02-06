#include "BlockDevice.h"

BlockDevice::BlockDevice(const std::string& devicePath, const size_t size)
	:	m_deviceName(devicePath), m_size(size), m_deviceStream(devicePath, std::fstream::in | std::fstream::out | std::fstream::trunc)
{
	if (!m_deviceStream)
	{
		for (int i = 0; i < size; ++i)
		{
			m_deviceStream << '\0';
		}
	}
}

void BlockDevice::Write(const byte* const buff, const size_t offset, const size_t count)
{
	if (offset + count > m_size)
		throw InvalidInput("Invalid memory access to block device in the writing proccess");

	m_deviceStream.seekp(offset);

	for (int i = 0; i < count; ++i)
	{
		m_deviceStream << buff[i];
	}

	m_deviceStream.flush();
}

void BlockDevice::Read(byte* const buff, const size_t offset, const size_t count)
{
	if (offset + count > m_size)
		throw InvalidInput("Invalid memory access to block device in the reading proccess");

	m_deviceStream.seekg(offset);

	for (int i = 0; i < count; ++i)
	{
		m_deviceStream >> buff[i];
	}

	m_deviceStream.flush();
}
