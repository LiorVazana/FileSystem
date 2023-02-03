#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "BlockDevice.h"
#include "Inode.h"
#include "PathException.h"

typedef size_t offset_t;

class FileSystem
{
public:
	// C'tor
	FileSystem(const std::string& deviceName);

	// Inode with index accessing
	Inode GetInodeFromIndex(offset_t index);
	offset_t GetInodeOffsetFromIndex(const offset_t index);
	void SetInodeFromIndex(const offset_t index, const Inode& inode);

	// Inode with offset accessing
	Inode GetInodeFromOffset(const offset_t offset);
	offset_t GetFreeInodeOffset();
	offset_t GetInodeIndexFromOffset(const offset_t offset);
	void SetInodeFromOffset(const offset_t offset, const Inode& inode);
	size_t GetNumOfBlocksFromInodeOffset(const offset_t offset);
	void SetInodeContent(const offset_t inodesOffset, const std::vector<byte>& content);

	// Block with offset accesing
	offset_t GetFreeBlockOffset();
	offset_t GetBlockOffsetFromIndex(const offset_t index);
	void WriteIntoBlockFromOffset(const byte* const data, const offset_t offset, const size_t count);
	void SetBlockStateByOffset(const bool isTaken, const offset_t offset);
	// Block with index accesing
	void WriteIntoBlockFromIndex(const byte* const data, const offset_t index, const size_t count);
	offset_t GetBlockIndexFromOffset(const offset_t offset);
	void SetBlockStateByIndex(const bool isTaken, const offset_t index);

	offset_t CreateFile(const std::string& fileName, const offset_t indexOfDirInode);
	std::unordered_map<std::string, size_t> GetFilesFromDir(const offset_t indexOfDirInode);
	std::vector<byte> GetInodesBlocksContent(const offset_t inodesOffset);

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
