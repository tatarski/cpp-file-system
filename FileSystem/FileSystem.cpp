#pragma once
#include <iostream>
#include <cstring>
#include "Tree.hpp"
#include "FileSystem.hpp"
#include "FileSystemCLI.hpp"

using namespace std;

int main()
{
	FileSystemCLI cli = FileSystemCLI("db1.txt");
	bool shouldContinue = true;
	while (shouldContinue) { 
		string commandStr;
		cout << "$ ";
		getline(cin, commandStr);
		cli.readCommand(commandStr);
	}
}
