


#ifndef _WEBSOCKET_H
#define _WEBSOCKET_H



#include "Handshake.h"
#include "Communicate.h"
#include "Errors.h"


#define PORT 4567

ws_list *l;
int port;
pthread_t pthread_id;
pthread_attr_t pthread_attr;


extern void receberMenssagem(char **message, ws_client *n);
extern void* init_websocket(void *args);
extern void* sendMsgBroadcast(void *msg);
extern void sendMsgUnicast(char  *msg, ws_client *l);
#endif
