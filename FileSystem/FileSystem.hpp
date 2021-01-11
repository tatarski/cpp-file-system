#pragma once
#include<string>
#include "Tree.hpp"
#include <fstream>
#include<iostream>
#include<string>
#include<stack>
using namespace std;
const char PATH_ROOT_SYM = '/';
const char PATH_DIR_SEP = '/';
const string PATH_UP_STR = "..";
const string PATH_STAY_STR = ".";
const string FILE_TYPE_SEP = ".";
const int NODE_METADATA_BEGIN = 1;
const int NODE_METADATA_SIZE = 9;
const string NODE_PARENT_INDEX_BEGIN_FLAG = "PARENT_INDEX_ARRAY:";
const string DIRECTORY_TYPE_STR = "dir";
const string CAT_REDIRECT_STR = ">";
const char END_OF_INPUT_CH = '.';
struct FileSystemNode {
	unsigned int id;
	unsigned int size;
	bool isDir;
	string name;
	string type;
	time_t created;
	time_t lastChange;
	string contents;

	FileSystemNode(unsigned int id = 0, unsigned int size = 0, bool isDir = false, string name = "", string type = "", time_t created = 0, time_t lastChange = 0, string contents = "")
		: id(id), size(size), isDir(isDir), name(name), type(type), created(created), lastChange(lastChange), contents(contents) {}
	string getFullName() const;
	string toString() const;
};
class FileSystem {
private:
	Tree<FileSystemNode> fs;
	Tree<FileSystemNode>::TreeIterator cur;
	int file_count;

	// Extract path of dir names from path string
	vector<string> getFolderNameListFromPath(const string& path) const;

