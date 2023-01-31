#pragma once
#include <string>
#include "BlockDevice.h"
#include "Inode.h"

class FileSystem
{
public:
	// C'tor
	FileSystem(const std::string& fileName);

private:
	BlockDevice m_blockDevice;

// For const values
private:
	static constexpr size_t MB = 1024 * 1024;
	static constexpr size_t BLOCK_DEVICE_SIZE = 1 * MB;
	static constexpr size_t NUM_OF_INODES = 512;
	static constexpr size_t SIZE_OF_BLOCK = 256;
	static constexpr size_t NUM_OF_BLOCKS = (BLOCK_DEVICE_SIZE - (NUM_OF_INODES * sizeof(Inode))) / SIZE_OF_BLOCK;
};
