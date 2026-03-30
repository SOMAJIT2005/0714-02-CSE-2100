#include "Systems/Network.hpp"
#include <iostream>

static TCPsocket s_socket = nullptr;
static SDLNet_SocketSet s_socketSet = nullptr;
static bool s_isConnected = false;

bool Network::init() {
    if (SDLNet_Init() == -1) {
        std::cerr << "Network Error: " << SDLNet_GetError() << "\n";
        return false;
    }
    s_socketSet = SDLNet_AllocSocketSet(1);
    return true;
}

bool Network::hostGame(int port) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, nullptr, port) == -1) return false;
    TCPsocket server = SDLNet_TCP_Open(&ip);
    if (!server) return false;
    std::cout << "Hosting on port " << port << ". Waiting for Player 2...\n";
    
    // Blocking wait for connection
    while (!s_socket) {
        s_socket = SDLNet_TCP_Accept(server);
        SDL_Delay(100);
    }
    
    SDLNet_TCP_AddSocket(s_socketSet, s_socket);
    s_isConnected = true;
    std::cout << "Player 2 Connected!\n";
    return true;
}

bool Network::joinGame(const char* ipAddress, int port) {
    IPaddress ip;
    std::cout << "Connecting to " << ipAddress << ":" << port << "...\n";
    if (SDLNet_ResolveHost(&ip, ipAddress, port) == -1) return false;
    
    s_socket = SDLNet_TCP_Open(&ip);
    if (!s_socket) return false;
    
    SDLNet_TCP_AddSocket(s_socketSet, s_socket);
    s_isConnected = true;
    std::cout << "Connected to Host successfully!\n";
    return true;
}

bool Network::sendMove(int x, int y, char letter) {
    if (!s_isConnected || !s_socket) return false;
    NetworkMove packet{ x, y, letter };
    int result = SDLNet_TCP_Send(s_socket, &packet, sizeof(NetworkMove));
    return (result >= static_cast<int>(sizeof(NetworkMove)));
}

bool Network::receiveMove(NetworkMove& incomingMove) {
    if (!s_isConnected || !s_socket) return false;
    if (SDLNet_CheckSockets(s_socketSet, 0) > 0) {
        if (SDLNet_SocketReady(s_socket)) {
            int result = SDLNet_TCP_Recv(s_socket, &incomingMove, sizeof(NetworkMove));
            if (result > 0) return true;
        }
    }
    return false;
}

void Network::cleanup() {
    if (s_socket) SDLNet_TCP_Close(s_socket);
    if (s_socketSet) SDLNet_FreeSocketSet(s_socketSet);
    SDLNet_Quit();
    s_isConnected = false;
}