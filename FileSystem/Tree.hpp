#pragma once;
#include<iostream>
#include<vector>
#include<string>
#include<stack>
using namespace std;

template <typename T>
class Tree {
private:
	struct Node {
		Node(T d) : data(d), children(vector<Node*>()), parent(nullptr) {}
		Node() : data(T()), children(vector<Node*>()), parent(nullptr) {}
		T data;
		vector<Node*> children;
		Node* parent;
	};
	Node* root;

	// Create a new node and add it as a child of parent
	// Returns the added node
	Node* addChild(T data, Node* parent);
	// Remove remove i-th child of parent node
	void removeChild(Node* parent, unsigned int i);
	// Function that deletes nodes and their children
	void rem_rec(Node* n);
	// Trace a path of T starting from start node
	// Obsolete method: can be done specifically for FileSystem using iterator
	Node* traceFrom(stack<T> path, Node* start);

public:
	class TreeIterator {
	private:
		// Cur is a reference to a node of a Tree<T>
		Node* cur;
		Tree<T>* tree;
		bool isEmptyThrow();

	public:
		TreeIterator();
		// Iterators begin on root
		TreeIterator(Tree<T>* tree);;
		TreeIterator& operator=(const TreeIterator& old);
		// Get data from current node
		T getCur();;
		// Get i-th child of current node
		T& getChild(int i);
		bool hasParent();
		// Get number of children of current node
		int childrenCount();;
		// Move cursor to i-th child of current Node
		void moveToChild(unsigned int i);
		// Move cursor to parent of current Node
		void moveToParent();
		// Remove current node and delete all children
		// If root is removed, then cur=nullptr
		void removeCur();
		// Push child to current node children
		void addChild(T data);
		bool isDetached();
	};
private:
	// Used to copy node and its children
	Node* copy_node_rec(const Node* oldN, Node* parent = nullptr);
public:
	Tree<T>();
	Tree<T>(const Tree<T>& old);
	Tree<T>& operator= (const Tree<T>& old);
	bool isEmpty() const;
	Tree<T>::TreeIterator begin();
	~Tree();
};

template<typename T>
bool Tree<T>::isEmpty() const {
	return root == nullptr;
}

template<typename T>
typename inline Tree<T>::TreeIterator Tree<T>::begin() {
	return Tree<T>::TreeIterator(this);
}

template<typename T>
Tree<T>::~Tree() {
	if (!isEmpty()) {
		rem_rec(root);
	}
}

template<typename T>
Tree<T>::Tree() : root(nullptr) {}

template<typename T>
Tree<T>::Tree(const Tree<T>& old) {
	if (!isEmpty()) {
		rem_rec(root);
	}
	if (!old.isEmpty()) {
		root = copy_node_rec(old.root);
	}
}



// Create a new node and add it as a child of parent
// Returns the added node
template<typename T>
typename Tree<T>::Node* Tree<T>::addChild(T data, Node* parent) {
	if (parent == nullptr) {
		if (isEmpty()) {
			root = new Node(data);
			root->parent = nullptr;
			return root;
		}
		else {
			cerr << "Only root can have no parent";
			throw "Only root can have no parent";
			return nullptr;
		}
	}

	Node* added = new Node(data);
	added->parent = parent;
	parent->children.push_back(added);
	return added;
}

// Remove remove i-th child of parent node
template<typename T>
void Tree<T>::removeChild(Node* parent, unsigned int i) {
	if (parent->children.size() > i) {
		rem_rec(parent->children[i]);
		// Remove i-th element of parent->children
		parent->children[i] = parent->children[parent->children.size() - 1];
		parent->children.pop_back();
	}
	else {
		cerr << "Index out of range.";
		throw "Index out of range.";
	}
}

// Function that deletes nodes and their children
template<typename T>
void Tree<T>::rem_rec(Node* n) {
	int k = n->children.size();
	for (int i = 0; i < k; i++) {
		rem_rec(n->children[i]);
	}
	delete n;
}

// Trace a path of T starting from start node
// Obsolete method: can be done specifically for FileSystem using iterator
template<typename T>
typename Tree<T>::Node* Tree<T>::traceFrom(stack<T> path, Node* start) {
	if (path.isEmpty()) {
		return start;
	}
	else {
		int k = start->children.length();
		for (int i = 0; i < k; i++) {
			if (path[i] == start->data) {
				return traceFrom(path.pop());
			}
		}
	}
	return nullptr;
}

