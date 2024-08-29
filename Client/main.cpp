//
// Created by dominichann on 8/28/24.
//

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../Packet.h"

int main()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8081);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    auto* pkt = new Packet("69", "NICE");
    pkt->generateUSERID();
    int bufferSize = pkt->getSerializedSize();
    char* buffer = new char[bufferSize];
    pkt->serialize(buffer, bufferSize);
    send(clientSocket, &bufferSize, sizeof(int), 0);
    send(clientSocket, buffer, bufferSize, 0);
    delete pkt;

    close(clientSocket);
}

