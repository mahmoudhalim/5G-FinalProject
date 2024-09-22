#include <sstream>
#include <fstream>
#include <iostream>
#include "EthernetPacket.hpp"

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
    BurstSize = std::stoi(EthConfig["BurstSize"]);
    BurstPeriodicity_us = std::stoi(EthConfig["BurstPeriodicity_us"]);
}

EthernetPacket::~EthernetPacket()
{
}
void EthernetPacket::WriteFile(std::string output_file){
    std::ofstream Output(output_file);
    if(!Output.is_open()){
        std::cout << "packetGenerator: Error opening " << output_file<<std::endl;
        exit(-1);
    }
    std::string packet = GeneratePacket();
    double burstDuration = (packet.size()/2 * BurstSize * 8) / LineRate*1e-3; // us
    double IFGsDuration = BurstPeriodicity_us - burstDuration; // us
    double IFGsSize = (double)IFGsDuration * LineRate * 1000 / 8;
    // std::cout << "burstDuration: " << burstDuration << std::endl;
    // std::cout << "packet:: " << packet.size()/2 << std::endl;
    // std::cout << "ifgssize: " << IFGsSize << std::endl;
    // std::cout << "ifgs: "<< IFGsDuration<<std::endl;
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
    Output.close();
}

std::string EthernetPacket::GeneratePacket(){
    std::stringstream packet;
    packet << std::hex << PREAMBLE;
    packet << std::hex << DestAddress.substr(2);
    packet << std::hex << SourceAddress.substr(2);
    packet << std::hex << ETHERTYPE;
    int header_size = packet.str().size()/2;
    // generate payload
    for (int i = 0; i < this->MaxPacketSize - header_size - 4; i++)
    {
        packet << "00";
    }
    packet << GetCRC();
    packet << GetIFGs(packet.str().size() + this->MinNumOfIFGSPerPacket*2);
    return packet.str();
}

std::string EthernetPacket::GetCRC(){
    return "DEADBEEF"; //TODO
}
std::string EthernetPacket::GetIFGs(int packetSize){
    int ByteOffset = 4 - (packetSize / 2 % 4);
    if(ByteOffset == 4)
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