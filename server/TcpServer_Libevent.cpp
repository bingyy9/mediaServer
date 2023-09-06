#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#define PORT 8111

void on_read_cb(struct bufferevent *bevent, void* ctx){
    struct evbuffer *input = NULL;
    struct evbuffer *output = NULL;
    input = bufferevent_get_input(bevent);
    output = bufferevent_get_output(bevent);
    evbuffer_add_buffer(output, input);
}

void on_accept_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr* addr, int socklen){
    struct bufferevent *bevent = NULL;
    struct event_base *base = NULL;

    base = evconnlistener_get_base(listener);
    bevent = bufferevent_socket_new(base, fd, 0);

    bufferevent_enable(bevent, EV_READ | EV_WRITE);
    bufferevent_setcb(bevent, on_read_cb, NULL, NULL, NULL);

}

int main(int argc, char* argv[]){
    struct event_base *base = NULL;
    struct sockaddr_in serveraddr;
    struct evconnlistener *listener = NULL;
    base = event_base_new();

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = PORT;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    listener = evconnlistener_new_bind(base, on_accept_cb, NULL, LEV_OPT_REUSEABLE, 10, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    event_base_dispatch(base);
    return 0;
}