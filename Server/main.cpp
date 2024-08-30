#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../Packet.h"

#define PORT 8083


using namespace std;

int initServerSocket(int &serverSocket);




int main() {
    int serverSocket;

    initServerSocket(serverSocket);
    cout << "Listing for Connections " << endl ;

    while (true) {
        listen(serverSocket, 5);

        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Error: Failed to accept client connection!" << std::endl;
            continue;
        }

        int bufferSize;
        size_t receivedSize = recv(clientSocket, &bufferSize, sizeof(bufferSize), 0);
        if (receivedSize != sizeof(bufferSize) || bufferSize <= 0) {
            std::cerr << "Error: Received incorrect buffer size!" << std::endl;
            close(clientSocket);
            continue;
        }

        char* buffer = new char[bufferSize];

        Packet::receiveAll(clientSocket, buffer, bufferSize);

        auto* pkt = new Packet();
        pkt->deserialize(buffer, bufferSize);

        std::cout << "Message from " << pkt->getUsr() << ": "
        << pkt->getMsg() << '\n';
        std::cout << "Deserialization Debug: ID = " << pkt->getID() << '\n';

        delete[] buffer;
        delete pkt;
        close(clientSocket);
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