	// Extracts data read from DB file
	// Fills treeNodeDataList and parentIndexArray with values taken from file
	void extractFSFromFile(vector<FileSystemNode>& treeNodeDataList, vector<int>& parentIndexArray, ifstream& f);
	void getChildrenArray(vector<int>& parentIndexArray, vector<vector<int>>& childrenOf);
	// Traverses tree and calls cur.addChild for each child a node
	void constructTreeWith(vector<int>& parentIndexArray, vector<vector<int>> childrenOf, vector<FileSystemNode>& treeNodeDataList);
	// Returns iterator on folder, found by tracing a path of string representing directory names
	// If path cannot be traced - returns detachhed empty TreeIterator
	Tree<FileSystemNode>::TreeIterator tracePath(const vector<string>& folderNameList, bool fromRoot = false);
	Tree<FileSystemNode>::TreeIterator relativePathIter(const vector<string>& folderNameList);
	Tree<FileSystemNode>::TreeIterator absolutePathIter(const vector<string>& folderNameList);
	Tree<FileSystemNode>::TreeIterator getDirIter(const string& path);
public:
	// Constructor creates file system from pathToDB
	FileSystem();
	bool cd(const string& path);
	string getCurPath();
	vector<string> getDirFiles(const string& path);
	// Finds whether a child name is unique in current directory
	int getChildIndex(const string file_name_type);
	// Create file in current working dir
	void createFile(const string& file_name_type, const string& file_name_contents);
	void createDir(const string& folder_name);
	// Removes child of current directory, only if it is an empty directory
	void removeDir(const string& folder_name, bool shouldBeEmpty);
	void removeDirPath(const string& path, bool shouldBeEmpty) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), dirName = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(dirName + FILE_TYPE_SEP + DIRECTORY_TYPE_STR);
			if (i == -1) {
				cerr << "File not found" << endl;
			}
			else {
				removeDir(dirName, shouldBeEmpty);
			}
			cd(pathBefore);
		}

	}
	string getMetadataStr(const string& file_name_type);
	string getMetadataStrPath(const string& path) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), fileNameType = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		string res = "";
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(fileNameType);
			if (i == -1) {
				cerr << "File does not exist";
				return "";
			}
			else {
				res = getMetadataStr(fileNameType);
			}
			cd(pathBefore);
			return res;
		}
		return "";
	}
	void removeFile(const string& file_name_type);
	void removeFilePath(const string& path) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), fileNameType = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(fileNameType);
			if (i == -1) {
				cerr << "File does not exist";
			}
			else {
				removeFile(fileNameType);
			}
			cd(pathBefore);
		}

	};
	static bool tryToLoadFS(const string& DBPath, FileSystem& fs);
	string getFileContents(const string& path) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), fileNameType = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(fileNameType);
			string res = "";
			if (i == -1) {
				cerr << "File not found" << endl;
			}
			else {
				res = cur.getChild(i).contents;
			}
			cd(pathBefore);
			return res;
		}
		return "";
	}
	void updateFile(const string& file_name_type, const string& file_name_contents) {
		int i = getChildIndex(file_name_type);
		if (i == -1) {
			cerr << "File does not exist" << endl;
		}
		else {
			FileSystemNode& changed = cur.getChild(i);
			changed.contents = file_name_contents;
		}
	}
	void updateFilePath(const string& path, const string& file_name_contents) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), fileNameType = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(fileNameType);
			if (i == -1) {
				cerr << "File does not exist";
			}
			updateFile(fileNameType, file_name_contents);
			cd(pathBefore);
		}

	}
	bool fileExists(const string& path) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), fileNameType = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(fileNameType);
			cd(pathBefore);
			if (i == -1) {
				return false;
			}
			return true;
		}
		return false;


	}
	void createFilePath(const string& path, const string& contents) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), fileNameType = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(fileNameType);
			if (i == -1) {
				createFile(fileNameType, contents);
			}
			else {
				cerr << "File alrady exists" << endl;
			}
			cd(pathBefore);
		}
	}
	void createDirPath(const string& path) {
		string pathBefore = this->getCurPath();
		int lastPathSepI = path.find_last_of(PATH_DIR_SEP);
		string newPath = path.substr(0, lastPathSepI + 1), dirName = path.substr(lastPathSepI + 1, path.length() - lastPathSepI);
		if ((lastPathSepI != -1 && cd(newPath)) || lastPathSepI == -1) {
			int i = getChildIndex(dirName + FILE_TYPE_SEP + DIRECTORY_TYPE_STR);
			if (i == -1) {
				createDir(dirName);
			}
			else {
				cerr << "Directory alrady exists" << endl;
			}
			cd(pathBefore);
		}
	}
};

// Extracts data read from DB file
// Fills treeNodeDataList and parentIndexArray with values taken from file


// Extract path of dir names from path string

inline vector<string> FileSystem::getFolderNameListFromPath(const string& path) const {
	vector<string> res;
	int begin = 0;
	if (path[0] == PATH_ROOT_SYM) {
		begin++;
		res.push_back("/");
	}

	int k = path.length();
	string cur = "";
	for (int i = begin; i < k; i++) {
		if (path[i] == PATH_DIR_SEP) {
			res.push_back(cur);
			cur = "";
		}
		else {
			cur += path[i];
		}
	}
	if (cur != "") {
		res.push_back(cur);
	}
	return res;
}

