#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../Packet.h"
#include "User.h"
#include <vector>
#include <thread>
#include <mutex>

#define PORT 8080


using namespace std;

int initServerSocket(int &serverSocket);
void addUser(Packet* pkt, int* clientSocket);
int acceptConnection(int* clientSocket, const int* serverSocket);
int receviePacketSize(int* bufferSize, const int* clientSocket);

std::vector<User*> users = {};
std::mutex usersMutex;

int main() {
    int serverSocket;

    initServerSocket(serverSocket);
    cout << "Listing for Connections " << endl ;
    listen(serverSocket, 5);

    while (true) {

        int clientSocket;
        if(acceptConnection(&clientSocket, &serverSocket) == -1)
            continue;

        int bufferSize;
        if(receviePacketSize(&bufferSize, &clientSocket) == -1)
            continue;

        char* buffer = new char[bufferSize];

        if (!Packet::receiveAll(clientSocket, buffer, bufferSize)) {
            std::cerr << "Failed to receive complete packet data." << std::endl;
            close(clientSocket);
            delete[] buffer;
            continue;
        }

        auto* pkt = new Packet();
        pkt->deserialize(buffer, bufferSize);

        addUser(pkt, &clientSocket);


        delete[] buffer;
        delete pkt;
    }
    return 0;
}

int initServerSocket(int &serverSocket)
{

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
    {
        cerr << " ERROR: Failed to Bind Server socket to port " << PORT << endl;
    }


    return 0;
}

void addUser(Packet* pkt, int* clientSocket)
{
    users.push_back(new User(clientSocket, pkt->getUsr()));
}

int acceptConnection(int* clientSocket, const int* serverSocket) {
    *clientSocket = accept(*serverSocket, nullptr, nullptr);
    if (*clientSocket < 0) {
        std::cerr << "Error: Failed to accept client connection!" << std::endl;
        return -1;
    }
    return 0;
}

int receviePacketSize(int* bufferSize, const int* clientSocket)
{
    int receivedSize = recv(*clientSocket, bufferSize, sizeof(*bufferSize), 0);
    if (receivedSize != sizeof(*bufferSize) || *bufferSize <= 0) {
        std::cerr << "Error: Received incorrect buffer size!" << std::endl;
        close(*clientSocket);
        return -1;
    }
    return 0;
}