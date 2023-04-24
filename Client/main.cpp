#include <stdio.h>
#include <time.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

const int PORT = 2500;
const int BUFFER_SIZE = 1024;

void conn_writecb(struct bufferevent *, void *);
void conn_readcb(struct bufferevent *, void *);
void conn_eventcb(struct bufferevent *, short, void *);
void delay(int ms);

int main()
{
    printf("I am client\n");
#ifdef WIN32
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);

    struct event_base *base = event_base_new();
    if (!base)
    {
        printf("Could not initialize libevent\n");
        return 1;
    }

    struct bufferevent* bev = bufferevent_socket_new(base, -1,
                             BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
    int flag=bufferevent_socket_connect(bev, (struct sockaddr *)&srv,sizeof(srv));
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    if(-1==flag)
    {
        printf("Connect failed\n");
        return 1;
    }

    event_base_dispatch(base);
    event_base_free(base);

    printf("done\n");
    return 0;
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
    const char *str = "I receive a message from client ";
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
        fflush(stdout);
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
    else if( events & BEV_EVENT_CONNECTED)
    {
        printf("Connect succeed\n");
        //客户端链接成功后，给服务器发送第一条消息
        const char *reply_msg = "I receive a message from client";
        bufferevent_write(bev, reply_msg, strlen(reply_msg));
        return ;
    }

    bufferevent_free(bev);
}

void delay(int ms)
{
   clock_t start = clock();
   while(clock() - start < ms);
}
