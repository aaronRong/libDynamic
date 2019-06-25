#ifndef __LL_RELAYCON__
#define __LL_RELAYCON__

/* switch */
#define DLLPORT_EXPORTS

#ifdef DLLPORT_EXPORTS
#define _DLL_PORT _declspec( dllexport )
#else
#define _DLL_PORT _declspec( dllimport )
#endif

/* definition */
#define RET_ERR		(-1)
#define RET_PASS	(1)

/* enum definition */
typedef enum _e_LL_Relay_NO
{
	LL_RELAY_ONE = 0x00,
	LL_RELAY_TWO,
	LL_RELAY_THREE,
}e_LL_Relay_NO;

typedef enum _e_LL_Relay_State
{
	LL_RELAY_ON = 0x00,
	LL_RELAY_OFF,
}e_LL_Relay_State;

/* function declaration */
_DLL_PORT int open(const char* serverIp, unsigned short commPort);
_DLL_PORT int alarm(int id, unsigned short seconds);
_DLL_PORT int stop(int id);
_DLL_PORT int isalarm(int id);
_DLL_PORT int close(void);

#endif