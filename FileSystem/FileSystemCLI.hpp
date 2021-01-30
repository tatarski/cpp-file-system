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

	// Command logic
	void printMan();
	void printWd();
	void execCD(const string& operand);
	void printLS(const string& operand);
	void execMKDIR(const string& operand);
	void execRMDIR(const string& operand);
	void printSTAT(const string& operand);
	void execCAT(const string& operand);
	void execRM(const string& operand);
	void execCP(const string& operand);
	void execLN(const string& operand);
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
	void readCommand(const string& line, bool& shouldContinue);

	// Deletes fs, only is isOwn==true
	~FileSystemCLI();
};

inline void FileSystemCLI::execRM(const string& operand) {
	vector<string> fileNames = exctractNames(operand);
	int k = fileNames.size();
	for (int i = 0; i < k; i++) {
		fs->removeFilePath(fileNames[i]);
	}
}

inline void FileSystemCLI::execCP(const string& operand) {
	vector<string> fileNames = exctractNames(operand);
	int k = fileNames.size();
	for (int i = 0; i < k; i++) {
		fs->removeFilePath(fileNames[i]);
	}
}

inline void FileSystemCLI::execLN(const string& operand) {
	vector<string> fileNames = exctractNames(operand);
	int k = fileNames.size();
	for (int i = 0; i < k; i++) {
		fs->removeFilePath(fileNames[i]);
	}
}

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

inline void FileSystemCLI::printMan() {
	cout << MAN_STR << endl;
}

inline void FileSystemCLI::printWd() {
	cout << fs->getCurPath() << endl;
}

inline void FileSystemCLI::execCD(const string& operand) {
	if (operand == "") {
		fs->cd("/");
	}
	else {
		fs->cd(operand);
	}
}

inline void FileSystemCLI::printLS(const string& operand) {
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
}

inline void FileSystemCLI::execMKDIR(const string& operand) {
	vector<string> dirNames = exctractNames(operand);
	int k = dirNames.size();
	for (int i = 0; i < k; i++) {
		fs->createDirPath(dirNames[i]);
	}
}

inline void FileSystemCLI::execRMDIR(const string& operand) {
	vector<string> dirNames = exctractNames(operand);
	int k = dirNames.size();
	for (int i = 0; i < k; i++) {
		fs->removeDirPath(dirNames[i], true);
	}
}

inline void FileSystemCLI::printSTAT(const string& operand) {
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

inline void FileSystemCLI::execCAT(const string& operand) {
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
		cerr << "CAT requires at least one argument!" << endl;
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

			while (line != END_OF_INPUT_STR) {
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
			cerr << "Invalid operand list" << endl;
		}
	}
}

inline void FileSystemCLI::readCommand(const string& line, bool& shouldContinue) {
	shouldContinue = true;
	if (hasFS()) {
		// 0 argument commands
		if (line == "man") {
			printMan();
			return;
		}
		else if (line == "pwd") {
			printWd();
			return;
		}
		else if (line == "exit") {
			shouldContinue = false;
			return;
		}
		else {
			// >0 argument commands
			int first_space = line.find(' ', 0);
			string command = line.substr(0, first_space), operand = "";
			if (first_space > -1) {
				operand = line.substr(first_space + 1, line.length());
			}
			if (command == "cd") {
				execCD(operand);
			}
			else if (command == "ls") {
				printLS(operand);
			}
			else if (command == "mkdir") {
				execMKDIR(operand);
			}
			else if (command == "rmdir") {
				execRMDIR(operand);
			}
			else if (command == "stat") {
				printSTAT(operand);
			}
			else if (command == "cat") {
				execCAT(operand);
			}
			else if (command == "rm") {
				execRM(operand);
			}
			else if (command == "cp") {
				execCP(operand);
			}
			else if (command == "ln") {
				execLN(operand);
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