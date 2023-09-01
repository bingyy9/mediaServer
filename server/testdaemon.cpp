#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void daemonize(){
    pid_t pid = fork();
    if(pid < 0){
        printf("cannot create child process!\n");
    }

    if(pid > 0){
        exit(0);
    }

    //child process
    setsid();
    if(chdir("/") < 0){
        printf("cannot change dir!\n");
        exit(0);
    }

    int fd = open("/dev/null", O_RDWR);
    //重定向文件描述
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    return;
}

int main(int argc, char* argv[]){
    daemonize();
    while(1){
        sleep(1);
    }
    return 0;
}