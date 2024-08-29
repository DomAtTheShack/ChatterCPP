//
// Created by dominichann on 8/28/24.
//

#include "Packet.h"
#include <cstdio>
#include <cstring>
#include <utility>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <netdb.h>
#include <iostream>

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

void Packet::createUserID()
{
    char host[256];
    char *IP;
    struct hostent *host_entry;
    int hostname;
    hostname = gethostname(host, sizeof(host)); //find the host name
    check_host_name(hostname);
    host_entry = gethostbyname(host); //find host information
    check_host_entry(host_entry);
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
    printf("Current Host Name: %s\n", host);
    printf("Host IP: %s\n", IP);
}

void Packet::check_host_entry(struct hostent * hostentry) { //find host info from host name
    if (hostentry == NULL){
        perror("gethostbyname");
        exit(1);
    }
}

void Packet::check_host_name(int hostname) { //This function returns host name for local computer
    if (hostname == -1) {
        perror("gethostname");
        exit(1);
    }
}