inline void FileSystem::extractFSFromFile(vector<FileSystemNode>& treeNodeDataList, vector<int>& parentIndexArray, ifstream& f) {
	bool readMetadata = true, readParentIndexArray = true;
	int metadataBegin = NODE_METADATA_BEGIN;
	int parentIndexArrayBegin = 0;
	string line;
	for (int i = 0; getline(f, line); i++) {
		// First lines from fs file contain metadata
		if (readMetadata) {
			int propertyN = i - metadataBegin;
			if (propertyN == 0) {
				treeNodeDataList.push_back(FileSystemNode());
				unsigned int id_parsed = stoi(line);

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].id = id_parsed;
			}
			if (propertyN == 1) {
				unsigned int size_parsed = stoi(line);

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].size = size_parsed;
			}
			if (propertyN == 2) {
				bool isDir_parsed = stoi(line);

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].isDir = isDir_parsed;
			}
			if (propertyN == 3) {
				string name_parsed = line;

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].name = name_parsed;
			}
			if (propertyN == 4) {
				string type_parsed = line;

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].type = type_parsed;
			}
			if (propertyN == 5) {
				time_t created_parsed = 0;
				// TODO: get parsed  

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].created = created_parsed;
			}
			if (propertyN == 6) {
				time_t lastChange_parsed = 0;
				// TODO: get parsed  

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].lastChange = lastChange_parsed;
			}
			if (propertyN == 7) {
				string contents_parsed = line;
				// TODO: get parsed  

				// modify last element
				treeNodeDataList[treeNodeDataList.size() - 1].contents = contents_parsed;
			}
			if (propertyN == 8) {
				metadataBegin = i + 1;
				// If line marks begin of parent index aray - switch mode to reading parentIndexArray
				if (line == NODE_PARENT_INDEX_BEGIN_FLAG) {
					readMetadata = false;
					parentIndexArrayBegin = i + 1;
					readParentIndexArray = true;
					continue;
				}
			}
		}
		// Read parent index array of fs, contained in only one line
		else if (readParentIndexArray) {
			//cout << "Reading parentIndexArray" << endl;
			string curStr;
			int k = line.length();
			for (int i = 0; i < k; i++) {
				if (line[i] == ' ') {
					parentIndexArray.push_back(stoi(curStr));
					curStr = "";
				}
				else {
					curStr.push_back(line[i]);
				}
			}
			if (curStr != "") {
				parentIndexArray.push_back(stoi(curStr));
			}
			break;
		}
		//cout << line << endl;
	}
}

inline void FileSystem::getChildrenArray(vector<int>& parentIndexArray, vector<vector<int>>& childrenOf) {
	int k = parentIndexArray.size();
	// Create tree from metadata and metadata parent index aray
	for (int i = 0; i < k; i++) {
		childrenOf.push_back(vector<int>());
	}
	//cout << "NODESN: " << childrenOf.size() << endl << endl;
	//cout << "Beggining conversion from parentArray to children lists:" << endl;
	for (int i = 0; i < k; i++) {
		if (parentIndexArray[i] != -1) {
			//cout << i << " child to " << parentIndexArray[i] << endl;
			childrenOf[parentIndexArray[i]].push_back(i);
		}
	}
}

// Traverses tree and calls cur.addChild for each child a node

inline void FileSystem::constructTreeWith(vector<int>& parentIndexArray, vector<vector<int>> childrenOf, vector<FileSystemNode>& treeNodeDataList) {
	// Construct tree using instance of Tree<FileMetadataNode>::Iterator

	stack<int> currentPath;
	vector<bool> visited;

	// Start traversal from the root which is always on index 0
	currentPath.push(0);
	file_count++;
	cur.addChild(treeNodeDataList[0]);

	int k = parentIndexArray.size();
	for (int i = 0; i < k; i++) {
		visited.push_back(false);
	}
	while (!currentPath.empty()) {
		int curNode = currentPath.top();

		// Return back up the tree
		if (visited[curNode]) {
			// Move up only if not on root
			if (parentIndexArray[curNode] != -1) {
				// Move iterator up
				cur.moveToParent();
			}
			// Remove node index 
			currentPath.pop();
		}
		else {
			// Creates children of curNode
			k = childrenOf[curNode].size();
			bool newlyVisited = false;
			// Find first unvisited child - create node in Tree, move iterator to this child
			// If no child is unvisited - current node is marked as visited and cur goes up (to its parent)
			for (int i = 0; i < k; i++) {
				if (!visited[childrenOf[curNode][i]]) {
					//cout << "To parent " << curNode << endl;
					//cout << "Adding child " << treeNodeDataList[childrenOf[curNode][i]].name << endl;
					cur.addChild(treeNodeDataList[childrenOf[curNode][i]]);
					file_count++;

					cur.moveToChild(cur.childrenCount() - 1);
					currentPath.push(childrenOf[curNode][i]);
					newlyVisited = true;
					break;
				}
			}
			if (!newlyVisited) {
				visited[curNode] = true;
			}
		}
	}
}

