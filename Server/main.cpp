#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../Packet.h"

using namespace std;

int initServerSocket(int &serverSocket);




int main() {
    int serverSocket;

    initServerSocket(serverSocket);
    cout << "Listing for Connections " << endl ;
    listen(serverSocket, 5);


    int clientSocket = accept(serverSocket, nullptr, nullptr);
    int bufferSize;
    recv(clientSocket, &bufferSize, 32, 0);
    char* buffer = new char[bufferSize];
    ssize_t bytesReceived = recv(clientSocket, buffer, 32, 0);


    Packet* pkt = new Packet(0, "NULL");
    pkt->deserialize(buffer);

    cout << "Message from client: " << pkt->getUsr() << " " << pkt->getID() << endl;

    close(serverSocket);
    return 0;
}

int initServerSocket(int &serverSocket)
{
    int port = 8081;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
    {
        cerr << " ERROR: Failed to Bind Server socket to port " << port << endl;
    }


    return 0;
}
