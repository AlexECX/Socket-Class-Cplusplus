#include "SocketC++.h"
#include <string>

using namespace std;
using namespace cppsock;

//BaseSocket///////////////////////////////////////////////////////

BaseSocket::BaseSocket()
{
}

BaseSocket::BaseSocket(int af, int type, int protocol)
{
	mySocket = ::socket(af, type, protocol);
	this->ai_family = af;
	this->ai_socktype = type;
	this->ai_protocol = protocol;

	if (mySocket == INVALID_SOCKET || mySocket == SOCKET_ERROR)
	{
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
}

BaseSocket::BaseSocket(const SOCKET & socket, SOCKET_ADDRESS addr, int type, int protocol)
{
	this->ai_family = addr.lpSockaddr->sa_family;
	this->ai_socktype = type;
	this->ai_protocol = protocol;
	this->addrsock = addr;

	mySocket = socket;
	if (mySocket == INVALID_SOCKET || mySocket == SOCKET_ERROR)
	{
		this->socketError(/*WSA_ERROR, __FUNCTION__*/);
	}
}


BaseSocket::BaseSocket(const BaseSocket & other)
{
	if (this != &other)
	{
		if (this->autoclose)
		{
			this->close();
		}
		this->mySocket = other.mySocket;
		this->ai_flags = other.ai_flags;
		this->ai_family = other.ai_family;
		this->ai_socktype = other.ai_socktype;
		this->ai_protocol = other.ai_protocol;
		this->ai_canonname = other.ai_canonname;
		this->addrsock = other.addrsock;
		this->wsa_err = other.wsa_err;
		
		this->autoclose = (other.autoclose) ? false : other.autoclose;
	}
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

BaseSocket &BaseSocket::operator=(BaseSocket &other)
{
	if (this != &other)
	{
		if (this->autoclose)
		{
			this->close();
		}
		this->mySocket = other.mySocket;
		this->ai_flags = other.ai_flags;
		this->ai_family = other.ai_family;
		this->ai_socktype = other.ai_socktype;
		this->ai_protocol = other.ai_protocol;
		this->ai_canonname = other.ai_canonname;
		this->addrsock = other.addrsock;
		this->wsa_err = other.wsa_err;
		
		this->autoclose = other.autoclose;

		other.autoclose = false;
	}
	return *this;
}

BaseSocket &BaseSocket::operator=(BaseSocket &&other)
{
	//mySocket_ptr = other.mySocket_ptr;
	if (this != &other)
	{
		if (this->autoclose)
		{
			this->close();
		}
		this->mySocket = other.mySocket;
		this->ai_flags = other.ai_flags;
		this->ai_family = other.ai_family;
		this->ai_socktype = other.ai_socktype;
		this->ai_protocol = other.ai_protocol;
		this->ai_canonname = other.ai_canonname;
		this->addrsock = other.addrsock;
		this->wsa_err = other.wsa_err;
		
		this->autoclose = other.autoclose;

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
	sockaddr* addr = this->addrsock.lpSockaddr;
	if (addr->sa_family == AF_INET) {
		//info.Port = htons(((sockaddr_in*)this->ai_addr)->sin_port);
		sockaddr_in *ptr = ((sockaddr_in*)addr);
		info.Port = htons(ptr->sin_port);
		char buff[INET_ADDRSTRLEN];
		::inet_ntop(ptr->sin_family, &ptr->sin_addr, buff, INET_ADDRSTRLEN);
		info.IP = buff;
	}
	else
	{
		//info.Port = htons(((sockaddr_in6*)this->ai_addr)->sin6_port);
		sockaddr_in6 *ptr = ((sockaddr_in6*)addr);
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
	
	//::inet_ntop(t.sin_family, &t.sin_addr, info.IP, INET_ADDRSTRLEN);

	if (this->addrsock.lpSockaddr->sa_family == AF_INET) {
		int iNlen = sizeof(sockaddr_in);
		::getsockname(mySocket, &t, &iNlen);
		//info.Port = htons(((sockaddr_in*)this->ai_addr)->sin_port);
		sockaddr_in *ptr = ((sockaddr_in*)&t);
		info.Port = htons(ptr->sin_port);
		char buff[INET_ADDRSTRLEN];
		::inet_ntop(ptr->sin_family, &ptr->sin_addr, buff, INET_ADDRSTRLEN);
		info.IP = buff;
	}
	else
	{
		int iNlen = sizeof(sockaddr_in6);
		::getsockname(mySocket, &t, &iNlen);
		//info.Port = htons(((sockaddr_in6*)this->ai_addr)->sin6_port);
		sockaddr_in6 *ptr = ((sockaddr_in6*)&t);
		info.Port = htons(ptr->sin6_port);
		char buff[INET6_ADDRSTRLEN];
		::inet_ntop(ptr->sin6_family, &ptr->sin6_addr, buff, INET6_ADDRSTRLEN);
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