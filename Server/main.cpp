#include <stdio.h>
#include <time.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

const int PORT = 2500;
const int BUFFER_SIZE = 1024;

void listener_cb(struct evconnlistener *, evutil_socket_t,
                 struct sockaddr *, int socklen, void *);
void conn_writecb(struct bufferevent *, void *);
void conn_readcb(struct bufferevent *, void *);
void conn_eventcb(struct bufferevent *, short, void *);
void delay(int ms);

int main(int argc, char **argv)
{
    printf("I am server\n");
#ifdef WIN32
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;

    struct evconnlistener *listener;
    struct event_base *base = event_base_new();
    if (!base)
    {
        printf("Could not initialize libevent\n");
        return 1;
    }
    listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
        LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr*)&sin,
        sizeof(sin));

    if (!listener)
    {
        printf("Could not create a listener\n");
        return 1;
    }

    event_base_dispatch(base);
    evconnlistener_free(listener);
    event_base_free(base);

    printf("done\n");
    return 0;
}

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                 struct sockaddr *sa, int socklen, void *user_data)
{
    printf("Detect an connection\n");
    struct event_base *base = (struct event_base *)user_data;
    struct bufferevent *bev;

    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
        printf("Could not create a bufferevent\n");
        event_base_loopbreak(base);
        return;
    }
    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
    //服务器监听到连接时，给客户端发送第一条消息
    const char *reply_msg = "I receive a message from server";
    bufferevent_write(bev, reply_msg, strlen(reply_msg));
}

//conn_writecwritecb函数将在bufferevent中的output evbuffer缓冲区发送完成后被调用。
//此时evbuffer_get_length(output) = 0，说明output evbuffer缓冲区被清空。
//假设发现有10000条记录要发送出去，1次发送10000条将占用大量内存，所以，我们要分批发送
//先发送100条数据，假设每条数据为1024字节bufferevent_write(bev,buf,1024 *100);
//系统在这100条记录发送完成后将调用conn_writecbb回调函数,然后在该函数中循环发送剩下的
//数据
void conn_writecb(struct bufferevent *bev, void *user_data)
{
//    struct evbuffer *output = bufferevent_get_output(bev);
//    if (evbuffer_get_length(output) == 0)
//    {
//        printf("Output evbuffer is flushed\n");
//        bufferevent_free(bev);
//    }
    //delay 1 second
    delay(1000);
    static int msg_num = 1;
    char reply_msg[1000] = {'\0'};
    const char *str = "I receive a message from server ";
    memcpy(reply_msg,str,strlen(str));
    sprintf(reply_msg+strlen(str),"%d",msg_num);
    bufferevent_write(bev, reply_msg, strlen(reply_msg));
    msg_num++;
}

void conn_readcb(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *input =bufferevent_get_input(bev);
    size_t sz=evbuffer_get_length(input);
    if (sz > 0)
    {
        char msg[BUFFER_SIZE] = {'\0'};
        bufferevent_read(bev, msg, sz);
        printf("%s\n", msg);
    }
}

void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
    if (events & BEV_EVENT_EOF)
    {
        printf("Connection closed\n");
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Got an error on the connection: %s\n",strerror(errno));
    }

    bufferevent_free(bev);
}

void delay(int ms)
{
   clock_t start = clock();
   while(clock() - start < ms);
}
