//
// Created by dominichann on 8/30/24.
//

#include "User.h"
#include <unistd.h>
#include <utility>

User::User(int* clientSocket, std::string username, std::string ID)
{
    this->clientSocket = clientSocket;
    user = std::move(username);
    this->ID = std::move(ID);
}


User::~User()
{
    close(*clientSocket);
    delete this;
}

std::string User::getID() const {
    return ID;
}
