#include "SocketC++.h"
#include <string>


using namespace std;
using namespace cppsock;

//ServerSocket///////////////////////////////////////////////////////


//ServerSocket::ServerSocket() : BaseSocket()
//{
//}

ServerSocket::ServerSocket(int af, int type, int protocol) : BaseSocket(af, type, protocol)
{
}

ServerSocket::ServerSocket(const SOCKET & socket, SOCKET_ADDRESS addr, int type, int protocol)
	: BaseSocket(socket, addr, type, protocol)
{
}

ServerSocket::ServerSocket(const ServerSocket & other) : BaseSocket(other)
{
}

ServerSocket::ServerSocket(ServerSocket && other) : BaseSocket(std::move(other))
{
}

ServerSocket::ServerSocket(const char* cPort) : ServerSocket()
{
	bind("", cPort);
}

ServerSocket::ServerSocket(const char* server_addr, const char* cPort) : ServerSocket()
{
	bind(server_addr, cPort);
}


ServerSocket::~ServerSocket()
{
}

int ServerSocket::bind(const char* server_addr, const char* cPort, unsigned queue_size)
{
	addrinfo addrInfo = { 0 };
	addrInfo.ai_flags =		this->ai_flags;
	addrInfo.ai_family =	this->ai_family;
	addrInfo.ai_socktype =	this->ai_socktype;
	addrInfo.ai_protocol =	this->ai_protocol;
	addrinfo *result = nullptr;

	int nRet;
	if (server_addr != "") {
		nRet = ::getaddrinfo(server_addr, cPort, &addrInfo, &result);
	}
	else {
		this->ai_flags = addrInfo.ai_flags = AI_PASSIVE;
		nRet = ::getaddrinfo(NULL, cPort, &addrInfo, &result);
	}

	if (nRet != 0) {
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
		nRet = -1;
	}
	else if (0 > (nRet = bind(result->ai_addr, (int)result->ai_addrlen))) {
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
	
	::freeaddrinfo(result);

	return nRet;
}

int ServerSocket::bind(const sockaddr * name, int namelen, unsigned queue_size)
{
	int nRet;
	if (0 > (nRet = ::bind(mySocket, name, namelen))) {
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
	else if (0 > (nRet = ::listen(mySocket, queue_size))) {
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
	else
	{
		this->addrsock.lpSockaddr = (LPSOCKADDR)name;
		this->addrsock.iSockaddrLength = namelen;
		//this->addrInfo.ai_canonname = result->ai_canonname;
	}

	return nRet;
}

Socket ServerSocket::accept()
{
	SOCKET_ADDRESS addr;
	sockaddr tmp_addr = { 0 };
	addr.iSockaddrLength = this->addrsock.iSockaddrLength;
	SOCKET s = ::accept(mySocket, &tmp_addr, &addr.iSockaddrLength);
	addr.lpSockaddr = (LPSOCKADDR)&tmp_addr;
	return Socket(s, addr, this->ai_socktype, this->ai_protocol);
}

