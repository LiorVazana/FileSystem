#pragma once
#include <string>
#include "FileSystemException.h"

class InvalidInput : public FileSystemException
{
public:
	InvalidInput(const std::string& msg)
		:	FileSystemException(msg)
	{
	}
};