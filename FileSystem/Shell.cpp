#include "Shell.h"

Shell::Shell(FileSystem& fileSystem)
	:	m_fileSystem(fileSystem)
{
}

void Shell::ExecuteCommand(const std::string& command)
{
	if (command.empty())
		return;

	size_t sepIndex = command.find_first_of(' ');
	std::string commandName = command.substr(0, sepIndex);
	std::string arg = sepIndex == std::string::npos ? "" : command.substr(sepIndex + 1);

	if (m_commandsHandlers.count(commandName) == 0)
		throw InvalidInput("Command doesn't supported");

	(this->*m_commandsHandlers[commandName])(arg);
}

void Shell::Cat(const std::string& path)
{
	std::string absolutePath = GetAbsolutePath(path);

	if (m_fileSystem.IsDir(absolutePath))
		throw PathException("Can't use cat command on directory");

	std::vector<byte> rawContent = m_fileSystem.GetFileContent(absolutePath);

	for (byte b : rawContent)
	{
		std::cout << b;
	}
}

void Shell::Edit(const std::string& path)
{
	std::string line;
	std::vector<byte> fileContent;

	if (m_fileSystem.GetFileEntry(path).IsDir)
		throw InvalidInput("'Edit' can be used only for files");

	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << "Write the file content line by line" << std::endl;
	std::cout << "Press 'ctrl+z' and right after 'enter' to stop edit mode and save the content" << std::endl;
	std::cout << "-----------------------------------------------------------------------------" << std::endl;

	while (getline(std::cin, line) && !std::cin.eof())
	{
		fileContent.insert(fileContent.end(), line.begin(), line.end());
		fileContent.push_back('\n');
	}

	// Flush the buffer
	std::cin.ignore(std::numeric_limits<std::streamsize>::max());
	std::cin.clear();

	m_fileSystem.SetFileContent(GetAbsolutePath(path), fileContent);
}

void Shell::Mkdir(const std::string& path)
{
	m_fileSystem.CreateDirEntry(GetAbsolutePath(path), true);
}

void Shell::Touch(const std::string& path)
{
	m_fileSystem.CreateDirEntry(GetAbsolutePath(path), false);
}

void Shell::Ls(const std::string& path)
{
	std::vector<Entry> entries = m_fileSystem.GetDirEntries(GetAbsolutePath(path));

	std::cout << "Total Entries: " << entries.size() << std::endl;

	for (const Entry& entry : entries)
	{
		std::cout << entry.Name  << (entry.IsDir ? " (Dir)" : " (File)") << std::endl;
	}
}

void Shell::Format(const std::string& type)
{
	if (type != "soft" && type != "hard")
		throw InvalidInput("Usage: format [soft/hard]");

	if (type == "soft")
		m_fileSystem.SoftFormat();
	else
		m_fileSystem.HardFormat();
}

void Shell::Tree(const std::string& path, const std::wstring& prefix)
{
	std::vector<Entry> entries = m_fileSystem.GetDirEntries(path);

	for (int i = 0; i < entries.size(); ++i)
	{
		const Entry& currEntry = entries[i];
		std::wstring entryPrefix = prefix;

		if (i == entries.size() - 1)
			entryPrefix += L"└── ";
		else
			entryPrefix += L"├── ";

		std::wcout << entryPrefix;
		std::cout << currEntry.Name << (currEntry.IsDir ? " (Dir)" : " (File)") << std::endl;

		if (currEntry.IsDir) 
		{
			std::wstring dirPrefix = prefix;

			if (i == entries.size() - 1)
				dirPrefix += L"    ";
			else
				dirPrefix += L"│   ";

			Tree(path + "/" + currEntry.Name, dirPrefix);
		}
	}
}

void Shell::Tree(const std::string& path)
{
	std::string absoluePath = GetAbsolutePath(path);

	setlocale(LC_ALL, "en_US.UTF-8");

	std::cout << absoluePath << std::endl;
	Tree(absoluePath, L"");
}

std::string Shell::GetAbsolutePath(const std::string& path)
{
	std::string absolutePath = path;

	if (absolutePath.empty() || absolutePath[0] != '/')
		absolutePath = "/" + absolutePath;

	return absolutePath;
}
