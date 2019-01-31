#pragma once
#include <winsock.h>
#include <string>
#include <memory>
#include "WSA_Utils.h"

#define TRACEBACK __FILE__, __LINE__, __FUNCTION__

//Wrapper to help manage SOCKET lifetime
class SocketWrap
{
private:
	SOCKET theSocket;

public:
	SocketWrap(SOCKET socket)
	{
		theSocket = socket;
	}
	~SocketWrap()
	{
		closesocket(theSocket);
	}

	SOCKET &getSocket()
	{
		return theSocket;
	}
};

class SocketStream;

struct connectionInfo
{
    char *IP;
    u_short Port;
};

class BaseSocket
{
  private:
  protected:
    int af, type, protocol;
    std::shared_ptr<SocketWrap> mySocket_ptr = nullptr;
    SOCKET mySocket;
    SOCKADDR_IN addrInfo = {0};
    std::string socket_err = "";

    BaseSocket(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
    BaseSocket(const SOCKET &socket);
    BaseSocket &operator=(const BaseSocket &other);
    void socketError(const std::string &msg, std::string f);

  public:
    virtual ~BaseSocket();

    connectionInfo getIPinfo();

    connectionInfo getIPinfoLocal();

    std::string getSocketErr();

    int shutdown(int how);

    void close();

    bool valid_socket() { return mySocket > 0; }

    int sendFile(std::string FilePath);

    int recvFile(std::string FilePath);

    //permet d'envoyer une string. Si trop grand,
    //l'envoie en plusieurs paquets.
    int sendStr(const std::string &message);

    //permet de recevoir une string. Si trop grand,
    //est pret a recevoir en plusieurs paquets.
    int recvStr(std::string &message);

    void sendStr_Ex(const std::string &message);

    void recvStr_Ex(std::string &message);

    std::string recvStr_Ex();

	SocketStream getStream();
};
