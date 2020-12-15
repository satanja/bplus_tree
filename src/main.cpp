#include <iostream>
#include "bplus_tree.h"

int main()
{
    bplus_tree<int, int> btree(3);
    btree.insert(0, 0);
    btree.insert(2, 2);
    btree.insert(1, 1);
    btree.insert(3, 3);

    std::cout << btree.contains(2) << std::endl;
    std::cout << btree.at(2) << std::endl;
    std::cout << btree.contains(3) << std::endl;
    std::cout << btree.at(3) << std::endl;
    std::cout << btree.contains(1) << std::endl;
    std::cout << btree.at(1) << std::endl;
    std::cout << btree.contains(0) << std::endl;
    std::cout << btree.at(0) << std::endl;
}