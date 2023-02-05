#include "FileSystem.h"

FileSystem::FileSystem(const std::string& deviceName)
	:	m_blockDevice(deviceName, BLOCK_DEVICE_SIZE)
{
	Inode defaultInode;
	Inode firstDir;
	firstDir.InodeType == InodeType::DIR;

	m_blockDevice.Write(reinterpret_cast<byte*>(&firstDir), 0, sizeof(Inode));

	for (int i = 1; i < NUM_OF_INODES; ++i)
	{
		m_blockDevice.Write(reinterpret_cast<byte*>(&defaultInode), i * sizeof(Inode), sizeof(Inode));
	}

	byte isBlockFree = 0;

	for (int i = 0; i < NUM_OF_BLOCKS; ++i)
	{
		m_blockDevice.Write(&isBlockFree, NUM_OF_INODES * sizeof(Inode) + i, 1);
	}
}

Inode FileSystem::GetInodeFromIndex(offset_t index)
{
	if (index >= NUM_OF_INODES || index < 0)
		throw InvalidInput("given index is out of bounds (must be between 0 to " + std::to_string(NUM_OF_INODES));

	return GetInodeFromOffset(index * sizeof(Inode));
}

void FileSystem::SetInodeFromIndex(const offset_t index, const Inode& inode)
{
	offset_t offset = GetInodeOffsetFromIndex(index);
	m_blockDevice.Write(reinterpret_cast<const byte*>(&inode), offset, sizeof(Inode));
}

