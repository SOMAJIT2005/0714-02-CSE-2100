#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "Systems/INetwork.hpp"
#include <SDL2/SDL_net.h>

class Network : public INetwork {
public:
    bool init() override;
    bool hostGame(int port) override;
    bool joinGame(const char* ipAddress, int port) override;
    bool sendMove(int x, int y, char letter) override;
    bool receiveMove(NetworkMove& outMove) override;
    void cleanup() override;
private:
    TCPsocket m_socket = nullptr;
    SDLNet_SocketSet m_socketSet = nullptr;
    bool m_isConnected = false;
};

#endif // NETWORK_HPP