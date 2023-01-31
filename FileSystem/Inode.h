#pragma once
#include <ctime>

constexpr size_t BLOCKS_IN_INODE = 10;

enum class InodeType
{
	FREE, FILE,
	DIR
};

struct Inode
{
	InodeType InodeType = InodeType::FREE;
	size_t Length = 0;
	size_t Blocks[BLOCKS_IN_INODE] = { 0 };
	std::time_t DateCreated = std::time(nullptr);
	std::time_t LastModified = std::time(nullptr);
}; 
