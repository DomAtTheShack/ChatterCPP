//
// Created by dominichann on 8/30/24.
//

#ifndef CHATTERCPP_USER_H
#define CHATTERCPP_USER_H


#include <string>

class User
        {
public:
    User(int* clientSocket, std::string username);

    ~User();


private:
    int* clientSocket;
    std::string user;


};


#endif //CHATTERCPP_USER_H
