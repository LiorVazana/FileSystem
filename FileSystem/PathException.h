#pragma once
#include <string>
#include "FileSystemException.h"

class PathException : public FileSystemException
{
public:
	PathException(const std::string& msg)
		: FileSystemException(msg)
	{
	}
};