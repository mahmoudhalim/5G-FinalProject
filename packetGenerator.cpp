#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "parser.hpp"
using std::string;
using std::unordered_map;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "packetGenerator: error: Setup File is missing\n";
        return -1;
    }

    std::string input = argv[1];

    unordered_map<string, unordered_map<string, string>> attributes = parseFile(input);

    // Print the parsed attributes
    for (const auto &object : attributes)
    {
        std::cout << object.first << " {\n";
        for (const auto &pair : object.second)
        {
            std::cout << pair.first << " : " << pair.second << std::endl;
        }
        std::cout << "}\n";
    }

    return 0;
}