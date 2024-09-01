//
// Created by dominichann on 8/30/24.
//

#include "User.h"

#include <utility>

User::User(int *clientSocket, std::string username)
{
    this->clientSocket = clientSocket;
    user = username;
}


User::~User() = default;
