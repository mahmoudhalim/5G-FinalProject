#include "ECPRIPacket.hpp"
#include "ORANPacket.hpp"
#include <unordered_map>
unsigned char ECPRIPacket::SeqID = 0x00;

ECPRIPacket::ECPRIPacket()
{
    Message = "00";
    PC_RTC = "00";
}

ECPRIPacket::~ECPRIPacket()
{
}

std::vector<std::string> ECPRIPacket::GeneratePacket(std::unordered_map<std::string, std::string> OranConfig, int NumberOfFrames)
{
    ORANPacket *oran = new ORANPacket(OranConfig);
    std::vector<std::string> OranPackets = oran->GeneratePacket(NumberOfFrames);
    std::vector<std::string> eCPRIPackets;
    std::stringstream packet;
    for (size_t i = 0; i < OranPackets.size(); i++)
    {
        packet << "00";
        packet << Message;
        packet << std::hex << std::setw(4) << std::setfill('0') << (OranPackets.at(i).size() / 2 & 0xFFFF) ;
        packet << PC_RTC;
        packet << std::hex << std::setw(4) << std::setfill('0') << (i & 0xFFFF);
        packet << OranPackets.at(i);
        eCPRIPackets.push_back(packet.str());
        packet.str("");
        packet.clear();
    }

    return eCPRIPackets;
}

