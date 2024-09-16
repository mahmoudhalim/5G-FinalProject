#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;
#if !defined(parseFile_HPP)
#define parseFile_HPP
unordered_map<string, unordered_map<string, string>> parseFile(const string &input);

#endif // parseFile_HPP
