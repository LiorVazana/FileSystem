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
	void ExecuteCommand(const std::string& path);

private:
	void Cat(const std::vector<std::string>& argVec);
	void Edit(const std::vector<std::string>& argVec);
	void Mkdir(const std::vector<std::string>& argVec);
	void Touch(const std::vector<std::string>& argVec);
	void Ls(const std::vector<std::string>& argVec);
	void Format(const std::vector<std::string>& argVec);
	void Tree(const std::vector<std::string>& argVec, const std::wstring& prefix);
	void Tree(const std::vector<std::string>& argVec);
	void cls(const std::vector<std::string>& argVec);
	void rm(const std::vector<std::string>& argVec);

private:
	std::string GetAbsolutePath(const std::string& path);

private:
	typedef void(Shell::*CommandHandler)(const std::vector<std::string>&);
	std::unordered_map<std::string, CommandHandler> m_commandsHandlers = { {"cat", &Shell::Cat}, {"edit", &Shell::Edit}, {"mkdir", &Shell::Mkdir}, {"touch", &Shell::Touch},
																		   {"ls", &Shell::Ls}, {"format", &Shell::Format}, {"tree", &Shell::Tree}, {"cls", &Shell::cls}, {"rm", &Shell::rm} };
	FileSystem& m_fileSystem;
};
