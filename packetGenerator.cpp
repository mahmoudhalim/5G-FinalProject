#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "parser.hpp"
#include "EthernetPacket.hpp"
#include "ORANPacket.hpp"
#include "ECPRIPacket.hpp"
using std::string;
using std::unordered_map;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "packetGenerator: setup_file output_file\n";
        return -1;
    }

    std::string input = argv[1];
    std::string output = argv[2];

    unordered_map<string, unordered_map<string, string>> config = parseFile(input);

    // Print the parsed attributes
    // for (const auto &object : config)
    // {
    //     std::cout << object.first << " {\n";
    //     for (const auto &pair : object.second)
    //     {
    //         std::cout << pair.first << " : " << pair.second << std::endl;
    //     }
    //     std::cout << "}\n";
    // }
    EthernetPacket *eth = new EthernetPacket(config["Eth"]);
    // std::cout << eth->ToString();
    eth->WriteFile(output, config["Oran"]);
    return 0;
}