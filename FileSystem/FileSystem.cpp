#include "FileSystem.h"

FileSystem::FileSystem(const std::string& fileName)
	:	m_blockDevice(fileName, BLOCK_DEVICE_SIZE)
{
	Inode defaultInode;

	for (int i = 0; i < NUM_OF_INODES; ++i)
	{
		m_blockDevice.Write(reinterpret_cast<byte*>(&defaultInode), i * sizeof(Inode), sizeof(Inode));
	}
}
