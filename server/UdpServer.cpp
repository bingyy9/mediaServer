#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8112 
#define MESSAGE_LEN 1024

int main(int argc, char* argv[]){
    int socketfd, accept_fd;
    int on = 1;
    int ret = -1;
    int backlog = 10;
    struct sockaddr_in localaddr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char in_buff[MESSAGE_LEN] = {0, };

    //SOCK_DGRAM is for UDP
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketfd == -1){
        std::cout << "Failed to create socket! " << std::endl;
        exit(-1);
    }

    localaddr.sin_family = AF_INET;
    localaddr.sin_port = PORT;
    localaddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(localaddr.sin_zero), 8);
    
    ret = bind(socketfd, (struct sockaddr *)&localaddr, sizeof(struct sockaddr));
    if(ret == -1){
        std::cout << "Failed to bind addr !" << std::endl;
        exit(-1);
    }
    std::cout << "Udp Server running ..." << std::endl;

    for(;;){
        ret = recvfrom(socketfd, (void *)in_buff, MESSAGE_LEN, 0, (struct sockaddr*)&client_addr, &client_len);
        if(ret < 0){
            std::cout << "recvfrom failed!" << std::endl;
            break;
        }

        std::cout << "recv: " << in_buff << std::endl;

        sendto(socketfd, (void *)in_buff, MESSAGE_LEN, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    }

    close(socketfd);
    return 0;
}