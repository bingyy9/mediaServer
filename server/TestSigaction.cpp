#include <iostream>
#include <signal.h>
#include <unistd.h>

void sighandle(int sig){
    std::cout << "received signal: " << sig << std::endl;
}

int main(int argc, char* argv[]){
    struct sigaction act, oact;
    act.sa_handler = sighandle;
    sigfillset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, &oact);
    pause();
    return 0;
}