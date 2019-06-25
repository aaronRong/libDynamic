#ifndef __LL_RELAYCON__
#define __LL_RELAYCON__

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
int open(const char* serverIp, unsigned short commPort);
int alarm(int id, unsigned short seconds);
int stop(int id);
int isalarm(int id);
int close(void);

#endif