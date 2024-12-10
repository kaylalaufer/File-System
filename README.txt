Kayla Laufer
Github Link: https://github.com/kaylalaufer/File-System

Introduction:
The FileManager CLI is a command-line interface for managing a virtual file system. It allows users to create, read, write, move, 
and delete files and directories. The system simulates a Unix-like file system with a hierarchical directory structure, providing 
a simple way to manage files and directories through a set of intuitive commands.

Features:
File Operations: Create, read, write, move, and delete files.
Directory Operations: Create and delete directories and list their contents.
Data Persistence: Files are stored in a virtual disk file with persistent metadata.

How It Works:
File Table: Tracks file and directory metadata (e.g., path, size, type).
Disk Manager: Manages disk storage, tracks available disk blocks, and stores file data.
Data Storage: Each file is stored as a series of blocks on a virtual disk, and directories are used to organize files hierarchically.
Files and directories are stored in a persistent disk file (cli_disk.dat), and metadata is saved in filesystem.dat, ensuring data 
    is retained between sessions.

How to Run:
Start Fresh (Clean Build)
1. Delete existing build directory:
    rm -rf build
    mkdir build
    cd build
    cmake ..
    make command_line
    ./command_line
2. Run Unit Tests (make sure to comment out the main function in command_line.cpp):
    rm -rf build
    mkdir build
    cd build
    cmake ..
    make runTests
    ./runTests
Continue from an Existing Build
1. If the build directory already exists, navigate to it:
    cd build
2. Run the program, 'make command_line' is only needed on first compile or if files were modified:
    make command_line
    ./command_line
3. Run the tests (comment out main in command_line.cpp):
    make runTests
    ./runTests

Commands:
help: Shows all available commands.
create_file [path] [size]: Creates a new file at the specified path with the given size (default size is 100 bytes).
delete_file [path]: Deletes a file.
create_dir [path]: Creates a directory at the given path. By default, it will recursively create parent directories if they don't exist.
delete_dir [path] [recursive]: Deletes a directory at the given path. By default, it will recursively delete sub content.
list_dir [path]: Lists the contents of a directory. If no path is provided, it lists the contents of the root (/).
write_file [path] [data] [append]: Writes data to a file. By default, it appends (true) the data, but can be set to overwrite (false).
read_file [path]: Reads and displays the contents of a file.
move_file [source] [destination]: Moves a file from one location to another. You can also rename the file as part of the move by 
    specifying a new name in the destination path. The file name must be part of the destination path.
exit: Exits the CLI.

Error Handling:
The system provides error messages for common issues:
    File does not exist: When you try to access a non-existent file.
    Path is not a file: When you attempt to read/write to a directory instead of a file.
    Invalid command: When an unrecognized command is entered.
    Directory does not exist: When trying to access or create files in a non-existent directory.
    File already exists: When trying to create a file that already exists.

Development Notes:
File Table: Tracks metadata for each file and directory (e.g., path, size, type).
    File types (file or directory) are explicitly stored, so file extensions (like .txt) are not required.
    Each entry in the FileTable tracks the path, type, size, and allocated disk blocks.
Disk Manager: Handles disk storage, tracking available and used disk blocks.
    When a file is deleted, the disk space is released and marked as free.
Data Persistence: File metadata is stored in filesystem.dat.
    File contents and disk block information are stored in cli_disk.dat, which persists even after the 
    program is closed.

Examples:
Create a directory:
> create_dir /folder1

Create a file:
> create_file /folder1/file1.txt 50

Write to a file:
> write_file /folder1/file1.txt "Hello, World!"

Read a file:
> read_file /folder1/file1.txt
Contents of /folder1/file1.txt:
Hello, World!

Delete a file:
> delete_file /folder1/file1.txt

Move a file (moves and renames the file)
> move_file /folder1/file1.txt /folder2/file2.txt

List directory contents:
> list_dir /
Contents of /:
  folder1
  folder2