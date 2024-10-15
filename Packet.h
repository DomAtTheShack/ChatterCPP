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

    Packet();

    Packet(std::string usr, std::string msg);
    explicit Packet(std::string usr);
    ~Packet();

    static void serialize(char* buffer, size_t size, Packet* pkt);
    static void deserialize(const char* buffer, size_t bufferSize, Packet* pkt);

    size_t getSerializedSize() const;

    const std::string & getID() const;

    const std::string & getUsr() const;

    std::string getMsg() const;

    void setUsr(std::string inUSR);

    static bool receiveAll(int clientSocket, char *buffer, size_t totalBytes);
    static int sendPacket(Packet pkt, int* clientSocket);

    bool checkAndReceivePacket(int* clientSocket);

private:
    std::string id;
    std::string usr;
    std::string message;

    static std::string generateUSERID();
    static std::string getComputerHostname();
    static std::string getInternalIPAddress();

    static std::string md5(const std::string &str);

    static void deserializeString(size_t &offset, const char *buffer, size_t bufferSize, std::string *stringToDeser);

    static void serializeString(char *buffer, size_t bufferSize, size_t &offset, const std::string &stringToSerial);

    bool receiveAndDeserialize(int *clientSocket, char *buffer, size_t totalBytes, Packet *pkt);

    bool isSocketReady(int *clientSocket);
};


#endif //CHATTERCPP_PACKET_H
