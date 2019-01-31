#include "SocketC++.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace cppsock;
using namespace std;

//SocketException///////////////////////////////////////////////////////

SocketException::SocketException(const string & msg) :
	std::runtime_error(msg) {
	stream_Msg = "Traceback: \n";
	stream_Msg += msg + ": SocketException";
}

SocketException::SocketException(const string& msg, const char *file, int line, string f) :
	std::runtime_error(msg) {
	stream_Msg = "Traceback:";
	stream_Msg += "\n  File \"" + string(file) + "\", line " + to_string(line) + " in " + f;
	stream_Msg += "\nSocketException: " + msg;

}

char const * SocketException::what() const throw()
{
	return stream_Msg.c_str();
}

SocketException::~SocketException()
{
}


//SocketIO///////////////////////////////////////////////////////

SocketIO::SocketIO(const SOCKET &socket){
    //mySocket_ptr = socket_ptr;
	mySocket = socket;
}

SocketIO::~SocketIO()
{
}

string SocketIO::getSocketErr()
{
	return WSA_ERROR;
}

int SocketIO::sendFile(const string &filePath)
{
    int nRet;
    unsigned file_size = 0;

    std::ifstream file_source(filePath.c_str(), std::ios::binary);
    if (file_source.fail() || file_source.tellg() < 0)
    {
        //Send 0 to client, to tell no file was found
		if ((nRet = send(0)) < 0) {
			return nRet;
		}
        
    }
    //Get the requested file

    //Get and send file size
    file_source.seekg(0, std::ios::end);

    file_size = file_source.tellg();

    if ((nRet = send(file_size)) < 0) {
        return nRet;
    }

    //Send file in multiple packets
    nRet = 0;
    unsigned sent = 0;
    unsigned size = 0;
    unsigned progress = 1;

	if (file_size < 5000000)
    {
		size = file_size;
    }
    else
    {
		size = 5000000;
    }

	auto file_buffer = std::make_unique<char[]>(size);

    /** Tool for progress bar 1/2*/
    double progresStep = static_cast<double>(file_size) / 10.0;
    double progression = 0;
    cout << "Progression '*'x10 [ ";
    /**/

    do
    {
        sent += nRet;

        /** Tool for progress bar 2 / 2*/
        if (sent > progresStep * progression)
        {
            while (progresStep * progression < sent)
            {
                if (progression > 8)
                    cout << "* ]" << endl;
                else
                    cout << "* ";
                progression++;
            }
        }
        /**/

        if (file_size - sent >= 5000000)
        {
            size = 5000000;
        }
        else
        {
            size = file_size - sent;
        }

        file_source.seekg(sent, std::ios::beg);
        file_source.read(file_buffer.get(), size);

    } while (sent < file_size && (nRet = send(file_buffer.get(), size)) > 0);

    file_source.close();

    return (nRet < 0) ? nRet : sent;
}

int SocketIO::recvFile(const string &filePath)
{
    int nRet;
    unsigned file_size;
	int ibuff;

	if ((nRet = recv(ibuff)) < 0)
	{
		return nRet;
	}

    if (ibuff == 0)
    {
        throw SocketException("RemoteFileNotFound", TRACEBACK);
    }
	else
	{
		file_size = ibuff;
	}

    ofstream IMG_dest(filePath.c_str(), ios::binary);
    if (IMG_dest.fail())
    {
		throw SocketException("CannotOpenFile", TRACEBACK);
    }

    //Receive file in multiple pakcets
	unsigned recved = 0;
	unsigned size = 0;
	unsigned progress = 1;

	if (file_size < 5000000)
	{
		size = file_size;
	}
	else
	{
		size = 5000000;
	}

	auto file_buffer = std::make_unique<char[]>(size);
	IMG_dest.seekp(0, ios::beg);

	/** Tool for progress bar 1/2*/
	double progresStep = static_cast<double>(file_size) / 10.0;
	double progression = 0;
	cout << "Progression '*'x10 [ ";
	/**/

	while (recved < file_size && (nRet = recv(file_buffer.get(), 0, size)) > 0) {
		recved += nRet;
		IMG_dest.write(file_buffer.get(), nRet);

		/** Tool for progress bar 2 / 2*/
		if (recved > progresStep * progression)
		{
			while (progresStep * progression < recved)
			{
				if (progression > 8)
					cout << "* ]" << endl;
				else
					cout << "* ";
				progression++;
			}
		}
		/**/

	} 
    IMG_dest.close();

    return (nRet < 0) ? nRet : recved;
}

int SocketIO::sendStr(const std::string & str)
{
	return send(str.c_str(), str.size() + 1);
}

