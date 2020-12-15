#include <iostream>
#include <chrono>
#include <map>
#include "bplus_tree.h"

int main()
{
    bplus_tree<int, int> btree(20);
    std::map<int, int> dict;

    auto begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        dict.insert(std::make_pair(i, i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "map init: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        btree.insert(i, i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "btree init: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        dict.contains(i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "map search: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        btree.contains(i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "btree search: " << duration.count() << "ms" << std::endl;
}