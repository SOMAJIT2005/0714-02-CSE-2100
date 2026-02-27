#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>

// The exact packet of data we will send over Wi-Fi
typedef struct {
    int x;
    int y;
    char letter;
} NetworkMove;

bool Network_Init(void);
bool Network_HostGame(int port);
bool Network_JoinGame(const char* ipAddress, int port);
bool Network_SendMove(int x, int y, char letter);
bool Network_ReceiveMove(NetworkMove* incomingMove);
void Network_Cleanup(void);

#endif // NETWORK_H