#pragma once
#include <stdexcept>
#include <string>

class FileSystemException : public std::runtime_error
{
public:
	FileSystemException(const std::string& msg)
		:	std::runtime_error(msg)
	{
	}
};
