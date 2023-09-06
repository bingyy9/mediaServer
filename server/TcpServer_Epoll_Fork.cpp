#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define PORT 8111
#define MESSAGE_LEN 1024
#define MAX_EVENTS 20
#define TIMEOUT 500
#define MAX_PROCESS 4

int main(int argc, char* argv[]){
    int socketfd, accept_fd;
    int on = 1;
    int ret = -1;
    int backlog = 10;
    struct sockaddr_in localaddr, remoteAddr;
    char in_buff[MESSAGE_LEN] = {0, };
    pid_t pid = -1;

    int flags;
    int epoll_fd;
    struct epoll_event ev, events[MAX_EVENTS];
    int event_number;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1){
        std::cout << "Failed to create socket! " << std::endl;
        exit(-1);
    }
    //设置非租塞
    flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);

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

    // epoll_fd = epoll_create(256); 
    // ev.events = EPOLLIN; //监听输入事件 监听的使用默认的水平触发
    // //当新的连接事件来的时候，就会触发EPOLLIN，然后通过ev.data.fd知道是哪个文件描述符的事件，如果是socketfd就是创建新的连接的事件
    // ev.data.fd = socketfd;  
    // epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socketfd, &ev);

    for(int i = 0; i < MAX_PROCESS; i++){
        if(pid != 0){
            //父进程
            pid = fork();
        }
    }

    if(pid == 0){

        epoll_fd = epoll_create(256); 
        ev.events = EPOLLIN; //监听输入事件 监听的使用默认的水平触发
        //当新的连接事件来的时候，就会触发EPOLLIN，然后通过ev.data.fd知道是哪个文件描述符的事件，如果是socketfd就是创建新的连接的事件
        ev.data.fd = socketfd;  
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socketfd, &ev);

        for(;;){
            event_number = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
            //event_number就是监听到有消息的数量
            for(int i=0; i<event_number; i++){
                if(events[i].data.fd == socketfd){
                    //创建新的连接
                    socklen_t addr_len = sizeof(struct sockaddr); 
                    accept_fd = accept(socketfd,  (struct sockaddr *)&remoteAddr, &addr_len);
                    if(accept_fd != 0){
                        std::cout << "accept_fd success!" << accept_fd << std::endl;
                    }
                    //设置非租塞
                    flags = fcntl(accept_fd, F_GETFL, 0);
                    fcntl(accept_fd, F_SETFL, flags | O_NONBLOCK);

                    ev.events = EPOLLIN | EPOLLET; //监听输入事件 边缘触发
                    //当新的连接事件来的时候，就会触发EPOLLIN，然后通过ev.data.fd知道是哪个文件描述符的事件，如果是socketfd就是创建新的连接的事件
                    ev.data.fd = accept_fd;  
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
                } else if(events[i].events & EPOLLIN){
                    do{
                        //读取处理
                        memset(in_buff, 0, MESSAGE_LEN); 
                        ret = recv(events[i].data.fd, (void *)in_buff, MESSAGE_LEN, 0);
                        if(ret == 0){
                            close(events[i].data.fd);
                        }

                        if(ret == MESSAGE_LEN){
                            //缓冲区满了， 后面还有数据。继续读
                            std::cout << "maybe have data...." << std::endl;
                        }

                        if(ret > 0){
                            std::cout << "recv: " << in_buff << std::endl;
                            send(events[i].data.fd, (void *)in_buff, MESSAGE_LEN, 0); 
                        }
                    
                    } while(ret < 0 && errno == EINTR);

                    if(ret < 0){
                        switch(errno){
                            case EAGAIN: //
                                break;
                            default:
                                break;
                        }
                    }
                }
            }  
        }
    } else {
        //父进程等待子进程结束，如果不等待，父进程直接结束。子进程就是孤儿进程了。
        do{
           pid = waitpid(-1, NULL, 0);//等待所有子进程
        } while (pid == -1); //所有子进程都结束，才会返回-1

        close(socketfd);

        //其实父进程在主核性能是最强劲的，这样做也是对资源的浪费。
    }
    return 0;
}