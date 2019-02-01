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
		this->socketError(WSA_ERROR, __FUNCTION__);
		//throw SocketException(WSA_ERROR, __FUNCTION__);
	}
}

BaseSocket::BaseSocket(const SOCKET &socket)
{
    //mySocket_ptr = make_shared<SocketWrap>(socket);
    mySocket = socket;
    if (mySocket == INVALID_SOCKET || mySocket == SOCKET_ERROR)
    {
        this->socketError(WSA_ERROR, __FUNCTION__);
        //throw SocketException(WSA_ERROR, __FUNCTION__);
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
		this->socket_err = other.socket_err;
	}
    return *this;
}

BaseSocket &BaseSocket::operator=(BaseSocket &&other)
{
	//mySocket_ptr = other.mySocket_ptr;
	if (this != &other)
	{
		mySocket = other.mySocket;
		other.autoclose = false;
	}
	return *this;
}

void BaseSocket::socketError(const string &msg, string f)
{
    this->socket_err = f + " " + msg + "\n";
}

//connectionInfo BaseSocket::getIPinfo()
//{
//    int tmp = sizeof(addrInfo);
//    ::getpeername(mySocket, (sockaddr *)&addrInfo, &tmp);
//    connectionInfo info;
//    info.IP = ::inet_ntoa(addrInfo.sin_addr);
//    info.Port = ::ntohs(addrInfo.sin_port);
//    return info;
//}
//
//connectionInfo BaseSocket::getIPinfoLocal()
//{
//    int tmp = sizeof(addrInfo);
//    getsockname(mySocket, (sockaddr *)&addrInfo, &tmp);
//    connectionInfo info;
//    info.IP = ::inet_ntoa(addrInfo.sin_addr);
//    info.Port = ::ntohs(addrInfo.sin_port);
//    return info;
//}

std::string BaseSocket::getSocketErr()
{
    return this->socket_err.substr(this->socket_err.find(" ") + 1);
}

int BaseSocket::shutdown(int how)
{
    int status = ::shutdown(this->mySocket, how);
    if (status < 0)
    {
        this->socketError(WSA_ERROR, __FUNCTION__);
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