offset_t FileSystem::GetInodeOffsetFromIndex(const offset_t index)
{
	return index * sizeof(Inode);
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

offset_t FileSystem::GetInodeIndexFromOffset(const offset_t offset)
{
	return offset / sizeof(Inode);
}

void FileSystem::SetInodeFromOffset(const offset_t offset, const Inode& inode)
{
	m_blockDevice.Write(reinterpret_cast<const byte*>(&inode), offset, sizeof(Inode));
}

size_t FileSystem::GetNumOfBlocksFromInodeOffset(const offset_t offset)
{
	Inode inode = GetInodeFromOffset(offset);
	return std::ceil((double)inode.Length / SIZE_OF_BLOCK);
}

void FileSystem::SetInodeContent(const offset_t inodesOffset, const std::vector<byte>& contentVec)
{
	Inode inode = GetInodeFromOffset(inodesOffset);
	size_t numOfBlocks = GetNumOfBlocksFromInodeOffset(inodesOffset);
	size_t length = contentVec.size();
	const byte* content = contentVec.data();
	size_t contentIndex = 0;

	for (int i = 0; i < numOfBlocks; ++i)
	{
		SetBlockStateByIndex(false, inode.Blocks[i]);
	}

	size_t blockNeeded = std::ceil((double)length / SIZE_OF_BLOCK);

	for (int i = 0; i < blockNeeded-1; ++i)
	{
		offset_t blockOffset = GetFreeBlockOffset();
		m_blockDevice.Write(content + (SIZE_OF_BLOCK * i), blockOffset, SIZE_OF_BLOCK);
	}

	if (blockNeeded > 1 && (double)length / SIZE_OF_BLOCK == blockNeeded)
	{
		offset_t blockOffset = GetFreeBlockOffset();
		m_blockDevice.Write(content + (SIZE_OF_BLOCK * (blockNeeded - 1)), blockOffset, SIZE_OF_BLOCK * ((double)length / SIZE_OF_BLOCK));
	}
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

offset_t FileSystem::GetBlockOffsetFromIndex(const offset_t index)
{
	if (index >= NUM_OF_BLOCKS || index < 0)
		throw InvalidInput("given index is out of bounds (must be between 0 to " + std::to_string(NUM_OF_BLOCKS));

	return BLOCKS_START_OFFEST + (index * SIZE_OF_BLOCK);
}

void FileSystem::WriteIntoBlockFromOffset(const byte* const data, const offset_t offset, const size_t count)
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

void FileSystem::WriteIntoBlockFromIndex(const byte* const data, const offset_t index, const size_t count)
{
	offset_t offset = GetBlockOffsetFromIndex(index);
	WriteIntoBlockFromOffset(data, offset, count);
}

offset_t FileSystem::GetBlockIndexFromOffset(const offset_t offset)
{
	if (offset < BLOCKS_START_OFFEST || offset > BLOCKS_START_OFFEST + (NUM_OF_BLOCKS * SIZE_OF_BLOCK))
		throw InvalidInput("given offset is out of bounds");

	return (offset - BLOCKS_START_OFFEST) / SIZE_OF_BLOCK;
}

void FileSystem::SetBlockStateByIndex(const bool state, const offset_t index)
{
	offset_t offset = GetBlockOffsetFromIndex(index);
	SetBlockStateByOffset(state, offset);
}

offset_t FileSystem::CreateDirEntry(const std::string& entryName, const bool isDir, const offset_t indexOfDirInode)
{	
	// read the dir that should contain the file and search for one with the same name
	Inode dirInode = GetInodeFromIndex(indexOfDirInode);
	std::unordered_map<std::string, size_t> dirEntries = GetFilesFromDir(indexOfDirInode);

	if (dirEntries.count(entryName) != 0)
		throw PathException("File with this name is already exist.");

	// update the dir that should contain the file by adding the new file name with his suitable inode
	offset_t newInodeOffset = GetFreeInodeOffset();
	Inode newInode = GetInodeFromOffset(newInodeOffset);

	newInode.InodeType = isDir ? InodeType::DIR : InodeType::FILE;
	dirEntries[entryName] == GetInodeIndexFromOffset(newInodeOffset);

	newInode.DateCreated = time(nullptr);
	newInode.LastModified = time(nullptr);

	SetFilesToDir(dirEntries, indexOfDirInode);
	SetInodeFromOffset(newInodeOffset, newInode);
}

void FileSystem::SetFilesToDir(const std::unordered_map<std::string, size_t>& dirEntries, const size_t dirInodeIndex)
{
	if (GetInodeFromIndex(dirInodeIndex).InodeType != InodeType::DIR)
		throw InvalidInput("Index of the the inode isn't a directory inode.");

	std::vector<byte> dirRawContent;
	std::string name;
	size_t inodeIndex;

	for (const auto& pair : dirEntries)
	{
		name = pair.first;
		inodeIndex = pair.second;

		for (const byte b : name)
		{
			dirRawContent.push_back(b);
		}

		dirRawContent.push_back('\0');

		byte* inodeIndexPtr = reinterpret_cast<byte*>(&inodeIndex);

		for (int i = 0; i < sizeof(inodeIndex); ++i)
		{
			dirRawContent.push_back(inodeIndexPtr[i]);
		}
	}

	SetInodeContent(GetInodeOffsetFromIndex(inodeIndex), dirRawContent);
}

std::unordered_map<std::string, size_t> FileSystem::GetFilesFromDir(const offset_t indexOfDirInode)
{
	if (GetInodeFromIndex(indexOfDirInode).InodeType != InodeType::DIR)
		throw InvalidInput("Index of the the inode isn't a directory inode.");

	std::vector<byte> content = GetInodesBlocksContent(indexOfDirInode);
	std::unordered_map<std::string, size_t> dirEntries;
	size_t i = 0;
	std::string name;
	byte currByte;

	while (i < content.size())
	{
		currByte = content[i];

		if (currByte != '\0')
		{
			name += currByte;
		}
		else
		{
			size_t inodeEntry = 0;
			byte* inodeEntryPtr = reinterpret_cast<byte*>(&inodeEntry);

			for (int j = 0; j < sizeof(inodeEntry); ++j)
			{
				inodeEntryPtr[j] = content[++i];
			}

			dirEntries[name] = inodeEntry;
			name = "";
		}

		++i;
	}

	return dirEntries;
}

std::vector<byte> FileSystem::GetInodesBlocksContent(const offset_t inodesOffset)
{
	std::vector<byte> blockContentVec;
	Inode inode = GetInodeFromOffset(inodesOffset);
	byte currByte;
	size_t numOfBlocks = GetNumOfBlocksFromInodeOffset(inodesOffset);
	size_t len = inode.Length;

	for (int i = 0; i < numOfBlocks; ++i)
	{
		for (int j = 0; j < SIZE_OF_BLOCK && len; ++j, --len)
		{
			m_blockDevice.Read(&currByte, inode.Blocks[i], 1);
			blockContentVec.push_back(currByte);
		}
	}

	return blockContentVec;
}
