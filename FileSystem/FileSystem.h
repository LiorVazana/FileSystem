#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "BlockDevice.h"
#include "Inode.h"
#include "PathException.h"
#include "Entry.h"

typedef size_t offset_t;

class FileSystem
{
public:
	// C'tor
	FileSystem(const std::string& devicePath);

	void CreateDirEntry(const std::string& path, const bool isDir);                  // Create file / directory
	std::vector<Entry> GetDirEntries(const std::string& path);						 // Get all files in given directory
	Entry GetFileEntry(const std::string& path);									 // 
	std::vector<byte> GetFileContent(const std::string& path);                       // Get the file content
	void SetFileContent(const std::string& path, const std::vector<byte>& data);     // Update the file contant
	bool IsDir(const std::string& path);                                             // check if the entry is directory
	void SoftFormat();
	void HardFormat();

private:
	// Inode with index accessing
	Inode GetInodeFromIndex(const offset_t index);
	offset_t GetInodeOffsetFromIndex(const offset_t index);
	void SetInodeFromIndex(const offset_t index, const Inode& inode);

	// Inode with offset accessing
	Inode GetInodeFromOffset(const offset_t offset);
	offset_t GetFreeInodeOffset();
	offset_t GetInodeIndexFromOffset(const offset_t offset);
	void SetInodeFromOffset(const offset_t offset, const Inode& inode);
	size_t GetNumOfBlocksFromInodeOffset(const offset_t offset);
	void SetInodeContent(const offset_t inodesOffset, const std::vector<byte>& contentVec);

	// Block with offset accesing
	offset_t GetFreeBlockOffset();
	offset_t GetBlockOffsetFromIndex(const offset_t index);
	void WriteIntoBlockFromOffset(const byte* const data, const offset_t offset, const size_t count);
	void SetBlockStateByOffset(const bool isFree, const offset_t offset);
	// Block with index accesing
	void WriteIntoBlockFromIndex(const byte* const data, const offset_t index, const size_t count);
	offset_t GetBlockIndexFromOffset(const offset_t offset);
	void SetBlockStateByIndex(const bool isFree, const offset_t index);

	void CreateDirEntry(const std::string& entryName, const bool isDir, const offset_t indexOfDirInode);
	void SetFilesToDir(const std::unordered_map<std::string, size_t>& dirEntries, const size_t dirInodeIndex);
	std::unordered_map<std::string, size_t> GetEntriesFromDir(const offset_t indexOfDirInode);
	std::vector<byte> GetInodesBlocksContent(const offset_t inodesOffset);
	size_t GetInodeIndexFromPath(const std::string& path, const size_t inodeIndex = 0);


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
