#include "BST.h"
#include <iostream>
#include <iterator>
using namespace std;

int main() {
    BinarySearchTree tree = BinarySearchTree();

    tree.insert(50, 50);
    tree.insert(30, 30);
    tree.insert(70, 70);
    tree.insert(20, 20);
    tree.insert(40, 40);
    tree.insert(60, 60);
    tree.insert(80, 80);

    

    tree.output_tree();

    return 0;
}