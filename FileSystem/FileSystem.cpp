#include "FileSystem.h"

FileSystem::FileSystem(const std::string& devicePath)
	:	m_blockDevice(devicePath, BLOCK_DEVICE_SIZE)
{
	if (GetInodeFromIndex(0).Type == InodeType::DIR)
		return;

	SoftFormat();
}

std::vector<Entry> FileSystem::GetDirEntries(const std::string& path)
{
	std::vector<Entry> entries;
	size_t inodeIndex = GetInodeIndexFromPath(path);
	std::unordered_map<std::string, size_t> dirEntries = GetEntriesFromDir(inodeIndex);
	Inode inode;
	Entry entry;

	for (const auto& pair : dirEntries)
	{
		inode = GetInodeFromIndex(pair.second);
		entry.IsDir = inode.Type == InodeType::DIR;
		entry.Name = pair.first;
		entry.Length = inode.Length;

		entries.push_back(entry);
	}

	return entries;
}

void FileSystem::RemoveEntry(const std::string& path)
{
	if (path.empty() || path[path.length() - 1] == '/')
		throw PathException("Invalid Path: Entry not specified");

	std::string fileName = path.substr(path.find_last_of('/') + 1);
	std::string dirPath = path.substr(0, path.find_last_of('/'));

	if (dirPath.empty() || dirPath == fileName)
		dirPath = "/";

	size_t inodeIndex = GetInodeIndexFromPath(dirPath);
	std::unordered_map<std::string, size_t> dirEntries = GetEntriesFromDir(inodeIndex);

	if (dirEntries.count(fileName) == 0)
		throw PathException("PathException: given entry doesn't exist");

	Inode entryInode = GetInodeFromIndex(GetInodeIndexFromPath(path));

	if (entryInode.Type == InodeType::DIR)
	{
		if (GetEntriesFromDir(GetInodeIndexFromPath(fileName)).size() != 0)
			throw PathException("PathException: can't remove a not empty directory.");
	}

	dirEntries.erase(path.substr(path.find_last_of('/') + 1));
	SetFilesToDir(dirEntries, inodeIndex);
}

Entry FileSystem::GetFileEntry(const std::string& path)
{
	Inode inode;
	Entry entry;

	size_t inodeIndex = GetInodeIndexFromPath(path);
	inode = GetInodeFromIndex(inodeIndex);
	entry.IsDir = inode.Type == InodeType::DIR;
	entry.Name = path.substr(path.find_last_of('/') + 1);
	entry.Length = inode.Length;

	return entry;
}

std::vector<byte> FileSystem::GetFileContent(const std::string& path)
{
	return GetInodesBlocksContent(GetInodeOffsetFromIndex(GetInodeIndexFromPath(path)));
}

void FileSystem::SetFileContent(const std::string& path, const std::vector<byte>& data)
{
	SetInodeContent(GetInodeOffsetFromIndex(GetInodeIndexFromPath(path)), data);
}

bool FileSystem::IsDir(const std::string& path)
{
	return GetInodeFromIndex(GetInodeIndexFromPath(path)).Type == InodeType::DIR;
}

void FileSystem::SoftFormat()
{
	Inode defaultInode;
	Inode firstDir;

	firstDir.Type = InodeType::DIR;

	SetInodeFromIndex(0, firstDir);

	for (int i = 1; i < NUM_OF_INODES; ++i)
	{
		SetInodeFromIndex(i, defaultInode);
	}

	for (int i = 0; i < NUM_OF_BLOCKS; ++i)
	{
		SetBlockStateByIndex(true, i);
	}
}

void FileSystem::HardFormat()
{
	m_blockDevice.Format();
	SoftFormat();
}

