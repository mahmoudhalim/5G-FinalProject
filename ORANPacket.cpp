#include "ORANPacket.hpp"
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
ORANPacket::ORANPacket(std::unordered_map<std::string, std::string> OranConfig)
{
    MaxNrb = std::stoi(OranConfig["MaxNrb"]);
    NrbPerPacket = std::stoi(OranConfig["NrbPerPacket"]);
    PayloadType = OranConfig["PayloadType"];
    PayloadFile = OranConfig["Payload"];
}

ORANPacket::~ORANPacket()
{
}

std::string ORANPacket::ReadPayloadFile()
{
    std::stringstream payload;
    std::ifstream input(PayloadFile);
    std::string sample;
    while (input >> sample)
    {
        payload << std::hex << (std::stoi(sample) & 0xFF);
    }
    return payload.str();
}

std::vector<std::string> ORANPacket::GeneratePacket(int NumberOfFrames, unsigned int size)
{
    std::vector<std::string> Packets;
    std::stringstream packet;
    unsigned int PacketsPerSymbol = std::ceil((double) MaxNrb / NrbPerPacket);
    unsigned int PacketsPerSlot = PacketsPerSymbol * 14;
    unsigned int PacketsPerSubframe = PacketsPerSlot * 2; // assume SCS = 30

    unsigned int subframeId = 0;
    unsigned int slotId = 0;
    unsigned int symbolId = 0;
    unsigned int startPrbu = 0;
    std::string payload = ReadPayloadFile();
    for (size_t i = 0; i < PacketsPerSubframe * NumberOfFrames * 10; i++)
    {
        packet << "00";
        packet << "00"; // frameId
        subframeId = i/ PacketsPerSubframe;
        slotId = i / PacketsPerSlot;
        symbolId = i / PacketsPerSymbol;

        unsigned int octet11 = (subframeId << 4) + (slotId >> 2);
        packet << std::hex << std::setw(2) << std::setfill('0') << (octet11 & 0xFF);

        unsigned int octet12 = (slotId << 6) + symbolId;
        packet << std::hex << std::setw(2) << std::setfill('0') << (octet12 & 0xFF);
        packet << "00"; // octet13

        unsigned int octet14 = startPrbu >> 8;
        packet << std::hex << std::setw(2) << std::setfill('0') << (octet14 & 0x03);

        unsigned int octet15 = startPrbu;
        packet << std::hex << std::setw(2) << std::setfill('0') << (octet15 & 0xFF);

        unsigned int octet16 = std::min(MaxNrb - startPrbu, NrbPerPacket);
        startPrbu += NrbPerPacket;
        startPrbu %= NrbPerPacket;

        packet << std::hex << std::setw(2) << std::setfill('0') << (octet16 & 0xFF);
        packet << payload.substr(i * NrbPerPacket, octet16 * 12);
        // Check Size
        std::cout << packet.str().size() / 2 << std::endl;
        if(packet.str().size() / 2 > size){
            std::cerr << "packetGenerator: O-RAN Packet size exceeds available size. Consider using smaller value of NrbPerPacket\n";
            return -1;
        }
        Packets.push_back(packet.str());
        packet.str("");
        packet.clear();
        }
        return Packets;
}
