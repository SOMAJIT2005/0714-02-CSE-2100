#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>
#include <SDL2/SDL_net.h>

bool Network_Init(void);
bool Network_HostGame(int port);
bool Network_JoinGame(const char* ipAddress, int port);
bool Network_SendMessage(const char* msg);
int Network_ReceiveMessage(char* buffer, int maxLen);
void Network_Cleanup(void);

const char* Network_GetLocalIP();
void Network_BroadcastPresence(const char* hostName);
bool Network_ListenForHost(char* foundIP);

#endif