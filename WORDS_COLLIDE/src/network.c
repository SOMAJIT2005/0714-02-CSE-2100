#include "../include/network.h"
#include <stdio.h>

static TCPsocket s_socket = NULL;
static SDLNet_SocketSet s_socketSet = NULL;
static bool s_isConnected = false;

bool Network_Init(void) {
    if (SDLNet_Init() == -1) {
        printf("Network Error: %s\n", SDLNet_GetError());
        return false;
    }
    s_socketSet = SDLNet_AllocSocketSet(1);
    return true;
}

bool Network_HostGame(int port) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, port) == -1) return false;
    
    TCPsocket server = SDLNet_TCP_Open(&ip);
    if (!server) return false;

    printf("Hosting on port %d. Waiting for Player 2...\n", port);
    
    // Halt the game and wait for someone to connect
    while (!s_socket) {
        s_socket = SDLNet_TCP_Accept(server);
        SDL_Delay(100); 
    }
    
    SDLNet_TCP_AddSocket(s_socketSet, s_socket);
    s_isConnected = true;
    printf("Player 2 Connected!\n");
    return true;
}

bool Network_JoinGame(const char* ipAddress, int port) {
    IPaddress ip;
    printf("Connecting to %s:%d...\n", ipAddress, port);
    if (SDLNet_ResolveHost(&ip, ipAddress, port) == -1) return false;

    s_socket = SDLNet_TCP_Open(&ip);
    if (!s_socket) return false;

    SDLNet_TCP_AddSocket(s_socketSet, s_socket);
    s_isConnected = true;
    printf("Connected to Host successfully!\n");
    return true;
}

bool Network_SendMove(int x, int y, char letter) {
    if (!s_isConnected || !s_socket) return false;
    
    NetworkMove packet = { x, y, letter };
    int result = SDLNet_TCP_Send(s_socket, &packet, sizeof(NetworkMove));
    return (result >= sizeof(NetworkMove));
}

bool Network_ReceiveMove(NetworkMove* incomingMove) {
    if (!s_isConnected || !s_socket) return false;

    // Check if data is waiting without freezing the game (Non-blocking)
    if (SDLNet_CheckSockets(s_socketSet, 0) > 0) {
        if (SDLNet_SocketReady(s_socket)) {
            int result = SDLNet_TCP_Recv(s_socket, incomingMove, sizeof(NetworkMove));
            if (result > 0) return true; 
        }
    }
    return false;
}

void Network_Cleanup(void) {
    if (s_socket) SDLNet_TCP_Close(s_socket);
    if (s_socketSet) SDLNet_FreeSocketSet(s_socketSet);
    SDLNet_Quit();
}