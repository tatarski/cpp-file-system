#pragma once
#include "FileSystem.hpp"
#include<string>
using namespace std;

class FileSystemCli {
	FileSystem* fs;
	bool ownFS;
private:
	FileSystem() : fs(new FileSystem()), ownFS(true) {}
	FileSystem(FileSystem* fs_) :fs(fs_), ownFS(false) {}
	FileSystem(const string& DBpath): FileSystem() {
		if (!tryToLoad(DBpath, fs)) {
			fs = nullptr;
			ownFS = false;
			return true;
		}
	}
	bool hasFS() {
		return fs != nullptr
	}
	void readCommand(const string& line) {
		if(hasFS()) {
			if (line == "pwd") {
				cout << fs->getCurPath() << endl;
				return;
			} else if (){
			}
		}
		
	}
	~FileSystem() {
		if (ownFS) {
			delete fs;
		}
	}
};
