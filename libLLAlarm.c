#include <stdio.h>
#include <winsock2.h>
#include <assert.h>
#include "libLLAlarm.h"

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

/* definition */
#define MAX_RECV_LEN			(30UL)
#define MIN_ID_INDEX			(0UL)
#define MAX_ID_INDEX			(2UL)
#define RELAY_ONE_TIMER_ID		(0UL)
#define RELAY_TWO_TIMER_ID		(0UL)
#define RELAY_THREE_TIMER_ID	(0UL)

/* global varaible */
static SOCKET socketClient;
static struct sockaddr_in serverIn;
static unsigned int relayState[4] = { LL_RELAY_OFF , LL_RELAY_OFF , LL_RELAY_OFF };

const char relayTcp[6][20] = {
	"relay one on",
	"relay one off",
	"relay two on",
	"relay two off",
	"relay three on",
	"relay three off",
};

const char relayTcpBack[6][20] = {
	"relay one on done",
	"relay one off done",
	"relay two on done",
	"relay two off done",
	"relay three on done",
	"relay three off done",
};

/* function declaration */
int setRelayStatus(int id, int status);
void CALLBACK timerCallbackOne(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);
void CALLBACK timerCallbackTwo(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);
void CALLBACK timerCallbackThree(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);

/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
int open(const char* serverIp, unsigned short commPort)
{
	WORD socket_version;
	WSADATA wsadata;
	socket_version = MAKEWORD(2, 2);			/* call different winsock version */
	if (WSAStartup(socket_version, &wsadata) != 0)
	{
		printf("WSAStartup error!");
		system("pause");
		return RET_ERR;
	}

	socketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		/* ipv4, reliable transmission */
	if (socketClient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		system("pause");
		return RET_ERR;
	}

	serverIn.sin_family = AF_INET;				/* ipv4 */
	serverIn.sin_port = htons(commPort);		/* port from little-endian to big-endian */
	serverIn.sin_addr.S_un.S_addr = inet_addr(serverIp);	/* convert ip */
	if (connect(socketClient, (struct sockaddr*) & serverIn, sizeof(serverIn)) == SOCKET_ERROR)		/* build socket transmission */
	{
		printf("connect error\n");
		system("pause");
		return RET_ERR;
	}

	return RET_PASS;
}

/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
int alarm(int id, unsigned short seconds)
{
	int status = RET_ERR;

	assert((id >= MIN_ID_INDEX) && (id <= MAX_ID_INDEX));

	status = setRelayStatus(id, LL_RELAY_ON);
	if (RET_ERR == status)
	{
		return RET_ERR;
	}

	/* start timer */
	switch (id)
	{
	case LL_RELAY_ONE:
		SetTimer(NULL, RELAY_ONE_TIMER_ID, seconds * 1000, timerCallbackOne);
		break;
	case LL_RELAY_TWO:
		SetTimer(NULL, RELAY_TWO_TIMER_ID, seconds * 1000, timerCallbackTwo);
		break;
	case LL_RELAY_THREE:
		SetTimer(NULL, RELAY_THREE_TIMER_ID, seconds * 1000, timerCallbackThree);
		break;
	}

	/* set state */
	relayState[id] = LL_RELAY_ON;

	return RET_PASS;
}

/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
int isalarm(int id)
{
	if (LL_RELAY_ON == relayState[id])
	{
		return RET_PASS;
	}

	return RET_ERR;
}

/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
int stop(int id)
{
	int status = RET_ERR;

	status = setRelayStatus(id, LL_RELAY_OFF);
	if (RET_PASS == status)
	{
		relayState[id] = LL_RELAY_OFF;
	}

	return status;
}

/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
int close(void)
{
	closesocket(socketClient);
	WSACleanup();

	return RET_PASS;
}

/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
void CALLBACK timerCallbackOne(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	int status = RET_ERR;

	KillTimer(NULL, nTimerid);

	status = setRelayStatus(LL_RELAY_ONE, LL_RELAY_OFF);

	relayState[LL_RELAY_ONE] = LL_RELAY_OFF;
}
/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
void CALLBACK timerCallbackTwo(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	int status = RET_ERR;

	KillTimer(NULL, nTimerid);

	status = setRelayStatus(LL_RELAY_ONE, LL_RELAY_OFF);

	relayState[LL_RELAY_TWO] = LL_RELAY_OFF;
}
/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
void CALLBACK timerCallbackThree(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	int status = RET_ERR;

	KillTimer(NULL, nTimerid);

	status = setRelayStatus(LL_RELAY_ONE, LL_RELAY_OFF);

	relayState[LL_RELAY_THREE] = LL_RELAY_OFF;
}

/*
* Author Name: aaron.gao
* Time: 2019.6.25
*/
int setRelayStatus(int id, int status)
{
	unsigned int len = 0;
	unsigned int ret = 0;
	unsigned char recvBuf[30];

	assert((id >= MIN_ID_INDEX) && (id <= MAX_ID_INDEX));
	assert((status == LL_RELAY_ON) || (status == LL_RELAY_OFF));

	len = send(socketClient, relayTcp[id * 2 + status], strlen(relayTcp[id * 2 + status]), 0);
	if (SOCKET_ERROR == len)
	{
		return RET_ERR;
	}

	/* recv */
	len = recv(socketClient, recvBuf, MAX_RECV_LEN, 0);
	if (len <= 0)
	{
		return RET_ERR;
	}
	ret = strncmp(relayTcpBack[id * 2 + status], recvBuf, len);
	if (ret != 0)
	{
		return RET_ERR;
	}

	return RET_PASS;
}
