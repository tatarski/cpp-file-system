#include<vector>
using namespace std;

template <typename T>
class Tree {
public:
	struct Node {
		Node(T d) : data(d), children(vector<Node*>()) {}
		T data;
		vector<Node*> children;
	};
	Node* root;
	Tree() { root = nullptr}
	isEmpty() {
		return root == nullptr;
	}
	addChild(T data, Node* parent) {
		Node* added = new Node(T);
	}
};