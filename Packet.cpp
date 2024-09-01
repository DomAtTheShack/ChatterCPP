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
#include <iostream>

using namespace std;

Packet::Packet()
{
    usr = "SERVER_DEBUG";
    id = "YOU SHOULDN'T SEE THIS";
    message = "REALLY YOU SHOULD WATCH BLUEY";
}

Packet::Packet(std::string usr, string msg)
{
    this->id = generateUSERID();
    this->usr = std::move(usr);
    message = std::move(msg);
}
Packet::~Packet() = default;

// Serialize the object into a byte buffer
void Packet::serialize(char* buffer, size_t size) {
    // Ensure the buffer size is sufficient
    if (size < getSerializedSize()) return;

    // Copy integer
    // memcpy(buffer + offset, &id, sizeof(id));
    // offset += sizeof(id);

    // Copy float
    // memcpy(buffer + offset, &value, sizeof(value));
    // offset += sizeof(value);

    // Copy array of doubles
    // memcpy(buffer + offset, numbers, sizeof(numbers));

    size_t offset = 0;

    serializeString(buffer, size, offset, id);
    serializeString(buffer, size, offset, usr);
    serializeString(buffer, size, offset, message);
}

void Packet::serializeString(char* buffer, size_t bufferSize, size_t& offset, const std::string& stringToSerial) {
    // Determine the length of the string
    size_t strLength = stringToSerial.size();

    // Check if the buffer has enough space to hold the string length and the string data
    if (offset + sizeof(strLength) + strLength > bufferSize) {
        std::cerr << "Error: Buffer size is too small to serialize the string!" << std::endl;
        return;
    }

    // Serialize the length of the string
    memcpy(buffer + offset, &strLength, sizeof(strLength));
    offset += sizeof(strLength);

    // Serialize the string data
    memcpy(buffer + offset, stringToSerial.c_str(), strLength);
    offset += strLength;
}

// Deserialize the object from a byte buffer
void Packet::deserialize(const char* buffer, size_t bufferSize) {
    size_t offset = 0;

    // Read the strings into the buffer
    deserializeString(offset, buffer, bufferSize, &id);

    deserializeString(offset, buffer, bufferSize, &usr);

    deserializeString(offset, buffer, bufferSize, &message);


}

void Packet::deserializeString(size_t& offset, const char* buffer, size_t bufferSize, string* stringToDeser)
{
    // Ensure the buffer is large enough to read the string length
    if (offset + sizeof(size_t) > bufferSize) {
        std::cerr << "Error: Buffer size too small to read string length!" << std::endl;
        return;
    }

    // Read the string length
    size_t strLength;
    memcpy(&strLength, buffer + offset, sizeof(strLength));
    offset += sizeof(strLength);

    // Check if the string length is reasonable and within the buffer limits
    if (offset + strLength > bufferSize) {
        std::cerr << "Error: Buffer size too small to read string data!" << std::endl;
        return;
    }

    // Assign the string data
    stringToDeser->assign(buffer + offset, strLength);
    offset += strLength;
}

// Calculate the total size required for serialization
size_t Packet::getSerializedSize() const {
    return id.size() + (sizeof(size_t) * 3) + usr.size() + message.size() ;
}

const string& Packet::getID() const {
    return id;
}

const string& Packet::getUsr() const {
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

std::string Packet::getInternalIPAddress() {
    struct ifaddrs *ifaddr, *ifa;
    char ip[INET_ADDRSTRLEN];  // Buffer for the IP address (IPv4)

    // Get the list of network interfaces
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return "";
    }

    std::string internalIP;

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
                internalIP = ip;  // Store the IP
                break;  // Found the internal IP, exit the loop
            }
        }
    }

    // Free the linked list
    freeifaddrs(ifaddr);

    return internalIP.empty() ? "" : internalIP;
}

std::string Packet::md5(const std::string &str)
{
    unsigned char hash[MD5_DIGEST_LENGTH];

    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, str.c_str(), str.size());
    MD5_Final(hash, &md5);

    std::stringstream ss;

    for(unsigned char i : hash){
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( i );
    }
    return ss.str();
}


std::string Packet::generateUSERID()
{
    string idBefore = getComputerHostname() + " " + getInternalIPAddress();
    return md5(idBefore);
}

bool Packet::receiveAll(int clientSocket, char* buffer, size_t totalBytes) {
    size_t bytesRead = 0;  // Total bytes read so far
    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 seconds timeout
    timeout.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));


    while (bytesRead < totalBytes) {
        // Attempt to read the remaining number of bytes
        ssize_t result = recv(clientSocket, buffer + bytesRead, totalBytes - bytesRead, 0);

        if (result < 0) {
            // Error occurred during recv
            std::cerr << "Error: Failed to receive data!" << std::endl;
            return false;
        } else if (result == 0) {
            // Connection closed gracefully by the client
            std::cerr << "Connection closed by the client!" << std::endl;
            return false;
        }

        // Update the total bytes read so far
        bytesRead += result;
    }

    // If we reach this point, we have successfully read all expected data
    return true;
}

int Packet::sendPacket(Packet pkt, int clientSocket)
{
    size_t bufferSize = pkt.getSerializedSize();
    char* buffer = new char[bufferSize];

    pkt.serialize(buffer, bufferSize);

    if(send(clientSocket, &bufferSize, sizeof(int), 0) == -1) return -1;
    return send(clientSocket, buffer, bufferSize, 0);
}

std::string Packet::getMsg() const {
    return message;
}



