#include "NetBattle.h"
#include "cocos2d.h"
#ifdef WIN32
#else
#define INVALID_SOCKET -1
#define closesocket close
#endif

SOCKET NetBattle::_server = INVALID_SOCKET;
SOCKET NetBattle::_connet = INVALID_SOCKET;
int NetBattle::_isConnected = 0;
int NetBattle::_isRecvComplete = 0;
char* NetBattle::_recvData = NULL;

bool NetBattle::Listen(short port)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cocos2d::log("error create socket");
		return false;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
#ifdef WIN32
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	addr.sin_addr.s_addr = INADDR_ANY;
#endif

#ifdef WIN32

#else
	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
#endif
	int ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	if (ret != 0)
	{
		cocos2d::log("bind error");
		closesocket(sock);
		return false;
	}

	listen(sock, 10);

	// 阻塞
	// SOCKET conn = accept(sock, NULL, NULL);

	_server = sock;
	_isConnected = 0;
#ifdef WIN32
	HANDLE hThread = CreateThread(NULL, 0, AcceptThreadFunc, NULL, 0, NULL);
	CloseHandle(hThread);
#else
	pthread_t tid;
	pthread_create(&tid, NULL, AcceptThreadFunc, NULL);
#endif
	return true;
}

#ifdef WIN32
DWORD NetBattle::AcceptThreadFunc(void*)
#else
void* NetBattle::AcceptThreadFunc(void *)
#endif
{
	_connet = accept(_server, NULL, NULL);
	_isConnected = 1;
#ifdef WIN32
	return 0;
#else
	return NULL;
#endif
}

bool NetBattle::isConnected()
{
	return (bool)_isConnected;
}

bool NetBattle::Connect(const char* ip, short port)
{
	_connet = socket(AF_INET, SOCK_STREAM, 0);
	if (_connet == INVALID_SOCKET)
		return false;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
#ifdef WIN32
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	addr.sin_addr.s_addr = inet_addr(ip);
#endif
	int ret = connect(_connet, (struct sockaddr*)&addr, sizeof(addr));
	if (ret != 0)
	{
		closesocket(_connet);
		return false;
	}

	return true;
}

bool NetBattle::RecvStart()
{
	// 阻塞
	// recv()
	_isRecvComplete = 0;
#ifdef WIN32
	HANDLE hThread = CreateThread(NULL, 0, RecvThreadFunc, NULL, 0, NULL);
	CloseHandle(hThread);
#else
	pthread_t tid;
	pthread_create(&tid, NULL, RecvThreadFunc, NULL);
#endif
	return true;
}

#ifdef WIN32
DWORD NetBattle::RecvThreadFunc(void*)
#else
void* NetBattle::RecvThreadFunc(void*)
#endif
{
	int ret;
	static char buf[16];
	ret = recv(_connet, buf, 1, 0);
	if (ret <= 0)
		return NULL;

	// 选中
	if (buf[0] == 1)
	{
		ret = recv(_connet, &buf[1], 1, 0);
		if (ret < 0) return NULL;
	}
	// 走棋
	else if (buf[0] == 2)
	{
		for (int i = 1; i <= 3; ++i)
		{
			ret = recv(_connet, &buf[i], 1, 0);
			if (ret < 0) return NULL;
		}
	}

	// recv结束了
	_recvData = buf;
	_isRecvComplete = 1;
#ifdef WIN32
	return 0;
#else
	return NULL;
#endif
}

bool NetBattle::isRecvComplete()
{
	return (bool)_isRecvComplete;
}

char* NetBattle::RecvData(int& len)
{
	len = 0; // len目前没有用
	_isRecvComplete = 0; // 报文被取走，接收状态回到初始状态
	return _recvData;
}

int NetBattle::Send(const char* buffer, int len)
{
	return send(_connet, buffer, len, 0);
}

void NetBattle::clear()
{
	closesocket(_connet);
	if (_server != INVALID_SOCKET)
		closesocket(_server);
}