int SocketIO::recvStr(std::string & str)
{
	int nRet;
	unsigned recved = 0;
	str.clear();
	char reception[4096];

	while ((nRet = recv(reception, 0, 4096)) > 0) {
		recved += nRet;
		if (reception[nRet - 1] == '\0')
		{
			str.append(reception, nRet - 1);
			break;
		}
		else
		{
			str.append(reception, nRet);
		}
	}

	return (nRet < 0) ? nRet : recved;
}

int SocketIO::send(const int i)
{
	return send((char *)&i, sizeof(int));
}

int SocketIO::send(const char * msg, unsigned offset, unsigned len)
{
	return ::send(mySocket, msg + offset, len, 0);
}

int SocketIO::send(const char * msg, unsigned len)
{
	int nRet;
	unsigned sent = 0;
	while (sent < len && (nRet = send(msg, sent, len - sent)) > 0) {
		sent += nRet;
	}
	return (nRet < 0) ? nRet : sent;
}

int SocketIO::send(const string &str, unsigned offset, unsigned len)
{
	return send(str.c_str() + offset, len);
}

int SocketIO::send(const string &str) 
{
	return send(str.c_str(), str.size());
}

int SocketIO::recv(int & i)
{
	return recv((char*)&i, sizeof(int));
}

int SocketIO::recv(char * msg, unsigned offset, unsigned len)
{
	return ::recv(mySocket, msg + offset, len, 0);
}

int SocketIO::recv(char * msg, unsigned len)
{
	int nRet;
	unsigned recved = 0;
	while (recved < len && (nRet = recv(msg, recved, len - recved)) > 0) {
		recved += nRet;
	}
	return (nRet < 0) ? nRet : recved;
}

int SocketIO::recv(string& str)
{
    int nRet;
    unsigned recved = 0;
	str.clear();
	char reception[4096];
	
	while ((nRet = recv(reception, 0, 4096)) > 0) {
		str.append(reception, nRet);
		recved += nRet;
	}
	return (nRet < 0) ? nRet : recved;
}



//BaseSocket///////////////////////////////////////////////////////

BaseSocket::BaseSocket(int af, int type, int protocol)
{
	this->af = af;
	this->type = type;
	this->protocol = protocol;
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
		this->af = other.af;
		this->type = other.type;
		this->protocol = other.protocol;
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

connectionInfo BaseSocket::getIPinfo()
{
    int tmp = sizeof(addrInfo);
    ::getpeername(mySocket, (sockaddr *)&addrInfo, &tmp);
    connectionInfo info;
    info.IP = ::inet_ntoa(addrInfo.sin_addr);
    info.Port = ::ntohs(addrInfo.sin_port);
    return info;
}

connectionInfo BaseSocket::getIPinfoLocal()
{
    int tmp = sizeof(addrInfo);
    getsockname(mySocket, (sockaddr *)&addrInfo, &tmp);
    connectionInfo info;
    info.IP = ::inet_ntoa(addrInfo.sin_addr);
    info.Port = ::ntohs(addrInfo.sin_port);
    return info;
}

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


//ServerSocket///////////////////////////////////////////////////////


ServerSocket::ServerSocket()
{
}

ServerSocket::ServerSocket(unsigned short cPort)
{
	bind("", cPort);
}

ServerSocket::ServerSocket(const char* server_addr, unsigned short cPort)
{
	bind(server_addr, cPort);
}


ServerSocket::~ServerSocket()
{
}

int ServerSocket::bind(const char* server_addr, unsigned short cPort, unsigned queue_size)
{
	addrInfo.sin_family = this->af;

	if (server_addr != "") {
		addrInfo.sin_addr.s_addr = ::inet_addr(server_addr);
		if (addrInfo.sin_addr.s_addr == INADDR_NONE) {
			LPHOSTENT lpHostEntry = ::gethostbyname(server_addr);
			if (lpHostEntry == NULL) {
				throw SocketException(string(server_addr) + " invalid parameter", TRACEBACK);
			}
			else {
				addrInfo.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
			}
		}
	}
	else {
		addrInfo.sin_addr.s_addr = INADDR_ANY;   }
	
	addrInfo.sin_port = cPort;        // Use port from command line

	int status;
	status = ::bind(mySocket, (LPSOCKADDR)&addrInfo, sizeof(struct sockaddr));
	if (status < 0) {
		this->socketError(WSA_ERROR, __FUNCTION__);
	}
	else if ((status = ::listen(mySocket, queue_size)) < 0)
	{
		this->socketError(WSA_ERROR, __FUNCTION__);
	}
	return status;
}

Socket ServerSocket::accept()
{
	//
// Wait for an incoming request
	return ::accept(mySocket, 0, 0);
}

