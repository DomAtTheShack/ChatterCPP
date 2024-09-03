//
// Created by dominichann on 8/30/24.
//

#ifndef CHATTERCPP_USER_H
#define CHATTERCPP_USER_H


#include <string>

class User
        {
public:
    User(int* clientSocket, std::string username, std::string ID);

    ~User();

    std::string getID() const;
private:
    int* clientSocket;
    std::string user;
    std::string ID;


};


#endif //CHATTERCPP_USER_H
