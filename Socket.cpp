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

Socket::Socket(const char* server_addr, const char* cPort)
{
    connect(server_addr, cPort);
}

Socket::~Socket()
{
}


int Socket::connect(const char* server_addr, const char* cPort)
{
	addrinfo *result = this->addrInfo.ai_next;
	int nRet = ::getaddrinfo(server_addr, cPort, &this->addrInfo, &result);
	if (nRet != 0)
	{
		this->socketError(WSA_ERROR, __FUNCTION__);
		nRet = -1;
	}
	else
	{
		do
		{
			nRet = ::connect(mySocket, result->ai_addr, (int)result->ai_addrlen);
		} while (nRet < 0 && (result = result->ai_next) != nullptr);
		
		if (nRet < 0)
		{
			this->socketError(WSA_ERROR, __FUNCTION__);
		}
		else if (result != nullptr)
		{
			this->addrInfo.ai_addr = result->ai_addr;
			this->addrInfo.ai_addrlen = result->ai_addrlen;
			this->addrInfo.ai_canonname = result->ai_canonname;
		}
	}

	::freeaddrinfo(this->addrInfo.ai_next);

    return nRet;
}