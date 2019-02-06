#include "SocketC++.h"
#include <string>

using namespace std;
using namespace cppsock;

//Socket///////////////////////////////////////////////////////

Socket::Socket()
{
}

Socket::Socket(const SOCKET &socket, const addrinfo &addrInfo, const sockaddr &addr)
{
	//mySocket_ptr = make_shared<SocketWrap>(socket);
	mySocket = socket;
	this->addrInfo = addrInfo;
	this->addrSock = addr;
	this->addrInfo.ai_addr = &this->addrSock;
	if (mySocket == INVALID_SOCKET || mySocket == SOCKET_ERROR)
	{
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
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
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
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
			this->socketError(/*WSA_ERROR, __FUNCTION__*/);
		}
		else if (result != nullptr)
		{
			this->addrSock = *result->ai_addr;
			this->addrInfo.ai_addrlen = result->ai_addrlen;
			this->addrInfo.ai_addr = &this->addrSock;
			//this->addrInfo.ai_canonname = result->ai_canonname;
		}
	}

	::freeaddrinfo(this->addrInfo.ai_next);

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
		this->addrSock = *name;
		this->addrInfo.ai_addrlen = namelen;
		this->addrInfo.ai_addr = &this->addrSock;
		//this->addrInfo.ai_canonname = result->ai_canonname;
	}
	return nRet;
}
