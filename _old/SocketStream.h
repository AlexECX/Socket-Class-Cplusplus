#pragma once
#include <winsock.h>
#include <string>
#include <memory>

class SocketWrap;

class SocketStream 
{
private:
    std::shared_ptr<SocketWrap> mySocket_ptr = nullptr;
    SOCKET mySocket;
public:
    SocketStream(std::shared_ptr<SocketWrap> socket);
    virtual ~SocketStream();

    /*int sendFile(std::string FilePath);

    int recvFile(std::string FilePath);*/
/*
	int send(const char* msg, unsigned start, unsigned size);

	int send(const string &str, unsigned offset, unsigned len);

	int send(const string &str);

	int recv(char* msg, unsigned start, unsigned size);

    int recvStr(std::string &message);

    void sendStr_Ex(const std::string &message);

    void recvStr_Ex(std::string &message);

    std::string recvStr_Ex();*/
};
