#pragma once
#include <string>
#include "BlockDevice.h"
#include "Inode.h"

typedef size_t offset_t;

class FileSystem
{
public:
	// C'tor
	FileSystem(const std::string& fileName);

	Inode GetInodeFromOffset(const offset_t offset);
	offset_t GetFreeInodeOffset();
	void SetInodeFromOffset(const offset_t offset, const Inode& inode);

	offset_t GetFreeBlockOffset();
	void WriteIntoBlock(const byte* const data, const offset_t offset, const size_t count);

	void SetBlockStateByOffset(const bool state, const offset_t offset);

private:
	BlockDevice m_blockDevice;

// For const values
private:
	static constexpr size_t MB = 1024 * 1024;
	static constexpr size_t BLOCK_DEVICE_SIZE = 1 * MB;
	static constexpr size_t NUM_OF_INODES = 512;
	static constexpr size_t SIZE_OF_BLOCK = 256;
	static constexpr size_t NUM_OF_BLOCKS = (BLOCK_DEVICE_SIZE - (NUM_OF_INODES * sizeof(Inode))) / SIZE_OF_BLOCK;
	static constexpr offset_t BLOCK_BITMAP_OFFEST = sizeof(Inode) * NUM_OF_INODES;
	static constexpr offset_t BLOCKS_START_OFFEST = BLOCK_BITMAP_OFFEST + NUM_OF_BLOCKS;
};
