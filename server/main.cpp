#include <iostream>
#include "Server.h"
using namespace bb;

int main(int argc, char* argv[]){
    Server* server = new Server();
    if(server){
        server->run();
    }
    return 0;
}