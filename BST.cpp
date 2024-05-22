#include "BST.h"
#include <cstddef>
#include <utility>
#include <iostream>

BinarySearchTree::Node::Node(Key key, Value value, Node *parent, Node *left, Node *right)
    : keyValuePair(key, value), parent(parent), left(left), right(right) { }

BinarySearchTree::Node::Node(const Node &other) {
    keyValuePair = other.keyValuePair;
    parent = other.parent;
    
    if (other.left == nullptr) {
        left = nullptr;
    } else {
        left = new Node(*other.left);
    }
    if (other.right == nullptr) {
        right = nullptr;
    } else {
        right = new Node(*other.right);
    }
}

bool BinarySearchTree::Node::operator==(const Node &other) const
{
    return keyValuePair == other.keyValuePair &&
           parent == other.parent &&
           left == other.left &&
           right == other.right;

}

void BinarySearchTree::Node::insert(const Key &key, const Value &value)
{   
    bool wasDeleted = deleteEmptyNode(this);
    if (key < keyValuePair.first) {
        if (!left) {
            left = new Node(key, value, this);
        } else {
            left->insert(key, value);
        }
    } else {
        if (!right) {
            right = new Node(key, value, this);
        } else {
            right->insert(key, value);
        }
    }

    if (wasDeleted) {
        createEmptyNode(this); 
    }
}

void BinarySearchTree::Node::erase(const Key &key) {
    Node* curr = this;
    Node* t = nullptr;

    while (curr != nullptr && curr->keyValuePair.first != key) {
        if (key < curr->keyValuePair.first)
            curr = curr->left;
        else
            curr = curr->right;
    }

    if (curr == nullptr)
        return;

    // child free
    if (curr->left == nullptr && curr->right == nullptr) {
        if (curr->parent == nullptr) {
            delete curr;
            return;
        }
        if (curr->keyValuePair.first < curr->parent->keyValuePair.first)
            curr->parent->left = nullptr;
        else
            curr->parent->right = nullptr;
        delete curr;
        return;
    }

    // один ребенок
    if (curr->left != nullptr ^ curr->right != nullptr) {
        t = curr->left ? curr->left : curr->right;
        curr->keyValuePair = t->keyValuePair;
        if (t->left)
            t->left->parent = curr;
        if (t->right)
            t->right->parent = curr;
        curr->left = t->left;
        curr->right = t->right;
    }

    // два ребенка
    if (curr->left != nullptr && curr->right != nullptr) {
        t = curr->right;
        while (t->left != nullptr)
            t = t->left;
        curr->keyValuePair = t->keyValuePair;
        if (t->right) {
            t->right->parent = t->parent;
        }
        if (t->parent != curr) {
            t->parent->left = t->right;
        } else {
            t->parent->right = t->right;
        }
    }

    if (t != nullptr)
        delete t;
}


BinarySearchTree::BinarySearchTree(const BinarySearchTree &other){    
    *this = other;
}

BinarySearchTree& BinarySearchTree::operator=(const BinarySearchTree &other){
    if (this != &other){

        bool wasDeleted = deleteEmptyNode(_root);
        bool wasDeletedOther = deleteEmptyNode(other._root);

        deBST(_root); 
        _size = other._size; 
        _root = new Node(*other._root);
        reCopy(_root, other._root);

        if (wasDeletedOther)
            createEmptyNode(other._root);
        if (wasDeleted)
            createEmptyNode(_root);
    }
    return *this;
}

BinarySearchTree::BinarySearchTree(BinarySearchTree &&other) noexcept {
    *this = std::move(other);
}

BinarySearchTree &BinarySearchTree::operator=(BinarySearchTree &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    _size = other._size;
    _root = other._root;

    other._size = 0;
    other._root = nullptr;

    return *this;
}

BinarySearchTree::~BinarySearchTree()
{
    if (_root) {
        deleteEmptyNode(_root);
        deBST(_root); 
    }
}

BinarySearchTree::Iterator::Iterator(Node *node) : _node(node) { }

std::pair<Key, Value> &BinarySearchTree::Iterator::operator*() {
    return _node->keyValuePair;
}

const std::pair<Key, Value> &BinarySearchTree::Iterator::operator*() const {
    return _node->keyValuePair;
}

std::pair<Key, Value> *BinarySearchTree::Iterator::operator->() {
    return &_node->keyValuePair;
}

