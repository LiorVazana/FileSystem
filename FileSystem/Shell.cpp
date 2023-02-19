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
	std::string path = sepIndex == std::string::npos ? "" : command.substr(sepIndex + 1);

	if (path.empty() || path[0] != '/')
		path = "/" + path;

	if (m_commandsHandlers.count(commandName) == 0)
		throw InvalidInput("Command doesn't supported");

	(this->*m_commandsHandlers[commandName])(path);
}

void Shell::Cat(const std::string& path)
{
	if (m_fileSystem.IsDir(path))
		throw PathException("Can't use cat command on directory");

	std::vector<byte> rawContent = m_fileSystem.GetFileContent(path);

	for (byte b : rawContent)
	{
		std::cout << b;
	}
}

void Shell::Edit(const std::string& path)
{
	std::string line;
	std::vector<byte> fileContent;

	while (getline(std::cin, line) && !std::cin.eof())
	{
		fileContent.insert(fileContent.end(), line.begin(), line.end());
	}
}

void Shell::Mkdir(const std::string& path)
{
	m_fileSystem.CreateDirEntry(path, true);
}

void Shell::Touch(const std::string& path)
{
	m_fileSystem.CreateDirEntry(path, false);
}

void Shell::Ls(const std::string& path)
{
	std::vector<Entry> entries = m_fileSystem.GetDirEntries(path);

	for (const Entry& entry : entries)
	{
		std::cout << entry.Name  << (entry.IsDir ? "/" : "") << std::endl;
	}
}

void Shell::Format(const std::string& type)
{
	if (type != "soft" || type != "hard")
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
		std::cout << currEntry.Name << std::endl;

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
	setlocale(LC_ALL, "en_US.UTF-8");

	std::cout << path << std::endl;
	Tree(path, L"");
}
