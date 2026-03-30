#ifndef I_NETWORK_HPP
#define I_NETWORK_HPP

#include "Core/Types.hpp"

// We must forward-declare the struct here or include its definition
struct NetworkMove;

class INetwork {
public:
    virtual ~INetwork() = default;
    virtual bool init() = 0;
    virtual bool hostGame(int port) = 0;
    virtual bool joinGame(const char* ipAddress, int port) = 0;
    virtual bool sendMove(int x, int y, char letter) = 0;
    virtual bool receiveMove(NetworkMove& outMove) = 0;
    virtual void cleanup() = 0;
};

#endif // I_NETWORK_HPP