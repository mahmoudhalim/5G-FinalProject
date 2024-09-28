#include <unordered_map>
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#if !defined(ORAN_HPP)
#define ORAN_HPP
class ORANPacket
{
private:
    int MaxNrb;
    unsigned int NrbPerPacket;
    std::string PayloadType;
    std::string PayloadFile;
    std::string ReadPayloadFile();

public:
    ORANPacket(std::unordered_map<std::string, std::string> OranConfig);
    ~ORANPacket();
    std::vector<std::string> GeneratePacket(int NumberOfFrames, unsigned int size);
};

#endif // ORAN_HPP
