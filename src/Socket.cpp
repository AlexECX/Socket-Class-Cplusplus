#include "SocketC++.h"
#include <string>

using namespace std;
using namespace cppsock;

//Socket///////////////////////////////////////////////////////

//Socket::Socket() : BaseSocket()
//{
//}

Socket::Socket(int af, int type, int protocol) : BaseSocket(af, type, protocol)
{
}

Socket::Socket(const Socket & other) : BaseSocket(other)
{
}

Socket::Socket(Socket && other) : BaseSocket(std::move(other))
{
}

Socket::Socket(const SOCKET &socket, SOCKET_ADDRESS addr, int type, int protocol)
	: BaseSocket(socket, addr, type, protocol)
{
}

Socket::Socket(const char* server_addr, const char* cPort) : Socket()
{
    connect(server_addr, cPort);
}

Socket::Socket(const sockaddr * name, int namelen) : Socket()
{
	connect(name, namelen);
}

Socket::~Socket()
{
}


int Socket::connect(const char* server_addr, const char* cPort)
{
	addrinfo addrInfo = { 0 };
	addrInfo.ai_flags = this->ai_flags;
	addrInfo.ai_family = this->ai_family;
	addrInfo.ai_socktype = this->ai_socktype;
	addrInfo.ai_protocol = this->ai_protocol;
	addrinfo *result = nullptr;

	int nRet = ::getaddrinfo(server_addr, cPort, &addrInfo, &result);
	if (nRet != 0)
	{
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
		nRet = -1;
	}
	else
	{
		do
		{
			nRet = connect(result->ai_addr, (int)result->ai_addrlen);
		} while (nRet < 0 && (result = result->ai_next) != nullptr);
		
		if (nRet < 0)
		{
			this->socketError(/*WSA_ERROR, __FUNCTION__*/);
		}
	}

	::freeaddrinfo(result);

    return nRet;
}

int Socket::connect(const sockaddr * name, int namelen)
{
	int nRet = ::connect(mySocket, name, namelen);
	if (nRet < 0)
	{
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
	else
	{
		this->addrsock.lpSockaddr = (LPSOCKADDR)name;
		this->addrsock.iSockaddrLength = namelen;
	}
	return nRet;
}
