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

/**
 *  This initializes the packet with default values that are never to be seen
 */
Packet::Packet()
{
    usr = "SERVER_DEBUG";
    id = "YOU SHOULD WATCH THIS SHOW";
    message = "REALLY YOU SHOULD WATCH BLUEY";
}

/**
 * Creates a packet with just a user and this main use is for a initial connection packet on first communication
 * @param usr The username to be stored in this packet
 */
Packet::Packet(std::string usr)
{
    this->usr = std::move(usr);
    id = generateUSERID();
    message = "$CONNECT_PKT";
}

/**
 * The main packet to sent with the username the message and user ID
 */
Packet::Packet(std::string usr, string msg)
{
    this->id = generateUSERID();
    this->usr = std::move(usr);
    message = std::move(msg);
}

/**
 * Default deconstruct
 */
Packet::~Packet() = default;

/** Serialize the object into a byte buffer to be sent over a socket
 *
 * @param buffer the byte buffer array to be stored with the packet data
 * @param size the size of the packet itself to be sent
 * @param pkt the packet object to be serialized
 */
void Packet::serialize(char* buffer, size_t size, Packet* pkt)
{
    // Ensure the buffer size is sufficient
    if (size < pkt->getSerializedSize()) return;

    // Copy integer
    // memcpy(buffer + offset, &id, sizeof(id));
    // offset += sizeof(id);

    // Copy float
    // memcpy(buffer + offset, &value, sizeof(value));
    // offset += sizeof(value);

    // Copy array of doubles
    // memcpy(buffer + offset, numbers, sizeof(numbers));

    size_t offset = 0;

    serializeString(buffer, size, offset, pkt->id);
    serializeString(buffer, size, offset, pkt->usr);
    serializeString(buffer, size, offset, pkt->message);
}

/**
 * This method will serialize the string its given into byte data
 *
 * @param buffer the buffer array to be stored with the byte data of the string
 * @param bufferSize the size of the buffer to be checking
 * @param offset the offset of the array or the i value of the array index
 * @param stringToSerial the string itself to be serialized
 */
void Packet::serializeString(char* buffer, size_t bufferSize, size_t& offset, const std::string& stringToSerial)
{
    // Determine the length of the string
    size_t strLength = stringToSerial.size();

    // Check if the buffer has enough space to hold the string length and the string data
    if (offset + sizeof(strLength) + strLength > bufferSize)
    {
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
void Packet::deserialize(const char* buffer, size_t bufferSize, Packet* pkt)
{
    size_t offset = 0;

    // Read the strings into the buffer
    deserializeString(offset, buffer, bufferSize, &pkt->id);

    deserializeString(offset, buffer, bufferSize, &pkt->usr);

    deserializeString(offset, buffer, bufferSize, &pkt->message);


}

void Packet::deserializeString(size_t& offset, const char* buffer, size_t bufferSize, string* stringToDeser)
{
    // Ensure the buffer is large enough to read the string length
    if (offset + sizeof(size_t) > bufferSize)
    {
        std::cerr << "Error: Buffer size too small to read string length!" << std::endl;
        return;
    }

    // Read the string length
    size_t strLength;
    memcpy(&strLength, buffer + offset, sizeof(strLength));
    offset += sizeof(strLength);

    // Check if the string length is reasonable and within the buffer limits
    if (offset + strLength > bufferSize)
    {
        std::cerr << "Error: Buffer size too small to read string data!" << std::endl;
        return;
    }

    // Assign the string data
    stringToDeser->assign(buffer + offset, strLength);
    offset += strLength;
}

// Calculate the total size required for serialization
size_t Packet::getSerializedSize() const
{
    return id.size() + (sizeof(size_t) * 3) + usr.size() + message.size() ;
}

const string& Packet::getID() const
{
    return id;
}

const string& Packet::getUsr() const
{
    return usr;
}

std::string Packet::getMsg() const
{
    return message;
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

std::string Packet::getInternalIPAddress()
{
    struct ifaddrs *ifaddr, *ifa;
    char ip[INET_ADDRSTRLEN];  // Buffer for the IP address (IPv4)

    // Get the list of network interfaces
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return "";
    }

    std::string internalIP;

    // Iterate through the linked list of interfaces
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == nullptr) continue;

        // Check if the address is IPv4
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            // Convert the address to a readable format
            void* addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, addr, ip, sizeof(ip));

            // Skip loopback address
            if (strcmp(ip, "127.0.0.1") != 0)
            {
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

    for(unsigned char i : hash)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( i );
    }
    return ss.str();
}


std::string Packet::generateUSERID()
{
    string idBefore = getComputerHostname() + " " + getInternalIPAddress();
    return md5(idBefore);
}

bool Packet::receiveAll(int clientSocket, char* buffer, size_t length)
{
    ssize_t total = 0;
    ssize_t n;

    while (total < length)
    {
        n = recv(clientSocket, buffer + total, length - total, 0);
        if (n < 0)
        {
            std::cerr << "Error receiving data: " << strerror(errno) << '\n';
            return false; // Error occurred
        } else if (n == 0)
        {
            std::cerr << "Connection closed by the server." << '\n';
            return false; // Connection closed
        }
        total += n;
    }
    return true;
}

bool Packet::checkAndReceivePacket(int* clientSocket, Packet* pkt)
{
    size_t totalBytes = 8192;
    char* buffer = new char[totalBytes];  // Dynamically allocate buffer
    memset(buffer, 0, totalBytes);        // Initialize buffer to zero

    fd_set read_fds;    // File descriptor set
    FD_ZERO(&read_fds); // Clear the set
    FD_SET(*clientSocket, &read_fds); // Add client socket to the set

    // Timeout setup (optional)
    struct timeval timeout;
    timeout.tv_sec = 5;  // 5-second timeout
    timeout.tv_usec = 0;

    // Wait for the socket to be ready for reading
    int selectResult = select(*clientSocket + 1, &read_fds, NULL, NULL, &timeout);

    if (selectResult < 0)
    {
        std::cerr << "Error: select() failed! " << strerror(errno) << std::endl;
        delete[] buffer;  // Clean up
        return false;
    } else if (selectResult == 0)
    {
        // Timeout occurred, no data available
        delete[] buffer;  // Clean up
        return false; // No new packet has been sent
    }

    // Data is available to be read
    if (FD_ISSET(*clientSocket, &read_fds))
    {
        // Call receiveAll to receive the complete packet
        if (Packet::receiveAll(*clientSocket, buffer, totalBytes))
        {
            // Copy the data from the buffer to the object
            deserialize(buffer, totalBytes, pkt);
            std::cout << "Packet received successfully!" << '\n';
            delete[] buffer;  // Clean up
            return true;
        } else
        {
            std::cerr << "Failed to receive complete packet!" << '\n';
            delete[] buffer;  // Clean up
            return false;
        }
    }

    delete[] buffer;  // Clean up
    return false;
}

int Packet::sendPacket(Packet pkt, int* clientSocket)
{
    size_t bufferSize = pkt.getSerializedSize();
    char* buffer = new char[bufferSize];

    serialize(buffer, bufferSize, &pkt);

    // Send the size of the packet first
    if (send(*clientSocket, &bufferSize, sizeof(int), 0) == -1)
    {
        delete[] buffer;  // Clean up memory in case of error
        return -1;
    }

    // Send the actual packet data
    int result = send(*clientSocket, buffer, bufferSize, 0);

    delete[] buffer;  // Clean up memory after sending

    return result; // Return the result of the send call
}
