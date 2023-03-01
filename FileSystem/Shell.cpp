#include "Shell.h"

Shell::Shell(FileSystem& fileSystem)
	:	m_fileSystem(fileSystem)
{
}

void Shell::ExecuteCommand(const std::string& path)
{
	if (path.empty())
		return;

	size_t sepIndex = path.find_first_of(' ');
	std::string commandName = path.substr(0, sepIndex);
	std::string arg = sepIndex == std::string::npos ? "" : path.substr(sepIndex + 1);
	std::vector<std::string> argsVec;
	argsVec.push_back(arg);

	if (m_commandsHandlers.count(commandName) == 0)
		throw InvalidInput("Command doesn't supported");

	(this->*m_commandsHandlers[commandName])(argsVec);
}

void Shell::Cat(const std::vector<std::string>& argVec)
{
	std::string absolutePath = GetAbsolutePath(argVec[0]);

	if (m_fileSystem.IsDir(absolutePath))
		throw PathException("Can't use cat command on directory");

	std::vector<byte> rawContent = m_fileSystem.GetFileContent(absolutePath);

	for (byte b : rawContent)
	{
		std::cout << b;
	}
}

void Shell::Edit(const std::vector<std::string>& argVec)
{
	cls(argVec);

	std::string line;
	std::vector<byte> fileContent;

	if (m_fileSystem.GetFileEntry(argVec[0]).IsDir)
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

	m_fileSystem.SetFileContent(GetAbsolutePath(argVec[0]), fileContent);

	cls(argVec);
}

void Shell::Mkdir(const std::vector<std::string>& argVec)
{
	m_fileSystem.CreateDirEntry(GetAbsolutePath(argVec[0]), true);
}

void Shell::Touch(const std::vector<std::string>& argVec)
{
	m_fileSystem.CreateDirEntry(GetAbsolutePath(argVec[0]), false);
}

void Shell::Ls(const std::vector<std::string>& argVec)
{
	std::vector<Entry> entries = m_fileSystem.GetDirEntries(GetAbsolutePath(argVec[0]));

	std::cout << "Total Entries: " << entries.size() << std::endl;

	for (const Entry& entry : entries)
	{
		std::cout << entry.Name << (entry.IsDir ? " (Dir)" : " (File)") << std::endl;
	}
}

void Shell::Format(const std::vector<std::string>& argVec)
{
	if (argVec[0] != "soft" && argVec[0] != "hard")
		throw InvalidInput("Usage: format [soft/hard]");

	if (argVec[0] == "soft")
		m_fileSystem.SoftFormat();
	else
		m_fileSystem.HardFormat();
}

void Shell::Tree(const std::vector<std::string>& argVec, const std::wstring& prefix)
{
	std::vector<Entry> entries = m_fileSystem.GetDirEntries(argVec[0]);

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

			std::vector<std::string> newArgVec;
			newArgVec.push_back(argVec[0] + "/" + currEntry.Name);
			Tree(newArgVec, dirPrefix);
		}
	}
}

void Shell::Tree(const std::vector<std::string>& argVec)
{
	std::string absoluePath = GetAbsolutePath(argVec[0]);

	setlocale(LC_ALL, "en_US.UTF-8");

	std::cout << absoluePath << std::endl;

	std::vector<std::string> newArgVec;
	newArgVec.push_back(argVec[0]);
	Tree(newArgVec, L"");
}

void Shell::cls(const std::vector<std::string>& argVec)
{
	system("cls");
}

void Shell::rm(const std::vector<std::string>& argVec)
{
	for (const std::string& entryName : argVec)
	{
		m_fileSystem.RemoveEntry(entryName);
	}	
}

void Shell::rmdir(const std::vector<std::string>& argVec)
{
}

std::string Shell::GetAbsolutePath(const std::string& path)
{
	std::string absolutePath = path;

	if (absolutePath.empty() || absolutePath[0] != '/')
		absolutePath = "/" + absolutePath;

	return absolutePath;
}
