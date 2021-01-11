#include "pch.h"
#include "CppUnitTest.h"
#include "..\FileSystem\TreeToTest.hpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
namespace FileSystemunittests
{
	// All tests are done with Tree<string> 
	TEST_CLASS(Tree)
	{
	public:
		
		TEST_METHOD(addChild)
		{
			Tree_<string> t;
			Tree_<string>::Node* r = t.addChild("root", nullptr);
			Assert::AreSame(r, t.root);
		}
	};
}
