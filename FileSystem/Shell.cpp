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
	std::vector<std::string> entries = m_fileSystem.GetDirEntries(path);

	for (const std::string& entry : entries)
	{
		std::cout << entry << std::endl;
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
