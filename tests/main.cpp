// tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "trie.h"

#include <iostream>
#include <fstream>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Two list files required.\n";
        return 1;
    }

    levenshteinsgate::Breathalyzer trie;

    {
        std::ifstream twl(argv[1]);

        std::string buf;
        while (twl >> buf)
        {
            trie.Insert(buf.c_str());
        }
    }

    int count = 0;

    std::ifstream in(argv[2]);

    clock_t start = clock();

    std::string buf;
    while (in >> buf)
    {
        count += trie.GetDistance(std::u32string(buf.begin(), buf.end()));
    }

    std::cout << count << " in " << double(clock() - start) / CLOCKS_PER_SEC << " seconds.\n";
}