const std::pair<Key, Value> *BinarySearchTree::Iterator::operator->() const {
    return &_node->keyValuePair;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++() {
    if (_node->right) {
        if (_node->right->empty) {
            _node = _node->right;
            return *this;
        }
        if (_node->right->left) {
           _node = _node->right->left;
           while (_node->left != nullptr) {
            _node = _node->left;
           }
        } else {
            _node = _node->right;
        }
    } else {
        Key tmp = _node->keyValuePair.first;
        if (_node->parent != nullptr) {
            _node = _node->parent;
            while (_node->keyValuePair.first <= tmp && _node->parent != nullptr) {
                _node = _node->parent;
            }
        }
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(){ 
    if (_node->empty) {
        _node = _node->parent;
        return *this;
    }
    if (_node->left) {
        if (_node->left->right) {
           _node = _node->left->right;
           while (_node->right != nullptr) {
            _node = _node->right;
           }
        } else {
            _node = _node->left;
        }
    } else {
        Key tmp = _node->keyValuePair.first;
        if (_node->parent) {
            _node = _node->parent;
            while (_node->keyValuePair.first > tmp && _node->parent) {
                _node = _node->parent;
            }
        }
    }
    return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

bool BinarySearchTree::Iterator::operator==(const Iterator &other) const {
    return _node == other._node;
}

bool BinarySearchTree::Iterator::operator!=(const Iterator &other) const {
    return !(_node == other._node);
}

BinarySearchTree::ConstIterator::ConstIterator(const BinarySearchTree::Node *node) : _node(node) { }

const std::pair<Key, Value> &BinarySearchTree::ConstIterator::operator*() const {
    return _node->keyValuePair;
}

const std::pair<Key, Value> *BinarySearchTree::ConstIterator::operator->() const {
    return &_node->keyValuePair;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++() {
    if (_node->right) {
        if (_node->right->empty) {
            _node = _node->right;
            return *this;
        }
        if (_node->right->left) {
           _node = _node->right->left;
           while (_node->left != nullptr) {
            _node = _node->left;
           }
        } else {
            _node = _node->right;
        }
    } else {
        Key tmp = _node->keyValuePair.first;
        if (_node->parent != nullptr) {
            _node = _node->parent;
            while (_node->keyValuePair.first <= tmp && _node->parent != nullptr) {
                _node = _node->parent;
            }
        }
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++(int) {
    ConstIterator temp = *this;
    ++(*this);
    return temp;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(){ 
    if (_node->empty) {
        _node = _node->parent;
        return *this;
    }
    if (_node->left) {
        if (_node->left->right) {
           _node = _node->left->right;
           while (_node->right != nullptr) {
            _node = _node->right;
           }
        } else {
            _node = _node->left;
        }
    } else {
        Key tmp = _node->keyValuePair.first;
        if (_node->parent) {
            _node = _node->parent;
            while (_node->keyValuePair.first > tmp && _node->parent) {
                _node = _node->parent;
            }
        }
    }
    return *this;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(int) {
    ConstIterator temp = *this;
    --(*this);
    return temp;
}

bool BinarySearchTree::ConstIterator::operator==(const ConstIterator &other) const {
    return _node == other._node;
}

bool BinarySearchTree::ConstIterator::operator!=(const ConstIterator &other) const {
    return !(_node == other._node);
}

void BinarySearchTree::insert(const Key &key, const Value &value) {
    _size++;
    if (!_root) {
        _root = new Node(key, value);
        createEmptyNode(_root);
        _size = 1;
    } else {
        _root->insert(key, value);
    }
}

void BinarySearchTree::erase(const Key& key) {
    if (_root == nullptr)
        return;

    while(find(key) != end()) {
        bool wasDeleted = deleteEmptyNode(_root);
        _root->erase(key);
        _size -= 1;
        if (_size == 0) {
            _root = nullptr;
            return;
        }
        if (wasDeleted) {
            createEmptyNode(_root);
        }
    }
}

BinarySearchTree::ConstIterator BinarySearchTree::find(const Key &key) const {
   Node* curNode = _root;
   while (curNode->keyValuePair.first != key) {
        if (key < curNode->keyValuePair.first) {
            curNode = curNode->left;
        } else {
            curNode = curNode->right;
        }
        if (!curNode) {
            return cend();
        }
    }
    return BinarySearchTree::ConstIterator(curNode);
}

BinarySearchTree::Iterator BinarySearchTree::find(const Key &key) {
   Node* curNode = _root;
   while (curNode->keyValuePair.first != key) {
        if (key < curNode->keyValuePair.first) {
            curNode = curNode->left;
        } else {
            curNode = curNode->right;
        }
        if (!curNode) {
            return end();
        }
    }
    return BinarySearchTree::Iterator(curNode);
}

std::pair<BinarySearchTree::Iterator, BinarySearchTree::Iterator> BinarySearchTree::equalRange(const Key &key) {
    Iterator it_start = find(key);
    if (it_start == end()) {
        return std::make_pair(it_start, it_start);
    }
    Iterator it_end = it_start;
    while (it_end != end() && it_end->first == key) {
        ++it_end;
    }
    return std::make_pair(it_start, it_end);
}

std::pair<BinarySearchTree::ConstIterator, BinarySearchTree::ConstIterator> BinarySearchTree::equalRange(const Key &key) const {
    ConstIterator lower = [this, key]() {
        ConstIterator it = cbegin();
        while (it != cend() && key > it->first) {
            ++it;
        }
        return it;
    } ();

    ConstIterator upper = [this, key]() {
        ConstIterator it = cbegin();
        while (it != cend() && !(key < it->first)) {
            ++it;
        }
        return it;
    } ();

    return std::make_pair(lower, upper);
}


//! Получить итератор на элемент с наименьшим ключем в дереве
BinarySearchTree::ConstIterator BinarySearchTree::min() const {
    Node* currNode = _root;

    while (currNode->left && currNode->left->keyValuePair.first < currNode->keyValuePair.first) {
        currNode = currNode->left;
    }
    return ConstIterator(currNode);
}

//! Получить итератор на элемент с наибольшим ключем в дереве
BinarySearchTree::ConstIterator BinarySearchTree::max() const {
    Node* currNode = _root;

    while (currNode->right && currNode->right->keyValuePair.first > currNode->keyValuePair.first) {
        currNode = currNode->right;
    }
    return ConstIterator(currNode);
}
//! Получить итератор на элемент с ключем key с наименьшим значением 
BinarySearchTree::ConstIterator BinarySearchTree::min(const Key &key) const {
    auto range = equalRange(key);
    ConstIterator minIterator = range.second;

    for (auto it = range.first; it != range.second; ++it) {
        if (minIterator == range.second || it->second < minIterator->second) {
            minIterator = it;
        }
    }

    return minIterator;
}

//! Получить итератор на элемент с ключем key с наибольшим значением
BinarySearchTree::ConstIterator BinarySearchTree::max(const Key &key) const {
    auto range = equalRange(key);
    ConstIterator maxIterator = range.first;

    for (auto it = range.first; it != range.second; ++it) {
        if (it->second > maxIterator->second) {
            maxIterator = it;
        }
    }

    return maxIterator;
}

BinarySearchTree::Iterator BinarySearchTree::begin() {
    Node* currNode = _root;

    while (currNode->left) {
        currNode = currNode->left;
    }
    return Iterator(currNode);
}

BinarySearchTree::Iterator BinarySearchTree::end() {
    Node* curNode = _root;
    while (curNode->right != nullptr){
        curNode = curNode->right;
    }
    return BinarySearchTree::Iterator(curNode);
}

BinarySearchTree::ConstIterator BinarySearchTree::cbegin() const {
    Node* currNode = _root;

    while (currNode->left) {
        currNode = currNode->left;
    }
    return ConstIterator(currNode);
}

BinarySearchTree::ConstIterator BinarySearchTree::cend()const {
    Node* curNode = _root;
    while (curNode->right != nullptr) {
        curNode = curNode->right;
    }
    return BinarySearchTree::ConstIterator(curNode);
}

size_t BinarySearchTree::size() const {
    return _size;
}

void BinarySearchTree::Node::output_node_tree(size_t prefix) const {
    if (this == nullptr)
        return;

    if (right != nullptr and !right->empty)
        right->output_node_tree(prefix + 1);

    for (size_t i = 0; i < prefix; ++i)
        std::cout << "\t";

    std::cout << keyValuePair.first << std::endl;

    if (left != nullptr)
        left->output_node_tree(prefix + 1);
}


void BinarySearchTree::output_tree() {
    if (_root != nullptr)
        _root->output_node_tree(0);
}

void createEmptyNode(BinarySearchTree::Node* root) {
    BinarySearchTree::Node* maxNode = root;
    if (root == nullptr) {
        return;
    }
    while (maxNode->right) {
        maxNode = maxNode->right;
    }
    if (!maxNode->empty){ 
        maxNode->right = new BinarySearchTree::Node(0,0, maxNode, nullptr, nullptr);
        maxNode->right->empty = true;
    }
}

bool deleteEmptyNode(BinarySearchTree::Node* root){
    BinarySearchTree::Node* maxNode = root;
    
    if (root == nullptr || !root->right) {
        return false;
    }

    while(maxNode->right->right != nullptr) {
        maxNode = maxNode->right;
    }

    BinarySearchTree::Node* endNode = maxNode->right;
    if (endNode->empty) {
        // maxNode->right->empty = false;
        maxNode->right = nullptr;
        delete endNode;
        return true;
    }
    else {
        return false;
    }
}

void deBST(BinarySearchTree::Node* node) {
    if (node == nullptr) {
        return;
    }

    deBST(node->left);
    deBST(node->right);

    delete node;
}

void reCopy(BinarySearchTree::Node* node, BinarySearchTree::Node* other) {
    if (other == nullptr) {
        return;
    }

    if (other->left) {
        if (node->left == nullptr || node->left->keyValuePair.first != other->left->keyValuePair.first) {
            node->left = new BinarySearchTree::Node(other->left->keyValuePair.first, other->left->keyValuePair.second, node);
            node->left->empty = other->left->empty;
        }
        reCopy(node->left, other->left);
    }

    if (other->right) {
        if (node->right == nullptr || node->right->keyValuePair.first != other->right->keyValuePair.first) {
            node->right = new BinarySearchTree::Node(other->right->keyValuePair.first, other->right->keyValuePair.second, node);
            node->right->empty = other->right->empty;
        }
        reCopy(node->right, other->right);
    }
}