// Used to copy node and its children
template<typename T>
typename Tree<T>::Node* Tree<T>::copy_node_rec(const Node* oldN, Node* parent) {
	Node* copied = new Node(oldN->data);
	copied->parent = parent;

	int k = oldN->children.size();
	for (int i = 0; i < k; i++) {
		copied->children.push_back(copy_node_rec(oldN->children[i], copied));
	}
	return copied;
}

template<typename T>
Tree<T>& Tree<T>::operator=(const Tree<T>& old) {
	if (this != &old) {
		if (!isEmpty()) {
			rem_rec(root);
		}
		if (!old.isEmpty()) {
			root = copy_node_rec(old.root);
		}
	}
	return *this;
}

template<typename T>
inline bool Tree<T>::TreeIterator::isEmptyThrow() {
	if (tree->isEmpty() || tree == nullptr) {
		cerr << "Tree is empty";
		throw "Tree is empty";
	}
	else {
		return false;
	}
}

template<typename T>
inline Tree<T>::TreeIterator::TreeIterator() :cur(nullptr), tree(nullptr) {}

// Iterators begin on root
template<typename T>
inline Tree<T>::TreeIterator::TreeIterator(Tree<T>* tree) : cur(tree->root), tree(tree) {}


// Get data from current node
template<typename T>
inline T Tree<T>::TreeIterator::getCur() {
	if (isEmptyThrow()) {
		return T();
	}
	return cur->data;
}

// Get i-th child of current node
template<typename T>
inline T& Tree<T>::TreeIterator::getChild(int i) {
	if (isEmptyThrow()) {
		T res = T();
		return res;
	}
	if (i >= 0 && i < this->childrenCount()) {
		return cur->children[i]->data;
	}
	else {
		cerr << "Index out of bounds";
		throw "Index out of bounds";
	}
}

template<typename T>
inline bool Tree<T>::TreeIterator::hasParent() {
	return cur->parent != nullptr;
}

// Get number of children of current node
template<typename T>
inline int Tree<T>::TreeIterator::childrenCount() {
	if (tree->isEmpty()) {
		return 0;
	}
	return cur->children.size();
}

// Move cursor to i-th child of current Node
template<typename T>
inline void Tree<T>::TreeIterator::moveToChild(unsigned int i) {
	if (isEmptyThrow()) {
		cout << "empty" << endl;
		return;
	}
	if (i < cur->children.size()) {
		cur = cur->children[i];
	}
	else {
		cerr << "Index out of range";
		throw "Index out of range";
	}
}

// Move cursor to parent of current Node
template<typename T>
inline void Tree<T>::TreeIterator::moveToParent() {
	if (isEmptyThrow()) {
		return;
	}
	if (cur->parent) {
		cur = cur->parent;
	}
	else {
		cerr << "Current node has no parent";
		throw "Current node has no parent";
	}
}

// Remove current node and delete all children
// If root is removed, then cur=nullptr
template<typename T>
inline void Tree<T>::TreeIterator::removeCur() {
	if (isEmptyThrow()) {
		return;
	}
	if (cur->parent != nullptr) {
		Node* parent = cur->parent;
		int k = parent->children.size(), child_i = 0;
		for (int i = 0; i < k; i++) {
			if (parent->children[i] == cur) {
				child_i = i;
				break;
			}
		}
		parent->children[child_i] = parent->children[k - 1];
		parent->children.pop_back();
		tree->rem_rec(cur);
		cur = parent;
	}
	else {
		cerr << "Cannot remove root node" << endl;
	}

	// If whole tree is deleted
	if (tree->isEmpty()) {
		cur = nullptr;
	}
}

// Push child to current node children
template<typename T>
inline void Tree<T>::TreeIterator::addChild(T data) {
	// If tree is empty - move cursor to root 
	if (tree->isEmpty()) {
		tree->addChild(data, nullptr);
		cur = tree->root;
	}
	else {
		tree->addChild(data, cur);
	}
}

// See whether TreeIterator is linked to a tree
template<typename T>
inline bool Tree<T>::TreeIterator::isDetached() {
	return tree == nullptr || cur == nullptr;
}

template<typename T>
typename Tree<T>::TreeIterator& Tree<T>::TreeIterator::operator=(const TreeIterator& old) {
	if (this != &old) {
		// When copying iterator, remove cur pointer
		cur = nullptr;
		// If they are pointing to one memory - copy old.cur pointer
		if (tree == old.tree) {
			cur = old.cur;
			tree = old.tree;
		}
		else {
			tree = old.tree;
			if (tree != nullptr) {
				cur = tree->root;
			}
		}
	}
	return *this;
}

