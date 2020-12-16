#include <iostream>
#include <chrono>
#include <map>
#include "bplus_tree.h"

#define SIZE 200000

int main()
{
    bplus_tree<int, int> btree(25);
    std::map<int, int> dict;
    
    std::cout << "N = " << SIZE << std::endl;
    auto begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < SIZE; i++)
    {
        dict.insert(std::make_pair(i, i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "map init: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < SIZE; i++)
    {
        btree.insert(i, i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "btree init: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    uint64_t hits1 = 0;
    for (int i = 0; i < SIZE; i++)
    {
        hits1 += dict.contains(i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "map search: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    uint64_t hits2 = 0;
    for (int i = 0; i < SIZE; i++)
    {
        hits2 += btree.contains(i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "btree search: " << duration.count() << "ms" << std::endl;
}