#pragma once
#include <ctime>

constexpr size_t BLOCKS_IN_INODE = 10;

#pragma pack(1)
struct Inode
{
	enum InodeType :char
	{
		FREE, FILE,
		DIR
	};

	InodeType InodeType = InodeType::FREE;
	size_t Length = 0;
	size_t Blocks[BLOCKS_IN_INODE] = { 0 };
}; 
