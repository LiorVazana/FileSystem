#include "BlockDevice.h"

BlockDevice::BlockDevice(const std::string& devicePath, const size_t size)
	:	m_deviceName(devicePath), m_size(size), m_deviceStream(devicePath, std::fstream::in | std::fstream::out | std::fstream::binary)
{
	m_deviceStream.seekg(0, m_deviceStream.end);
	size_t fileSize = m_deviceStream.tellg();

	if (fileSize != size)
		Format();
}

void BlockDevice::Write(const byte* const buff, const size_t offset, const size_t count)
{
	if (offset + count > m_size)
		throw InvalidInput("Invalid memory access to block device in the writing proccess");

	const Inode* const inode = reinterpret_cast<const Inode* const>(buff);

	m_deviceStream.seekp(offset);
	m_deviceStream.write(buff, count);

	m_deviceStream.flush();
}

void BlockDevice::Read(byte* const buff, const size_t offset, const size_t count)
{
	if (offset + count > m_size)
		throw InvalidInput("Invalid memory access to block device in the reading proccess");

	m_deviceStream.seekg(offset);
	m_deviceStream.read(buff, count);

	m_deviceStream.flush();
}

void BlockDevice::Format()
{
	m_deviceStream.seekp(0);

	for (int i = 0; i < m_size; ++i)
	{
		m_deviceStream << '\0';
	}
}
