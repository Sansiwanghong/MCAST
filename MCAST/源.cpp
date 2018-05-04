//在程序运行之前，需使用NTP协议完成局域网内部的对时操作
//目的：统一集群内各机器的时间轴线，方便机器的同步协调操
//     作.
//方法：查看系统是否安装有NTP对时程序，对NTP server与
//     NTP client 进行相应的配置即可。
#include <WinSock2.h>  
#include <iostream>  
#include<string>

using namespace std;
#pragma  comment (lib,"wsock32.lib")  

#define MCAST_PORT 8888  
#define MCAST_ADDR "224.0.0.88"  // 局部组播地址：在224.0.0.0~224.0.0.255之间
//这个区间的地址视为路由协议和其他用途保留的地址，路由器并不妆发范围的IP包。 
//#define MCAST_DATA "BROADCAST TEST DATA"  // 多播内容  
#define MCAST_INTERVAL 1000  //多播命令时间间隔，可视情况作相应灵敏度调整  
using namespace std;

struct Command
{
	uint16_t head;
	uint16_t type;
	uint16_t order;
	SYSTEMTIME time;
	uint16_t tail;

};



void main()
{
	
	
	//初始化命令数据包
	Command Cmd;
	Cmd.head = 0x01;
	Cmd.type = 0x01;
	Cmd.order = 0x01;
	 GetLocalTime(&Cmd.time);
	Cmd.tail = 0xf0;

    //创建设置组播
	SOCKET sock;
	struct sockaddr_in mcast_addr;
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		cout << err << endl;
		cout << "socket版本初始化失败" << endl;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)

	{
		WSACleanup();
		return;
	}
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "socket创建失败" << endl;
		cout << WSAGetLastError() << endl;
		return;
	}
	memset(&mcast_addr, 0, sizeof(mcast_addr));
	bool opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char FAR *>(&opt), sizeof(opt));
	mcast_addr.sin_family = AF_INET;
	mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);  //inet_npton  
	mcast_addr.sin_port = htons(MCAST_PORT);
	 char *MCAST_DATA = "hello mimi";
	
	 int  commandNum = -1;
	while (1)

	{ 
		cout << "Please input command number you want:" << endl;
		cout << "【1】.Start " << endl;
		cout << "【2】.Stop  " << endl;
		cout << "【3】.Exit  " << endl;

		cin >> commandNum;
		if (commandNum == 1)
		{
			Cmd.order = 1;

		}
		else if(commandNum==2)
		{
			Cmd.order = 2;
		}
		else if (commandNum == 3)
		{
			Cmd.order = 0x03;
		}
		else 
		{
			cout << "Error input" << endl;
			continue;
		}

		
		//MCAST_DATA = (char*)&Cmd;

		/*struct Command
		{
			uint16_t head;
			uint16_t type;
			uint16_t order;
			SYSTEMTIME time;
			uint16_t tail;

		};*/
		char buf[1000];
		char *p = buf;
		*((uint16_t*)p) = Cmd.head;
		p += sizeof(uint16_t);
		*((uint16_t*)p) = Cmd.type;
		p += sizeof(uint16_t);
		*((uint16_t*)p) = Cmd.order;
		p += sizeof(uint16_t);
		//获取尽可能接近的命令发送时间信息
		  GetLocalTime(&Cmd.time);
		*((SYSTEMTIME*)p) = Cmd.time;
		p += sizeof(SYSTEMTIME);
		*((uint16_t*)p) = Cmd.tail;

	
	
		//向局部多播地址发送多播内容  
		//int n = sendto(sock, MCAST_DATA, strlen(MCAST_DATA), 0, (struct sockaddr*)&mcast_addr, sizeof(mcast_addr));
		 int n = sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&mcast_addr, sizeof(mcast_addr));
		
		if (n<0)
		{
			cout << "send error" << endl;
			return;
		}
		else
		{
			cout << "send message is successful ...." << endl;
		}
		
		Sleep(MCAST_INTERVAL);

		//DWORD dwStart = GetTickCount();

		//DWORD dwEnd = dwStart;

		//do

		//{

		//	dwEnd = GetTickCount() - dwStart;

		//} while (dwEnd <1000);
	}
	if (!closesocket(sock)) //关闭套接字  
	{
		WSAGetLastError();
		return;
	}
	if (!WSACleanup())       //关闭Socket库  
	{
		WSAGetLastError();
		return;
	}
}