#include <sstream>
#include <fstream>
#include <iostream>
#include "EthernetPacket.hpp"

const long PREAMBLE = 0xFB555555555555D5;
const long ETHERTYPE = 0xAEFE; // eCPRI

EthernetPacket::EthernetPacket(std::unordered_map<std::string, std::string> EthConfig)
{
    this->LineRate = std::stod(EthConfig["LineRate"]);
    this->CaptureSizeMs = std::stoi(EthConfig["CaptureSizeMs"]);
    this->MinNumOfIFGSPerPacket = std::stoi(EthConfig["MinNumOfIFGsPerPacket"]);
    this->SourceAddress = EthConfig["SourceAddress"];
    this->DestAddress = EthConfig["DestAddress"];
    this->MaxPacketSize = std::stoi(EthConfig["MaxPacketSize"]);
    this->BurstSize = std::stoi(EthConfig["BurstSize"]);
    this->BurstPeriodicity_us = std::stoi(EthConfig["BurstPeriodicity_us"]);
    this->SetTotalPackets();
}

EthernetPacket::~EthernetPacket()
{
}
void EthernetPacket::WriteFile(std::string output_file){
    std::ofstream Output(output_file);
    if(!Output.is_open()){
        std::cout << "packetGenerator: Error opening " << output_file;
        exit(-1);
    }
    std::string packet = this->GeneratePacket();
    for (size_t i = 0; i < packet.size(); i+=8)
    {
        std::string line = packet.substr(i, 8);
        Output << line << std::endl;
    }
    Output.close();
}

void EthernetPacket::SetTotalPackets()
{
    double PacketTime = this->MaxPacketSize / this->LineRate;
    int PacketPerBurst = this->BurstPeriodicity_us / PacketTime;
    this->TotalPackets = PacketPerBurst*this->CaptureSizeMs*1000/this->BurstPeriodicity_us;
}
std::string EthernetPacket::GeneratePacket(){
    std::stringstream packet;
    packet << std::hex << PREAMBLE;
    packet << std::hex << this->DestAddress.substr(2);
    packet << std::hex << this->SourceAddress.substr(2);
    packet << std::hex << ETHERTYPE;
    int header_size = packet.str().size()/2 - sizeof(PREAMBLE);
    // generate payload
    for (int i = 0; i < this->MaxPacketSize - header_size - 4; i++)
    {
        packet << "00";
    }
    packet << this->GetCRC();
    packet << this->GetIFGs(packet.str().size() + this->MinNumOfIFGSPerPacket*2);
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