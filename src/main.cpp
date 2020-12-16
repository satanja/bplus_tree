#include <iostream>
#include <chrono>
#include <map>
#include <random>
#include "bplus_tree.h"

#define SIZE 5000000

void build_query(int size, std::vector<int> &queries)
{
    queries.resize(size);
    for (int i = 0; i < size; i++)
    {
        queries[i] = i;
    }
    std::shuffle(queries.begin(), queries.end(), std::default_random_engine(0));
}

int main()
{   
    std::vector<int> queries;
    build_query(SIZE, queries);

    bplus_tree<int, int> btree(50);
    std::map<int, int> dict;
    
    std::cout << "N = " << SIZE << std::endl;
    auto begin = std::chrono::high_resolution_clock::now();
    for (int i : queries)
    {
        dict.insert(std::make_pair(i, i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "map init: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    for (int i : queries)
    {
        btree.insert(i, i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "btree init: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    uint64_t hits1 = 0;
    for (int i : queries)
    {
        hits1 += dict.contains(i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "map search: " << duration.count() << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    uint64_t hits2 = 0;
    for (int i : queries)
    {
        hits2 += btree.contains(i);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double, std::milli>(end - begin);
    std::cout << "btree search: " << duration.count() << "ms" << std::endl;
    std::cout << (hits1 == hits2) << std::endl;
}