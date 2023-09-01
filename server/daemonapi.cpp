#include <iostream>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    //0 改变目录， 0重定向/dev/null
    if(daemon(0, 0) == -1){
        std::cout << "error" << std::endl;
        exit(-1);
    }

    while(1){
        sleep(1);
    }
    return 0;
}