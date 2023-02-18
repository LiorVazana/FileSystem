#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include "FileSystem.h"
#include "InvalidInput.h"
#include "FileSystemException.h"

class Shell
{
public:
	// C'tor
	Shell(FileSystem& fileSystem);
	void ExecuteCommand(const std::string& command);

private:
	void Cat(const std::string& path);
	void Edit(const std::string& path);
	void Mkdir(const std::string& path);
	void Touch(const std::string& path);
	void Ls(const std::string& path);
	void Format(const std::string& type);
	void Tree(const std::string& path, const std::wstring& prefix);
	void Tree(const std::string& path);

private:
	typedef void(Shell::*CommandHandler)(const std::string&);
	std::unordered_map<std::string, CommandHandler> m_commandsHandlers = { {"cat", &Shell::Cat}, {"edit", &Shell::Edit}, {"mkdir", &Shell::Mkdir}, {"touch", &Shell::Touch},
																		   {"ls", &Shell::Ls}, {"format", &Shell::Format}, {"tree", &Shell::Tree} };
	FileSystem& m_fileSystem;
};
