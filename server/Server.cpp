#include <iostream>
#include <unistd.h>
#include "Server.h"

namespace bb {

Server::Server(){
    std::cout << "construct..." << std::endl;
}

Server::~Server(){
   std::cout << "destruct..." << std::endl;
}

void Server::run(){
    while(1) {

         std::cout << "run..." << std::endl;
         ::usleep(100000);  //sleep 1s
    }
}



}