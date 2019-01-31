#include "SocketC++.h"
#include <string>

using namespace std;
using namespace cppsock;

//Socket///////////////////////////////////////////////////////

Socket::Socket()
{
}

Socket::Socket(const SOCKET &socket) : BaseSocket(socket)
{
}

Socket::Socket(const char* server_addr, unsigned short cPort)
{
    connect(server_addr, cPort);
}

Socket::~Socket()
{
}


int Socket::connect(const char* server_addr, unsigned short cPort)
{
    addrInfo.sin_family = this->af;
    addrInfo.sin_port = cPort;

    addrInfo.sin_addr.s_addr = ::inet_addr(server_addr);
    if (addrInfo.sin_addr.s_addr == INADDR_NONE)
    {
        LPHOSTENT lpHostEntry = ::gethostbyname(server_addr);
        if (lpHostEntry == NULL)
        {
            throw SocketException(string(server_addr) + " invalid parameter", TRACEBACK);
        }
        else
        {
            addrInfo.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
        }
    }

    int status = ::connect(mySocket, (LPSOCKADDR)&addrInfo, sizeof(struct sockaddr));
    if (status < 0)
    {
        this->socketError(WSA_ERROR, __FUNCTION__);
    }

    return status;
}