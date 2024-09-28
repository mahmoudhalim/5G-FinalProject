#include <sstream>
#include <fstream>
#include <iostream>
#include "EthernetPacket.hpp"
#include "ECPRIPacket.hpp"
const long PREAMBLE = 0xFB555555555555D5;
const long ETHERTYPE = 0xAEFE; // eCPRI

EthernetPacket::EthernetPacket(std::unordered_map<std::string, std::string> EthConfig)
{
    LineRate = std::stod(EthConfig["LineRate"]);
    CaptureSizeMs = std::stoi(EthConfig["CaptureSizeMs"]);
    MinNumOfIFGSPerPacket = std::stoi(EthConfig["MinNumOfIFGsPerPacket"]);
    SourceAddress = EthConfig["SourceAddress"];
    DestAddress = EthConfig["DestAddress"];
    MaxPacketSize = std::stoi(EthConfig["MaxPacketSize"]);
    BurstSize = 0;
    BurstPeriodicity_us = CaptureSizeMs * 1000;
    BurstMode = 0;
}

EthernetPacket::~EthernetPacket()
{
}
void EthernetPacket::WriteFile(std::string output_file, std::unordered_map<std::string, std::string> OranConfig)
{
    std::ofstream Output(output_file);
    if (!Output.is_open())
    {
        std::cout << "packetGenerator: Error opening " << output_file << std::endl;
        exit(-1);
    }
    unsigned int NumberOfFrames = CaptureSizeMs / 10;
    std::vector<std::string> ethPackets = GeneratePacket(OranConfig, NumberOfFrames);
    if (BurstMode)
    {
        std::string packet = ethPackets.at(0);
        double burstDuration = (packet.size() / 2 * BurstSize * 8) / LineRate * 1e-3; // us
        double IFGsDuration = BurstPeriodicity_us - burstDuration;                    // us
        double IFGsSize = (double)IFGsDuration * LineRate * 1000 / 8;
        size_t numOfBursts = CaptureSizeMs * 1000 / BurstPeriodicity_us;
        for (size_t i = 0; i < numOfBursts; i++)
        {
            for (int i = 0; i < BurstSize; i++)
            {
                // write packet
                for (size_t i = 0; i < packet.size(); i += 8)
                {
                    std::string line = packet.substr(i, 8);
                    Output << line << std::endl;
                }
            }
            // Write IFGS until next burst
            for (size_t i = 0; i < IFGsSize; i += 4)
                Output << "07070707\n";
        }
    }
    else {
        // write packet
        unsigned int BytesTransmitted = 0;
        for (size_t i = 0; i < ethPackets.size(); i++)
        {
            std::string packet = ethPackets.at(i);
            for (size_t i = 0; i < packet.size(); i += 8)
            {
                BytesTransmitted += 4;
                std::string line = packet.substr(i, 8);
                Output << line << std::endl;
            }
        }
        double IFGsDuration = CaptureSizeMs * 1000 - BytesTransmitted * 8 / LineRate * 1e-3;
        double IFGsSize = (double)IFGsDuration * LineRate * 1000 / 8;
        for (size_t i = 0; i < IFGsSize; i += 4)
            Output << "07070707\n";
    }
    Output.close();
}

std::vector<std::string> EthernetPacket::GeneratePacket(std::unordered_map<std::string, std::string> OranConfig, int NumberOfFrames)
{
    std::vector<std::string> EthPackets;
    ECPRIPacket *ecpri = new ECPRIPacket();
    std::vector<std::string> eCPRIPackets = ecpri->GeneratePacket(OranConfig, NumberOfFrames);
    std::stringstream packet;
    for (size_t i = 0; i < eCPRIPackets.size(); i++)
    {
        packet << std::hex << PREAMBLE;
        packet << std::hex << DestAddress.substr(2);
        packet << std::hex << SourceAddress.substr(2);
        packet << std::hex << ETHERTYPE;
        int header_size = packet.str().size() / 2;
        // generate payload
        if (BurstMode)
        {
            for (int i = 0; i < this->MaxPacketSize - header_size - 4; i++)
                packet << "00";
        }
        else
            packet << eCPRIPackets.at(i);

        packet << GetCRC();
        packet << GetIFGs(packet.str().size() + this->MinNumOfIFGSPerPacket * 2);
        EthPackets.push_back(packet.str());
        packet.str("");
        packet.clear();
    }
    

    return EthPackets;
}

std::string EthernetPacket::GetCRC()
{
    return "DEADBEEF"; // TODO
}
std::string EthernetPacket::GetIFGs(int packetSize)
{
    int ByteOffset = 4 - (packetSize / 2 % 4);
    if (ByteOffset == 4)
        ByteOffset = 0;
    int IFGsCount = this->MinNumOfIFGSPerPacket + ByteOffset;
    std::stringstream s;

    for (int i = 0; i < IFGsCount; i++)
    {
        s << "07";
    }
    return s.str();
}
std::string EthernetPacket::ToString()
{
    std::stringstream stream;
    stream << "Ethernet Configs:\n";
    stream << "LineRate: " << LineRate << " Gbps\n";
    stream << "CaptureSizeMs: " << CaptureSizeMs << " ms\n";
    stream << "MinNumOfIFGSPerPacket: " << MinNumOfIFGSPerPacket << '\n';
    stream << "SourceAddress: " << std::hex << SourceAddress << '\n';
    stream << "DestAddress: " << std::hex << DestAddress << '\n';
    return stream.str();
}