//
// Created by dominichann on 8/28/24.
//

#include "Packet.h"
#include <cstdio>
#include <cstring>
#include <utility>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <ifaddrs.h>
#include <openssl/md5.h>
#include <sstream>
#include <iomanip>

using namespace std;

Packet::Packet(std::string id, std::string msg)
{
    this->id = std::move(id);
    usr = std::move(msg);
}
Packet::~Packet() = default;

// Serialize the object into a byte buffer
void Packet::serialize(char* buffer, size_t size) const {
    // Ensure the buffer size is sufficient
    if (size < getSerializedSize()) return;

    // Copy integer
    size_t offset = 0;
    memcpy(buffer + offset, &id, sizeof(id));
    offset += sizeof(id);

//    // Copy float
//    memcpy(buffer + offset, &value, sizeof(value));
//    offset += sizeof(value);

    // Copy string length and content
    size_t strLength = usr.size();
    memcpy(buffer + offset, &strLength, sizeof(strLength));
    offset += sizeof(strLength);
    memcpy(buffer + offset, usr.c_str(), strLength);
    offset += strLength;

//    // Copy array of doubles
//    memcpy(buffer + offset, numbers, sizeof(numbers));
}

// Deserialize the object from a byte buffer
void Packet::deserialize(const char* buffer) {
    size_t offset = 0;

    // Read integer
//    memcpy(&id, buffer + offset, sizeof(id));
//    offset += sizeof(id);

//    // Read float
//    memcpy(&value, buffer + offset, sizeof(value));
//    offset += sizeof(value);

    // Read string length and content
    size_t strLength;
    memcpy(&strLength, buffer + offset, sizeof(strLength));
    offset += sizeof(strLength);
    usr.assign(buffer + offset, strLength);
    offset += strLength;

    memcpy(&strLength, buffer + offset, sizeof(strLength));
    offset += sizeof(strLength);
    id.assign(buffer + offset, strLength);
    offset += strLength;

//    // Read array of doubles
//    memcpy(numbers, buffer + offset, sizeof(numbers));
}

// Calculate the total size required for serialization
size_t Packet::getSerializedSize() const {
    return sizeof(id) + sizeof(size_t) + usr.size() ;
}

std::string Packet::getID() const {
    return id;
}

std::string Packet::getUsr() const {
    return usr;
}

void Packet::setUsr(std::string inUSR)
{
    usr = std::move(inUSR);
}

string Packet::getComputerHostname()
{
    char host[256];
    int hostname;
    hostname = gethostname(host, sizeof(host)); //find the host name
    return host;
}

string Packet::getInternalIPAddress()
{
    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    char ip[INET_ADDRSTRLEN];  // Buffer for the IP address (IPv4)

    // Get the list of network interfaces
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return NULL;
    }

    // Iterate through the linked list of interfaces
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) continue;

        // Check if the address is IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            // Convert the address to a readable format
            void* addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, addr, ip, sizeof(ip));

            // Skip loopback address
            if (strcmp(ip, "127.0.0.1") != 0) {
                 return ip;
            }
        }
    }

    // Free the linked list
    freeifaddrs(ifaddr);
    return NULL;
}

std::string Packet::md5(const std::string &str){
    unsigned char hash[MD5_DIGEST_LENGTH];

    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, str.c_str(), str.size());
    MD5_Final(hash, &md5);

    std::stringstream ss;

    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( hash[i] );
    }
    return ss.str();
}


void Packet::generateUSERID()
{
    string idBefore = getComputerHostname() + " " + getInternalIPAddress();
    string idAfter = md5(idBefore);
}





