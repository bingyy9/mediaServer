#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#define PORT 8111
#define MESSAGE_LEN 1024
//select fd的最大值1024
#define FD_SIZE 1024

int main(int argc, char* argv[]){
    int socketfd, accept_fd;
    int on = 1;
    int ret = -1;
    int backlog = 10;
    int pid = 0;
    char in_buff[MESSAGE_LEN] = {0, };

    struct sockaddr_in localaddr, remoteAddr;
    int events = 0, max_fd = -1;
    int curpos = -1;
    fd_set fd_sets;
    int accept_fds[FD_SIZE] = {-1, };
 
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1){
        std::cout << "Failed to create socket! " << std::endl;
        exit(-1);
    }

    //把socket设置成非租塞
    int flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
    max_fd = socketfd;

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
        FD_ZERO(&fd_sets);
        //把监听的fd添加到fdsets
        FD_SET(socketfd, &fd_sets);
        //把所有accept的accept fd添加到fdsets
        for(int i = 0; i < FD_SIZE; i++){
            if(accept_fds[i] != -1){
                //加入的同时需要更新nfds
                if(accept_fds[i] > max_fd){
                    max_fd = accept_fds[i];
                }
                FD_SET(accept_fds[i], &fd_sets);
            }
        }
        events = select(max_fd + 1, &fd_sets, NULL, NULL, NULL);
        if(events < 0){
             std::cout << "Failed to use select!" << std::endl;
             break;
        } else if(events == 0){
             std::cout << "timeout..." << std::endl;
             continue;
        } else if(events){
            if(FD_ISSET(socketfd, &fd_sets)){
                //如果是监听socket触发的事件，说明有新的建立请求来了，找空槽，插入
                for(int i = 0; i<FD_SIZE; i++){
                    if(accept_fds[i] == -1){ //找当前空的槽
                         curpos = i;  
                    }
                }
                socklen_t addr_len = sizeof(struct sockaddr); 
                accept_fd = accept(socketfd,  (struct sockaddr *)&remoteAddr, &addr_len);
                if(accept_fd != 0){
                    std::cout << "accept_fd success!" << accept_fd << std::endl;
                }
                //设置非租塞
                flags = fcntl(accept_fd, F_GETFL, 0);
                fcntl(accept_fd, F_SETFL, flags | O_NONBLOCK);
                accept_fds[curpos] = accept_fd; //插入空槽
            } 

            //如果事件是接收数据的socket
            for(int i=0; i<FD_SIZE; i++){
                if(accept_fds[i] != -1 && FD_ISSET(accept_fds[i], &fd_sets)){
                    //数据处理
                    memset(in_buff, 0, MESSAGE_LEN);
                    ret = recv(accept_fds[i], (void *)in_buff, MESSAGE_LEN, 0);
                    if(ret == 0){
                        close(accept_fds[i]);
                        break;
                    }
                    std::cout << "recv: " << in_buff << std::endl;
                    send(accept_fds[i], (void *)in_buff, MESSAGE_LEN, 0);
                }

            }
        }
    }

    close(socketfd);
 
    return 0;
}