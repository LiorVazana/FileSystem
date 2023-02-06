#include <iostream>
#include "Shell.h"
#include "FileSystem.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " [path to block device]" << std::endl;
		return -1;
	}

	FileSystem fileSystem(argv[1]);
	Shell shell(fileSystem);

	const std::string EXIT_COMMAND = "exit";
	std::string input = "";

	std::cout << ">> ";
	std::getline(std::cin, input);

	while (input != EXIT_COMMAND)
	{
		try
		{
			shell.ExecuteCommand(input);
		}
		catch (const FileSystemException& e)
		{
			std::cerr << e.what() << std::endl;
		}

		std::cout << ">> ";
		std::getline(std::cin, input);
	}

	return 0;
}
