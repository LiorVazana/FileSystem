# FileSystem
This project is a simple implementation of a file system in C++. It allows you to create, edit, delete and navigate files and directories, as well as display their contents.

## How it Works
The file system is essentially a tree structure, with the root directory as the top-level node. Each directory contains a list of files and sub-directories.

To use the file system, you can enter commands through the console. The commands are listed below:

* `ls` - Lists all the files and directories in the current directory.
* `touch [filename]` - Creates a new empty file with the given name in the current directory.
* `mkdir [dirname]` - Creates a new empty directory with the given name in the current directory.
* `cat [filename]` - Prints the contents of the file with the given name.
* `edit [filename]` - Allows you to edit the contents of the file with the given name.
* `tree` - Prints the directory hierarchy as a tree.
* `cls` - Cleans the console.
* `format soft` - Removes all the entries in the file system, but leaves the directory structure intact.
* `format hard` - Removes all the entries in the file system, including their contents.
* `rm [filename/dirname] [filename/dirname] ...` - Removes the files and directorys with the given names from the file system.
## Usage
To use this file system, you can run it on every C++ compiler. The console will prompt you to enter commands, and you can navigate through the file system using the commands listed above.

## Credits
This project was created by **VAZANA**. Feel free to use and modify it as you like.
