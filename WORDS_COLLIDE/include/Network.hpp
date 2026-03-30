#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "Types.hpp"
#include <SDL2/SDL_net.h>

struct NetworkMove {
    int x;
    int y;
    char letter;
};

class Network {
public:
    static bool init();
    static bool hostGame(int port);
    static bool joinGame(const char* ipAddress, int port);
    static bool sendMove(int x, int y, char letter);
    static bool receiveMove(NetworkMove& outMove);
    static void cleanup();
};

#endif // NETWORK_HPP
