#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "Systems/Network.hpp"
#include <SDL2/SDL_net.h>

struct NetworkMove {
    int x;
    int y;
    char letter;
};

class Network  {
public:
    bool init();
    bool hostGame(int port);
    bool joinGame(const char* ipAddress, int port);
    bool sendMove(int x, int y, char letter);
    bool receiveMove(NetworkMove& outMove);
    void cleanup();
private:
    TCPsocket m_socket = nullptr;
    SDLNet_SocketSet m_socketSet = nullptr;
    bool m_isConnected = false;
};

#endif // NETWORK_HPP