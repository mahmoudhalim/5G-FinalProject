#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <vector>
#if !defined(ECPRI_H)
#define ECPRI_H

class ECPRIPacket
{
private:
    std::string Message;
    std::string Payload;
    std::string PC_RTC;

public:
    static unsigned char SeqID;
    ECPRIPacket();
    ~ECPRIPacket();
    std::vector<std::string> GeneratePacket(std::unordered_map<std::string, std::string> OranConfig, int NumberOfFrames);
};

#endif // ECPRI_H
