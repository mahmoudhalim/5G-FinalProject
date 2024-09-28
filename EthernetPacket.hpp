#include <string>
#include <vector>
#include <unordered_map>
#if !defined(EthPacket_H)
#define EthPacket_H

 
class EthernetPacket{
private:
    double LineRate;
    unsigned int CaptureSizeMs;
    int MinNumOfIFGSPerPacket;
    std::string SourceAddress;
    std::string DestAddress;
    int MaxPacketSize;
    int BurstSize;
    int BurstPeriodicity_us;
    unsigned int TotalPackets;
    int BurstMode;

    long GetCRC(const std::string &data);
    std::string GetIFGs(int packetSize);
    void GenerateCRCTable();

public:
    static unsigned int CRCTable[256];
    EthernetPacket(std::unordered_map<std::string, std::string> EthConfig);
    ~EthernetPacket();
    void WriteFile(std::string output_file, std::unordered_map<std::string, std::string> OranConfig);
    std::vector<std::string> GeneratePacket(std::unordered_map<std::string, std::string> OranConfig, int NumberOfFrames);

    std::string ToString(); // for debugging
};

#endif // EthPacketGen_H
