#include "SocketC++.h"
#include <string>

using namespace std;
using namespace cppsock;

//BaseSocket///////////////////////////////////////////////////////

BaseSocket::BaseSocket(int af, int type, int protocol)
{
	this->addrInfo.ai_family = af;
	this->addrInfo.ai_socktype = type;
	this->addrInfo.ai_protocol = protocol;
	//mySocket_ptr = make_shared<SocketWrap>(::socket(af, type, protocol));
	mySocket = ::socket(af, type, protocol);

	if (mySocket == INVALID_SOCKET || mySocket == SOCKET_ERROR)
	{
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
}


BaseSocket::BaseSocket(const BaseSocket & other)
{
	*this = other;
}

BaseSocket::BaseSocket(BaseSocket &&other)
{
	*this = std::move(other);
}

BaseSocket::~BaseSocket()
{
	if (this->autoclose)
	{
		::closesocket(this->mySocket);
	}
}

BaseSocket &BaseSocket::operator=(const BaseSocket &other)
{
    //mySocket_ptr = other.mySocket_ptr;
	if (this != &other)
	{
		this->mySocket = other.mySocket;
		this->addrInfo = other.addrInfo;
		this->addrSock = other.addrSock;
		this->addrInfo.ai_addr = &this->addrSock;
		this->socket_err = other.socket_err;
	}
    return *this;
}

BaseSocket &BaseSocket::operator=(BaseSocket &&other)
{
	//mySocket_ptr = other.mySocket_ptr;
	if (this != &other)
	{
		this->mySocket = other.mySocket;
		this->addrInfo = other.addrInfo;
		this->addrSock = other.addrSock;
		this->addrInfo.ai_addr = &this->addrSock;
		this->socket_err = other.socket_err;
		other.autoclose = false;
	}
	return *this;
}

void BaseSocket::socketError(/*const string &msg, const string &f*/)
{
	this->wsa_err = WSAGetLastError();
}

connectionInfo BaseSocket::getIPinfo()
{
	connectionInfo info;

	if (addrInfo.ai_addr->sa_family == AF_INET) {
		//info.Port = htons(((sockaddr_in*)addrInfo.ai_addr)->sin_port);
		sockaddr_in *ptr = ((sockaddr_in*)addrInfo.ai_addr);
		info.Port = htons(ptr->sin_port);
		char buff[INET_ADDRSTRLEN];
		::inet_ntop(ptr->sin_family, &ptr->sin_addr, buff, INET_ADDRSTRLEN);
		info.IP = buff;
	}
	else
	{
		//info.Port = htons(((sockaddr_in6*)addrInfo.ai_addr)->sin6_port);
		sockaddr_in6 *ptr = ((sockaddr_in6*)addrInfo.ai_addr);
		info.Port = htons(ptr->sin6_port);
		char buff[INET6_ADDRSTRLEN];
		::inet_ntop(ptr->sin6_family, &ptr->sin6_addr, buff, INET6_ADDRSTRLEN);
		info.IP = buff;
	}
    
	return info;
}

connectionInfo BaseSocket::getIPinfoLocal()
{
    connectionInfo info;
	sockaddr t = { 0 };
	int iNlen = (int)addrInfo.ai_addrlen;
	::getsockname(mySocket, &t, &iNlen);
	//::inet_ntop(t.sin_family, &t.sin_addr, info.IP, INET_ADDRSTRLEN);

	if (addrInfo.ai_addr->sa_family == AF_INET) {
		//info.Port = htons(((sockaddr_in*)addrInfo.ai_addr)->sin_port);
		sockaddr_in *ptr = ((sockaddr_in*)&t);
		info.Port = htons(ptr->sin_port);
		char buff[INET_ADDRSTRLEN];
		::inet_ntop(ptr->sin_family, &ptr->sin_addr, buff, INET_ADDRSTRLEN);
		info.IP = buff;
	}
	else
	{
		//info.Port = htons(((sockaddr_in6*)addrInfo.ai_addr)->sin6_port);
		sockaddr_in6 *ptr = ((sockaddr_in6*)&t);
		info.Port = htons(ptr->sin6_port);
		char buff[INET_ADDRSTRLEN];
		::inet_ntop(ptr->sin6_family, &ptr->sin6_addr, buff, INET_ADDRSTRLEN);
		info.IP = buff;
	}
    
	return info;
}

std::string BaseSocket::getErrString()
{
	return WSA_Utils::formatWSAError(this->getErr());
}

int BaseSocket::shutdown(int how)
{
    int status = ::shutdown(this->mySocket, how);
    if (status < 0)
    {
        this->socketError(/*WSA_ERROR, __FUNCTION__*/);
        return status;
    }
    return status;
}

void BaseSocket::close()
{
    ::closesocket(mySocket);
}

SocketIO BaseSocket::getStream()
{
	return SocketIO(this->mySocket);
}