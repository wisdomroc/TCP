//#include<iostream>
//#include<WinSock2.h>
//using namespace std;
//#pragma comment(lib,"ws2_32.lib")

//int main()
//{
//    //初始化WSA
//    WORD sockVersion=MAKEWORD(2,2);
//    WSADATA wsaData;//WSADATA结构体变量的地址值

//    //int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
//    //成功时会返回0，失败时返回非零的错误代码值
//    if(WSAStartup(sockVersion,&wsaData)!=0)
//    {
//        cout<<"WSAStartup() error!"<<endl;
//        return 0;
//    }

//    //创建套接字
//    SOCKET slisten=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//    if(slisten==INVALID_SOCKET)
//    {
//        cout<<"socket error !"<<endl;
//        return 0;
//    }

//    //绑定IP和端口
//    sockaddr_in sin;//ipv4的指定方法是使用struct sockaddr_in类型的变量
//    sin.sin_family = AF_INET;
//    sin.sin_port = htons(8888);//设置端口。htons将主机的unsigned short int转换为网络字节顺序
//    sin.sin_addr.S_un.S_addr = INADDR_ANY;//IP地址设置成INADDR_ANY，让系统自动获取本机的IP地址
//    //bind函数把一个地址族中的特定地址赋给scket。
//    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
//    {
//        printf("bind error !");
//    }

//    //开始监听
//    if(listen(slisten,5)==SOCKET_ERROR)
//    {
//        cout<<"listen error !"<<endl;
//        return -1;
//    }

//    //循环接收数据
//    SOCKET sclient;
//    sockaddr_in remoteAddr;//sockaddr_in常用于socket定义和赋值,sockaddr用于函数参数
//    int nAddrlen=sizeof(remoteAddr);
//    char revData[255];
//    while(true)
//    {
//        cout<<"wait to connect..."<<endl;
//        sclient=accept(slisten,(sockaddr *)&remoteAddr,&nAddrlen);
//        if(sclient==INVALID_SOCKET)
//        {
//            cout<<"accept error !"<<endl;
//            continue;
//        }
//        cout<<"accepted a connection..."<<inet_ntoa(remoteAddr.sin_addr)<<endl;
//        //接收数据
////        int ret=recv(sclient,revData,255,0);
////        if(ret>0)
////        {
////            revData[ret]=0x00;
////            cout<<revData<<endl;
////        }
//        //发送数据
//        const char * sendData = "你好，TCP客户端！\n";
//        send(sclient, sendData, strlen(sendData), 0);
//        closesocket(sclient);
//    }
//    closesocket(slisten);
//    WSACleanup();
//    system("pause");
//    //return 0;
//}