Inode FileSystem::GetInodeFromIndex(const offset_t index)
{
	if (index >= NUM_OF_INODES || index < 0)
		throw InvalidInput("given index is out of bounds (must be between 0 to " + std::to_string(NUM_OF_INODES));


	Inode inode = GetInodeFromOffset(index * sizeof(Inode));
	return inode;
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
		if (GetInodeFromOffset(i * sizeof(Inode)).Type == InodeType::FREE)
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
	size_t length = contentVec.size();
	size_t numOfBlocks = GetNumOfBlocksFromInodeOffset(inodesOffset);
	const byte* content = contentVec.data();
	size_t contentIndex = 0;

	for (int i = 0; i < numOfBlocks; ++i)
	{
		SetBlockStateByIndex(true, inode.Blocks[i]);
	}

	size_t blockNeeded = std::ceil((double)length / SIZE_OF_BLOCK);

	for (int i = 0; i < blockNeeded; ++i)
	{
		size_t sizeToWrite = std::min(SIZE_OF_BLOCK, (size_t)(length - SIZE_OF_BLOCK * i));
		offset_t blockOffset = GetFreeBlockOffset();
		SetBlockStateByOffset(false, blockOffset);
		m_blockDevice.Write(content + (SIZE_OF_BLOCK * i), blockOffset, sizeToWrite);
		inode.Blocks[i] = GetBlockIndexFromOffset(blockOffset);
	}

	inode.Length = contentVec.size();
	SetInodeFromOffset(inodesOffset, inode);
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
	throw InvalidInput("There are no free blocks left");
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

void FileSystem::SetBlockStateByOffset(const bool isFree, const offset_t offset)
{
	offset_t blockInBitmapOffset = BLOCK_BITMAP_OFFEST + ((offset - BLOCKS_START_OFFEST) / SIZE_OF_BLOCK);
	m_blockDevice.Write(reinterpret_cast<const byte*>(&isFree), blockInBitmapOffset, 1);
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

void FileSystem::SetBlockStateByIndex(const bool isFree, const offset_t index)
{
	offset_t offset = GetBlockOffsetFromIndex(index);
	SetBlockStateByOffset(isFree, offset);
}

void FileSystem::CreateDirEntry(const std::string& path, const bool isDir)
{
	size_t lastSepIndex = path.find_last_of('/');

	std::string beforeSep = path.substr(0, lastSepIndex);
	std::string afterSep = path.substr(lastSepIndex + 1);

	size_t indexOfDirInode = beforeSep.empty() ? 0 : GetInodeIndexFromPath(beforeSep);

	CreateDirEntry(afterSep, isDir, indexOfDirInode);
}

void FileSystem::CreateDirEntry(const std::string& entryName, const bool isDir, const offset_t indexOfDirInode)
{	
	// read the dir that should contain the file and search for one with the same name
	Inode dirInode = GetInodeFromIndex(indexOfDirInode);
	std::unordered_map<std::string, size_t> dirEntries = GetEntriesFromDir(indexOfDirInode);

	if (dirEntries.count(entryName) != 0)
		throw PathException("File with this name is already exist.");

	// update the dir that should contain the file by adding the new file name with his suitable inode
	offset_t newInodeOffset = GetFreeInodeOffset();
	Inode newInode = GetInodeFromOffset(newInodeOffset);

	newInode.Type = isDir ? InodeType::DIR : InodeType::FILE;
	SetInodeFromOffset(newInodeOffset, newInode);

	dirEntries[entryName] = GetInodeIndexFromOffset(newInodeOffset);

	SetInodeFromOffset(newInodeOffset, newInode);

	SetFilesToDir(dirEntries, indexOfDirInode);
}

void FileSystem::SetFilesToDir(const std::unordered_map<std::string, size_t>& dirEntries, const size_t dirInodeIndex)
{
	if (GetInodeFromIndex(dirInodeIndex).Type != InodeType::DIR)
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

	SetInodeContent(GetInodeOffsetFromIndex(dirInodeIndex), dirRawContent);
}

std::unordered_map<std::string, size_t> FileSystem::GetEntriesFromDir(const offset_t indexOfDirInode)
{
	Inode inode = GetInodeFromIndex(indexOfDirInode);
	if (GetInodeFromIndex(indexOfDirInode).Type != InodeType::DIR)
		throw InvalidInput("Index of the the inode isn't a directory inode.");

	std::vector<byte> content = GetInodesBlocksContent(GetInodeOffsetFromIndex(indexOfDirInode));
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
		for (int j = 0; j < SIZE_OF_BLOCK && len != 0; ++j, --len)
		{
			m_blockDevice.Read(&currByte, GetBlockOffsetFromIndex(inode.Blocks[i]) + j, 1);
			blockContentVec.push_back(currByte);
		}
	}

	return blockContentVec;
}

size_t FileSystem::GetInodeIndexFromPath(const std::string& path, const size_t inodeIndex)
{
	if (path.empty())
		throw PathException("Path can't be empty");

	if (path == "/")
		return 0;
	
	char sep = '/';
	size_t sepIndex = path.find_first_of('/');

	std::string beforeSep = path.substr(0, sepIndex);
	std::string afterSep  = sepIndex == std::string::npos ? "" : path.substr(sepIndex + 1);

	if (beforeSep.empty())
		return GetInodeIndexFromPath(afterSep, inodeIndex);

	std::unordered_map<std::string, size_t> dirEntries = GetEntriesFromDir(inodeIndex);

	if (dirEntries.count(beforeSep) == 0)
		throw PathException("Unknown entry '" + beforeSep + "'");

	size_t entryInodeIndex = dirEntries[beforeSep];

	if (afterSep.empty())
		return entryInodeIndex;

	return GetInodeIndexFromPath(afterSep, entryInodeIndex);
}