// Returns iterator on folder, found by tracing a path of string representing directory names
// If path cannot be traced - returns detachhed empty TreeIterator

inline Tree<FileSystemNode>::TreeIterator FileSystem::tracePath(const vector<string>& folderNameList, bool fromRoot) {
	Tree<FileSystemNode>::TreeIterator temp = cur;
	if (fromRoot) {
		temp = fs.begin();
	}
	// For each string from folderNameList
	int k = folderNameList.size();
	for (int i = fromRoot; i < k && temp.getCur().isDir; i++) {
		int l = temp.childrenCount();
		bool forward = false;
		// If PATH_UP_STR is found - moveUp
		if (folderNameList[i] == PATH_UP_STR) {
			if (!temp.hasParent()) {
				cerr << "Invalid path" << endl;
				return Tree<FileSystemNode>::TreeIterator();
			}
			temp.moveToParent();
			forward = true;
		}
		else if (folderNameList[i] == PATH_STAY_STR) {
			forward = true;
		}
		else {
			// Search for child with matching name
			for (int j = 0; j < l; j++) {
				//cout << temp.getChild(j).name << " " << folderNameList[i] << endl;
				// First child that matches name and is a directory is selected

				if (temp.getChild(j).isDir && temp.getChild(j).name == folderNameList[i]) {
					temp.moveToChild(j);
					forward = true;
					break;
				}
			}
		}
		if (!forward) {
			// Empty iterator when path is invalid
			cerr << "Invalid path" << endl;
			return 	Tree<FileSystemNode>::TreeIterator();
		}
	}
	return temp;
}

inline Tree<FileSystemNode>::TreeIterator FileSystem::relativePathIter(const vector<string>& folderNameList) {
	return tracePath(folderNameList, false);
}

inline Tree<FileSystemNode>::TreeIterator FileSystem::absolutePathIter(const vector<string>& folderNameList) {
	return tracePath(folderNameList, true);
}

inline Tree<FileSystemNode>::TreeIterator FileSystem::getDirIter(const string& path) {
	vector<string> folderNameList = getFolderNameListFromPath(path);
	if (folderNameList[0] == "/") {
		//cout << "asdf" << endl;
		return absolutePathIter(folderNameList);
	}
	else {
		return relativePathIter(folderNameList);
	}
}

// Constructor creates file system from pathToDB

inline FileSystem::FileSystem() {
	fs = Tree<FileSystemNode>();
	cur = fs.begin();
	file_count = 0;
}

inline bool FileSystem::cd(const string& path) {
	Tree<FileSystemNode>::TreeIterator res = getDirIter(path);
	if (res.isDetached()) {
		return false;
	}
	else {
		cur = getDirIter(path);
		return true;
	}
}

inline string FileSystem::getCurPath() {
	Tree<FileSystemNode>::TreeIterator temp = cur;
	string path = "";
	while (temp.hasParent()) {
		path = temp.getCur().name + PATH_DIR_SEP + path;
		temp.moveToParent();
	}
	path = PATH_ROOT_SYM + path;
	return path;
}

inline vector<string> FileSystem::getDirFiles(const string& path) {
	Tree<FileSystemNode>::TreeIterator temp = getDirIter(path);
	vector<string> res;
	if (temp.isDetached()) {
		cerr << "Invalid path" << endl;
		return res;
	}
	int k = temp.childrenCount();
	for (int i = 0; i < k; i++) {
		res.push_back(temp.getChild(i).getFullName());
	}
	return res;
}

// Finds whether a child name is unique in current directory

