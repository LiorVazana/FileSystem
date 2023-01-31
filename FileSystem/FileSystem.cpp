#include "FileSystem.h"

FileSystem::FileSystem(const std::string& fileName)
	:	m_blockDevice(fileName, BLOCK_DEVICE_SIZE)
{
	Inode defaultInode;

	for (int i = 0; i < NUM_OF_INODES; ++i)
	{
		m_blockDevice.Write(reinterpret_cast<byte*>(&defaultInode), i * sizeof(Inode), sizeof(Inode));
	}

	byte isBlockFree = 0;

	for (int i = 0; i < NUM_OF_BLOCKS; ++i)
	{
		m_blockDevice.Write(&isBlockFree, NUM_OF_INODES * sizeof(Inode) + i, 1);
	}
}

Inode FileSystem::GetInodeFromOffset(const offset_t offset)
{
	Inode inode;
	m_blockDevice.Read(reinterpret_cast<byte*>(&inode), offset, sizeof(Inode));
	return inode;
}

offset_t FileSystem::GetFreeInodeOffset()
{
	for (int i = 0; i < NUM_OF_INODES; ++i)
	{
		if (GetInodeFromOffset(i * sizeof(Inode)).InodeType == InodeType::FREE)
			return i * sizeof(Inode);
	}
	throw InvalidInput("There are no free Inodes left. :(");
}

void FileSystem::SetInodeFromOffset(const offset_t offset, const Inode& inode)
{
	m_blockDevice.Write(reinterpret_cast<const byte*>(&inode), offset, sizeof(Inode));
}

offset_t FileSystem::GetFreeBlockOffset()
{
	byte isBlockFree;

	for (int i = 0; i < NUM_OF_BLOCKS; ++i)
	{
		m_blockDevice.Read(&isBlockFree, BLOCK_BITMAP_OFFEST + i, 1);

		if (isBlockFree)
			return BLOCKS_START_OFFEST + (i * SIZE_OF_BLOCK);
	}
}

void FileSystem::WriteIntoBlock(const byte* const data, const offset_t offset, const size_t count)
{
	if (count > SIZE_OF_BLOCK)
		throw InvalidInput("Can't write more than 256 bytes to block.");

	m_blockDevice.Write(data, offset, count);
}

void FileSystem::SetBlockStateByOffset(const bool state, const offset_t offset)
{
	offset_t blockInBitmapOffset = BLOCK_BITMAP_OFFEST + ((offset - BLOCKS_START_OFFEST) / SIZE_OF_BLOCK);
	m_blockDevice.Write(reinterpret_cast<const byte*>(&state), offset, 1);
}
