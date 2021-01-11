#pragma once
#include "FileSystem.hpp"
#include<string>
#include<iostream>

using namespace std;

// CLI for FileSystem
class FileSystemCLI {
	FileSystem* fs;
	// If ownFs = true, then the filesystem has been created by FileSystemCLI
	// On class destruction - delete fs if its not an outside reference
	bool ownFS;
	// Splits string by whitespace
	vector<string> exctractNames(const string& str) const;
public:
	// Creates FileSystem instance
	FileSystemCLI();
	// Creates fileSystem from existing one
	FileSystemCLI(FileSystem* fs_);
	// Load fileSystem from file
	// File has special format
	FileSystemCLI(const string& DBpath);

	// Is there an attached fileSystem
	bool hasFS();
	// Read and execute command on fileSystem reference
	void readCommand(const string& line);
	
	// Deletes fs, only is isOnw==true
	~FileSystemCLI();
};

inline FileSystemCLI::FileSystemCLI() : fs(new FileSystem()), ownFS(true) {}

inline FileSystemCLI::FileSystemCLI(FileSystem* fs_) : fs(fs_), ownFS(false) {}

inline FileSystemCLI::FileSystemCLI(const string& DBpath) : FileSystemCLI() {
	if (!FileSystem::tryToLoadFS(DBpath, *fs)) {
		fs = nullptr;
		ownFS = false;
	}
}

inline bool FileSystemCLI::hasFS() {
	return fs != nullptr;
}

inline vector<string> FileSystemCLI::exctractNames(const string& str) const {
	vector<string> names;
	string cur = "";
	int k = str.length();
	for (int i = 0; i < k; i++) {
		if (str[i] != ' ') {
			cur += str[i];
		}
		else {
			if (cur != "") {
				names.push_back(cur);
			}
			cur = "";
		}
	}
	if (cur != "") {
		names.push_back(cur);
	}
	return names;
}

inline void FileSystemCLI::readCommand(const string& line) {
	if (hasFS()) {
		if (line == "man") {
			cout << MAN_STR << endl;
		}
		else if (line == "pwd") {
			cout << fs->getCurPath() << endl;
			return;
		}
		else {
			int first_space = line.find(' ', 0);
			string command = line.substr(0, first_space), operand = "";
			if (first_space > -1) {
				operand = line.substr(first_space + 1, line.length());
			}

			if (command == "cd") {
				if (operand == "") {
					fs->cd("/");
				}
				else {
					fs->cd(operand);
				}
			}
			else if (command == "ls") {
				vector<string> file_names;

				if (operand == "") {
					file_names = fs->getDirFiles(".");
				}
				else {
					file_names = fs->getDirFiles(operand);
				}
				int k = file_names.size();
				if (k > 0) {
					for (int i = 0; i < k; i++) {
						cout << file_names[i] + " ";
					}
					cout << endl;
				}
				else {
					cout << "Empty directory" << endl;
				}
			}
			else if (command == "mkdir") {
				vector<string> dirNames = exctractNames(operand);
				int k = dirNames.size();
				for (int i = 0; i < k; i++) {
					fs->createDirPath(dirNames[i]);
				}
			}
			else if (command == "rmdir") {
				vector<string> dirNames = exctractNames(operand);
				int k = dirNames.size();
				for (int i = 0; i < k; i++) {
					fs->removeDirPath(dirNames[i], true);
				}
			}
			else if (command == "stat") {
				vector<string> fileNames = exctractNames(operand);
				int k = fileNames.size();
				for (int i = 0; i < k; i++) {
					bool isDir = (fileNames[i].find(FILE_TYPE_SEP) == -1);
					if (isDir) {
						cout << fs->getMetadataStrPath(fileNames[i] + FILE_TYPE_SEP + DIRECTORY_TYPE_STR) << endl;
					}
					else {
						cout << fs->getMetadataStrPath(fileNames[i]);
					}
					cout << endl;
				}
			}
			else if (command == "cat") {
				vector<string> catParamList = exctractNames(operand);
				int k = catParamList.size();
				int redirection_index = -1;
				for (int i = 0; i < k; i++) {
					if (catParamList[i] == CAT_REDIRECT_STR) {
						redirection_index = i;
						break;
					}
				}
				if (k == 0) {
					// cat
				}
				else {
					if (redirection_index == -1) {
						// cat file1 file2 file3
						string res = "";
						int k = catParamList.size();
						for (int i = 0; i < k; i++) {
							res += fs->getFileContents(catParamList[i]);
						}
						cout << res << endl;
					}
					else if (redirection_index == 0 && k == 2) {
						// cat > file
						string contents = "";
						string line = "";
	
						 while (line != END_OF_INPUT_STR){
							 if (line != "") {
								 contents += line + "\n";
							 }
							getline(cin, line);
						}

						if (fs->fileExists(catParamList[1])) {
							fs->updateFilePath(catParamList[1], contents);
						}
						else {
							fs->createFilePath(catParamList[1], contents);
						}

					}
					else if (redirection_index > 0 && redirection_index == k - 2) {
						// cat file1 file2 > file3
						string contents;

						// Concatenate contents
						for (int i = 0; i < redirection_index; i++) {
							contents += fs->getFileContents(catParamList[i]);
						}

						// If file exists - update it, else - create a file
						if (fs->fileExists(catParamList[redirection_index + 1])) {
							fs->updateFilePath(catParamList[redirection_index + 1], contents);
						}
						else {
							fs->createFilePath(catParamList[redirection_index + 1], contents);
						}

					}
					else {
						// invalid operands
					}
				}
			}
			else if (command == "rm") {
				vector<string> fileNames = exctractNames(operand);
				int k = fileNames.size();
				for (int i = 0; i < k; i++) {
					fs->removeFilePath(fileNames[i]);
				}
			}
			else if (command == "cp") {
				vector<string> argsStr = exctractNames(operand);
				int k = argsStr.size();
				for (int i = 0; i < k - 1; i++) {
					cout << argsStr[i] << endl;
					fs->copyFile(argsStr[i], argsStr[k-1]);
				}
			}
			else if (command == "ln") {
				vector<string> args = exctractNames(operand);
				if (args.size() == 2) {
					string symLinkContents = "";
					// Get absolute path for first argument
					symLinkContents = fs->getAbsPath(args[0]);
					// Attach symlink
					fs->createFilePath(args[1] + FILE_TYPE_SEP + SYMLINK_TYPE_STR, symLinkContents);
				}
			}
			else {
			cerr << "Invalid command. Type 'man' for help." << endl;
}
		}
	}
	else {
		cerr << "No attached file system" << endl;
		return;
	}
}

inline FileSystemCLI::~FileSystemCLI() {
	if (ownFS) {
		delete fs;
	}
}