#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "parser.hpp"
using std::string;
using std::unordered_map;

string removeWhiteSpace(string line){
    string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
    line.erase(end_pos, line.end());
    return line;
}

unordered_map<string, unordered_map<string, string>> parseFile(const string &input)
{
    std::ifstream stream(input);
    string line;
    unordered_map<string, unordered_map<string, string>> dict;

    while (std::getline(stream, line))
    {
        line = removeWhiteSpace(line);
        // remove comments
        size_t pos = line.find('/');
        line = line.substr(0, pos);
        
        pos = line.find(".");
        if (pos == string::npos) continue;
        string object = line.substr(0, pos);
        string data = line.substr(pos + 1);
        pos = data.find("=");
        if (pos == string::npos) continue;
        string field = data.substr(0, pos);
        string value = data.substr(pos + 1);
        dict[object][field] = value;
    }

    return dict;
}
