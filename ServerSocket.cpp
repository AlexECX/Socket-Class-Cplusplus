#include "SocketC++.h"
#include <string>


using namespace std;
using namespace cppsock;

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

