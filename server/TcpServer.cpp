#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8111
#define MESSAGE_LEN 1024

int main(int argc, char* argv[]){
    int socketfd, accept_fd;
    int on = 1;
    int ret = -1;
    int backlog = 10;
    struct sockaddr_in localaddr, remoteAddr;
    char in_buff[MESSAGE_LEN] = {0, };


    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1){
        std::cout << "Failed to create socket! " << std::endl;
        exit(-1);
    }

    ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(ret == -1){
        std::cout << "Failed to set sockete options" << std::endl;
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

    ret = listen(socketfd, backlog);
    if(ret == -1){
        std::cout << "listen fail!" << std::endl;
        exit(-1);
    } else {
         std::cout << "listen...." << std::endl;
    }

    for(;;){
        socklen_t addr_len = sizeof(struct sockaddr); 
        accept_fd = accept(socketfd,  (struct sockaddr *)&remoteAddr, &addr_len);
        if(accept_fd != 0){
            std::cout << "accept_fd success!" << accept_fd << std::endl;
        }

        for(;;){
            ret = recv(accept_fd, (void *)in_buff, MESSAGE_LEN, 0);
            if(ret == 0){
                break;
            }

            std::cout << "recv: " << in_buff << std::endl;

            send(accept_fd, (void *)in_buff, MESSAGE_LEN, 0);
        }

        close(accept_fd);  
    }

    close(socketfd);
    return 0;
}