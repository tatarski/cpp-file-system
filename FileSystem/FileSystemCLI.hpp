#pragma once
#include "FileSystem.hpp"
#include<string>
#include<iostream>

using namespace std;

class FileSystemCLI {
	FileSystem* fs;
	bool ownFS;
public:
	FileSystemCLI() : fs(new FileSystem()), ownFS(true) {}
	FileSystemCLI(FileSystem* fs_) :fs(fs_), ownFS(false) {}
	FileSystemCLI(const string& DBpath) : FileSystemCLI() {
		if (!FileSystem::tryToLoadFS(DBpath, *fs)) {
			fs = nullptr;
			ownFS = false;
		}
	}
	bool hasFS() {
		return fs != nullptr;
	}
	vector<string> getConcatenationParameters(const string& operand) {
		vector<string> res;
		int k = operand.length();
		string cur = "";
		for (int i = 0; i < k; i++) {
			if (operand[i] == ' ') {
				if (cur != "") {
					res.push_back(cur);
					cur = "";
				}
			}
			else {
				cur = cur + operand[i];
			}
		}
		if (cur != "") {
			res.push_back(cur);
		}
		return res;
	}
	// e.g. splits string by space
	vector<string> exctractNames(const string& str) {
		vector<string> names;
		string cur = "";
		int k = str.length();
		for (int i = 0; i < k; i++) {
			if (str[i] != ' ') {
				cur += str[i];
			}
			else {
				names.push_back(cur);
				cur = "";
			}
		}
		if (cur != "") {
			names.push_back(cur);
		}
		return names;
	}
	void readCommand(const string& line) {
		if (hasFS()) {
			if (line == "pwd") {
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
					vector<string> catParamList = getConcatenationParameters(operand);
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
							string contents;

							getline(cin, contents, END_OF_INPUT_CH);
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
							
							for (int i = 0; i < redirection_index; i++) {
								contents += fs->getFileContents(catParamList[i]);
							}

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
			}
		}
		else {
			cerr << "No attached file system" << endl;
			return;
		}
	}
	~FileSystemCLI() {
		if (ownFS) {
			delete fs;
		}
	}
};