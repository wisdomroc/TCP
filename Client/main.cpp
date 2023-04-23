#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib,"WS2_32.lib")
int main()
{
    WSADATA wsd;
    SOCKET sockClient;                                            //客户端socket
    SOCKADDR_IN addrSrv;
    char recvBuf[100];
    if(WSAStartup(MAKEWORD(2,2),&wsd)!=0)
    {
        printf("start up failed!\n");
        return -1;
    }
    sockClient=socket(AF_INET,SOCK_STREAM,0);                    //创建socket
    addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
    addrSrv.sin_family=AF_INET;
    addrSrv.sin_port=htons(8888);
    connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));    //连接服务器端
//    send(sockClient,"hello world",strlen("hello world")+1,0);    //向服务器端发送数据
    recv(sockClient,recvBuf,100,0);                                //接收服务器端数据
    printf("%s\n",recvBuf);
    closesocket(sockClient);                                    //关闭连接
    WSACleanup();
    return 0;
}
