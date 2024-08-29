//
// Created by dominichann on 8/28/24.
//

#ifndef CHATTERCPP_PACKET_H
#define CHATTERCPP_PACKET_H


#include <cstdio>
#include <string>
#include <netdb.h>

class Packet
{
public:

    Packet(std::string id, std::string msg);
    ~Packet();

    void serialize(char* buffer, size_t size) const;
    void deserialize(const char* buffer);

    size_t getSerializedSize() const;

    std::string getID() const;

    std::string getUsr() const;

    void setUsr(std::string inUSR);

    void generateUSERID();


private:
    std::string id;
    std::string usr;

    std::string getComputerHostname();
    std::string getInternalIPAddress();

    std::string md5(const std::string &str);
};


#endif //CHATTERCPP_PACKET_H
