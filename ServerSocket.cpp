#include "SocketC++.h"
#include <string>
#include <iostream>


using namespace std;
using namespace cppsock;

//ServerSocket///////////////////////////////////////////////////////


ServerSocket::ServerSocket()
{
}

ServerSocket::ServerSocket(const char* cPort)
{
	bind("", cPort);
}

ServerSocket::ServerSocket(const char* server_addr, const char* cPort)
{
	bind(server_addr, cPort);
}


ServerSocket::~ServerSocket()
{
}

int ServerSocket::bind(const char* server_addr, const char* cPort, unsigned queue_size)
{
	addrinfo *result = this->addrInfo.ai_next;
	int nRet;
	if (server_addr != "") {
		nRet = ::getaddrinfo(server_addr, cPort, &this->addrInfo, &result);
	}
	else {
		this->addrInfo.ai_flags = AI_PASSIVE;
		nRet = ::getaddrinfo(NULL, cPort, &this->addrInfo, &result);
	}

	if (nRet != 0) {
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
		nRet = -1;
	}
	else if (0 > (nRet = ::bind(mySocket, result->ai_addr, (int)result->ai_addrlen))) {
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
	else if (0 > (nRet = ::listen(mySocket, queue_size))) {
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
	else
	{
		this->addrSock = *result->ai_addr;
		this->addrInfo.ai_addrlen = result->ai_addrlen;
		this->addrInfo.ai_addr = &this->addrSock;
		//this->addrInfo.ai_canonname = result->ai_canonname;
	}
	
	::freeaddrinfo(this->addrInfo.ai_next);

	return nRet;
}

Socket ServerSocket::accept()
{
	sockaddr addr = { 0 };
	int len = (int)this->addrInfo.ai_addrlen;
	
	SOCKET s = ::accept(mySocket, &addr, &len);
	return Socket(s, this->addrInfo, addr);
}

