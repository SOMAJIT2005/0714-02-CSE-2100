#include "../include/network.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#endif

static TCPsocket s_serverSocket = NULL; 
static TCPsocket s_socket = NULL;       
static SDLNet_SocketSet s_socketSet = NULL;
static bool s_isConnected = false;

static UDPsocket s_udpSocket = NULL;
static UDPpacket *s_udpPacket = NULL;
static Uint32 s_lastBroadcastTime = 0;

static char s_netBuffer[1024] = {0};
static int s_bufferLen = 0;

bool Network_Init(void) {
    if (SDLNet_Init() == -1) return false;
    s_socketSet = SDLNet_AllocSocketSet(1);
    return true;
}

bool Network_HostGame(int port) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, port) == -1) return false;
    s_serverSocket = SDLNet_TCP_Open(&ip);
    if (!s_serverSocket) return false;
    return true; 
}

bool Network_JoinGame(const char* ipAddress, int port) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, ipAddress, port) == -1) return false;
    s_socket = SDLNet_TCP_Open(&ip);
    if (!s_socket) return false;
    SDLNet_TCP_AddSocket(s_socketSet, s_socket);
    s_isConnected = true;
    return true;
}

bool Network_SendMessage(const char* msg) {
    if (!s_isConnected || !s_socket) return false;
    int len = strlen(msg);
    int result = SDLNet_TCP_Send(s_socket, msg, len);
    return (result == len);
}

int Network_ReceiveMessage(char* buffer, int maxLen) {
    if (s_serverSocket && !s_isConnected) {
        s_socket = SDLNet_TCP_Accept(s_serverSocket);
        if (s_socket) { SDLNet_TCP_AddSocket(s_socketSet, s_socket); s_isConnected = true; }
        return 0; 
    }
    if (!s_isConnected || !s_socket) return 0;
    if (SDLNet_CheckSockets(s_socketSet, 0) > 0) {
        if (SDLNet_SocketReady(s_socket)) {
            char temp[256];
            int result = SDLNet_TCP_Recv(s_socket, temp, sizeof(temp) - 1);
            if (result > 0) {
                temp[result] = '\0';
                if (s_bufferLen + result < sizeof(s_netBuffer)) { strcat(s_netBuffer, temp); s_bufferLen += result; }
            }
        }
    }
    char* newline = strchr(s_netBuffer, '\n');
    if (newline) {
        *newline = '\0'; 
        strncpy(buffer, s_netBuffer, maxLen - 1);
        buffer[maxLen - 1] = '\0';
        int msgLen = (newline - s_netBuffer) + 1;
        int remaining = s_bufferLen - msgLen;
        if (remaining > 0) { memmove(s_netBuffer, newline + 1, remaining); s_netBuffer[remaining] = '\0'; s_bufferLen = remaining; } 
        else { s_netBuffer[0] = '\0'; s_bufferLen = 0; }
        return strlen(buffer);
    }
    return 0;
}

void Network_Cleanup(void) {
    if (s_socket) SDLNet_TCP_Close(s_socket);
    if (s_serverSocket) SDLNet_TCP_Close(s_serverSocket);
    if (s_socketSet) SDLNet_FreeSocketSet(s_socketSet);
    if (s_udpSocket) SDLNet_UDP_Close(s_udpSocket);
    if (s_udpPacket) SDLNet_FreePacket(s_udpPacket);
    SDLNet_Quit();
}

const char* Network_GetLocalIP() {
    static char ipBuffer[64] = "127.0.0.1"; 
#ifdef _WIN32
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        struct hostent *host = gethostbyname(hostname);
        if (host != NULL && host->h_addr_list[0] != NULL) {
            struct in_addr addr; addr.s_addr = *(u_long*)host->h_addr_list[0];
            strncpy(ipBuffer, inet_ntoa(addr), sizeof(ipBuffer) - 1);
        }
    }
#endif
    return ipBuffer;
}

void Network_BroadcastPresence(const char* hostName) {
    if (!s_udpSocket) { s_udpSocket = SDLNet_UDP_Open(0); s_udpPacket = SDLNet_AllocPacket(512); }
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - s_lastBroadcastTime > 1000) { 
        IPaddress ip; SDLNet_ResolveHost(&ip, "255.255.255.255", 8888); 
        s_udpPacket->address.host = ip.host; s_udpPacket->address.port = ip.port;
        sprintf((char*)s_udpPacket->data, "WORDS_COLLIDE,%s,%s", Network_GetLocalIP(), hostName);
        s_udpPacket->len = strlen((char*)s_udpPacket->data);
        SDLNet_UDP_Send(s_udpSocket, -1, s_udpPacket); s_lastBroadcastTime = currentTime;
    }
}

bool Network_ListenForHost(char* foundIP) {
    if (!s_udpSocket) { s_udpSocket = SDLNet_UDP_Open(8888); s_udpPacket = SDLNet_AllocPacket(512); }
    if (SDLNet_UDP_Recv(s_udpSocket, s_udpPacket)) {
        char* data = (char*)s_udpPacket->data; data[s_udpPacket->len] = '\0';
        if (strncmp(data, "WORDS_COLLIDE", 13) == 0) {
            char header[32], ip[64], name[64];
            sscanf(data, "%[^,],%[^,],%[^\n]", header, ip, name); strcpy(foundIP, ip); return true;
        }
    }
    return false;
}