inline int FileSystem::getChildIndex(const string file_name_type) {
	int k = cur.childrenCount();

	FileSystemNode curChild;
	int res = -1;
	for (int i = 0; i < k; i++) {
		curChild = cur.getChild(i);
		if (file_name_type == curChild.name + FILE_TYPE_SEP + curChild.type) {
			res = i;
		}
	}
	return res;
}

// Create file in current working dir

inline void FileSystem::createFile(const string& file_name_type, const string& file_name_contents) {

	if (getChildIndex(file_name_type) == -1) {
		int sep_index = file_name_type.find(FILE_TYPE_SEP);
		string file_name = file_name_type.substr(0, sep_index);
		string file_type = file_name_type.substr(sep_index + 1, file_name_type.length() - sep_index);

		/// TODO: fix time info
		FileSystemNode added(file_count, file_name_contents.length(), false, file_name, file_type, 0, 0, file_name_contents);
		file_count++;
		cur.addChild(added);
	}
	else {
		cerr << "File " << file_name_type << " already exists." << endl;
	}
}

inline void FileSystem::createDir(const string& folder_name) {
	if (getChildIndex(folder_name + FILE_TYPE_SEP + DIRECTORY_TYPE_STR) == -1) {
		/// TODO: fix time info
		FileSystemNode added(file_count, 0, true, folder_name, DIRECTORY_TYPE_STR, 0, 0, "");
		file_count++;
		cur.addChild(added);
	}
	else {
		cerr << "Directory" << folder_name << " already exists." << endl;
	}
}

// Removes child of current directory, only if it is an empty directory
inline void FileSystem::removeDir(const string& folder_name, bool shouldBeEmpty) {
	int i = getChildIndex(folder_name + FILE_TYPE_SEP + DIRECTORY_TYPE_STR);
	if (i == -1) {
		cerr << "Directory not found" << endl;
	}
	else {
		cur.moveToChild(i);
		if (shouldBeEmpty) {
			if (cur.childrenCount() == 0) {
				cur.removeCur();
				file_count--;
				return;
			}
			else {
				cur.moveToParent();
				cerr << "Directory is not empty" << endl;
				return;
			}
		}
		else {
			cur.removeCur();
			file_count--;
			return;
		}
	}
}

inline string FileSystem::getMetadataStr(const string& file_name_type) {
	int i = getChildIndex(file_name_type);
	if (i == -1) {
		cerr << "File not found" << endl;
		return "";
	}
	else {
		return cur.getChild(i).toString();
	}
}

inline void FileSystem::removeFile(const string& file_name_type) {
	int i = getChildIndex(file_name_type);
	if (i == -1) {
		cerr << "File not found" << endl;
	}
	else {
		cur.moveToChild(i);
		file_count--;
		cur.removeCur();
	}
}

inline bool FileSystem::tryToLoadFS(const string& DBPath, FileSystem& fs) {
	// Open fs file
	ifstream DBFile(DBPath);
	if (DBFile.is_open()) {
		vector<FileSystemNode> treeNodeDataList;
		vector<int> parentIndexArray;

		fs.extractFSFromFile(treeNodeDataList, parentIndexArray, DBFile);
		DBFile.close();

		// Create tree from metadata and metadata parent index aray
		vector<vector<int>> childrenOf;
		fs.getChildrenArray(parentIndexArray, childrenOf);

		fs.constructTreeWith(parentIndexArray, childrenOf, treeNodeDataList);
		return true;
	}
	else {
		cerr << "Unable to open file" << endl;
		return false;
	}
}
inline string FileSystemNode::getFullName() const {
	return name + (type == DIRECTORY_TYPE_STR ? "" : FILE_TYPE_SEP + type);
}

inline string FileSystemNode::toString() const {
	string res = ("File metadata:\nID: " + to_string(id));
	res += ("\n---Name: " + name);
	res += ("\n---Type: " + type);
	res += ("\n---Size: " + to_string(size));
	res += ("\n---IsDirectory: " + to_string(isDir));
	res += ("\n---Created: " + to_string(created));
	res += ("\n---LastChange: " + to_string(lastChange));
	return res;
}

