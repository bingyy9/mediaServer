#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#define PORT 8112
#define MESSAGE_LEN 1024

int main(int argc, char* argv[]){
    int ret = -1;
    int socketfd;
    struct sockaddr_in serveraddr;
    char sendbuf[MESSAGE_LEN] = {0, };
    char recvbuf[MESSAGE_LEN] = {0, };


    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketfd == -1){
        std::cout << "Failed to create socket! " << std::endl;
        exit(-1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = PORT;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(serveraddr.sin_zero), 8);
    
    while(1){
        memset(sendbuf, 0, MESSAGE_LEN);
        gets(sendbuf);
        ret = sendto(socketfd, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
        if(ret <= 0){
            std::cout << "Failed to send data to server !" << std::endl;
            break;
        }

        if(strcmp(sendbuf, "quit") == 0){
            break;
        }

        ret = recvfrom(socketfd, recvbuf, MESSAGE_LEN, 0, NULL, NULL);
        recvbuf[ret] = '\0';
        std::cout << "recvfrom: " << recvbuf << std::endl;
    }

   
    close(socketfd);
    return 0;
}