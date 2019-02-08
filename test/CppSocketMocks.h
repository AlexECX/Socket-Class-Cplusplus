#pragma once
#include "SocketC++.h"

using namespace cppsock;

class MockSocket : public Socket
{
public:
	int closeCount = 0;
	MockSocket(){}
	~MockSocket() {}
	MockSocket(const MockSocket& other) : Socket(other) { if (autoclose) close(); }
	MockSocket(MockSocket&& other) : Socket(std::move(other)) {}
	MockSocket& operator=(MockSocket& other) { 
		if (autoclose) close();
		Socket::operator=(other); 
		return *this; 
	}
	MockSocket& operator=(MockSocket&& other) { 
		if (autoclose) close();
		Socket::operator=(std::move(other)); 
		return *this; 
	}

	bool getAutoClose() { return autoclose; }
    int getWsaError() { return wsa_err; }
    SOCKET getMySocket() { return mySocket; }

	void close() {
		this->closeCount++;
	}

	MOCK_METHOD2(connect, int(const char* server_addr, const char* cPort));
	MOCK_METHOD2(connect, int(const sockaddr* name, int namelen));
};

class MockServerSocket : public ServerSocket
{
public:
	int closeCount = 0;
	MockServerSocket() {}
	~MockServerSocket() {}
	MockServerSocket(const MockServerSocket& other) : ServerSocket(other) { if (autoclose) close(); }
	MockServerSocket(MockServerSocket&& other) : ServerSocket(std::move(other)) {}
	MockServerSocket& operator=(MockServerSocket& other) {
		if (autoclose) close();
		ServerSocket::operator=(other);
		return *this;
	}
	MockServerSocket& operator=(MockServerSocket&& other) {
		if (autoclose) close();
		ServerSocket::operator=(std::move(other));
		return *this;
	}

	bool getAutoClose() { return autoclose; }
	int getWsaError() { return wsa_err; }
	SOCKET getMySocket() { return mySocket; }

	void close() {
		this->closeCount++;
	}

	MOCK_METHOD3(bind, int(const char* server_addr, const char* cPort, unsigned queue_size));
	MOCK_METHOD3(bind, int(const sockaddr* name, int namelen, unsigned queue_size));
	MOCK_METHOD0(accept, Socket());
	MOCK_METHOD2(accept, Socket(sockaddr * addr, int * addrlen));
};

class MockSocketIO : public SocketIO
{
public:
	MockSocketIO(const SOCKET &socket) : SocketIO(socket){}
	virtual ~MockSocketIO(){}
	char* getIObuffer() { return this->iobuff; }
	int getBuffLen() { return this->bufflen; }

	//send until len characters are sent
	virtual int send(const char* msg, int len) { return SocketIO::send(msg, len); }

	//recv until len characters are received
	virtual int recv(char* msg, int len) { return SocketIO::recv(msg, len); }

	/*send until sizeof(T) bytes are sent*/
	template <class T>
	int send(const T i)
	{
		return SocketIO::send((char *)&i, sizeof(T));
	}

	//recv until sizeof(T) bytes are received
	template <class T>
	int recv(T& i)
	{
		return SocketIO::recv((char*)&i, 0, sizeof(T));
	}

	//Adapter for C strings
	virtual size_t send(const char* str) { return SocketIO::send(str); }

	//send until len characters starting at offset position are sent
	virtual size_t send(const std::string &str, size_t offset, size_t len) { return SocketIO::send(str, offset, len); }

	//send the content of a string w/out std::string null terminator
	virtual size_t send(const std::string &str) { return SocketIO::send(str); }

	//recv into string container until len characters are received
	virtual size_t recv(std::string &str, size_t len) { return SocketIO::recv(str, len); }

	//recv into string container until connection is closed
	virtual size_t recv(std::string &str) { return SocketIO::recv(str); }

	//send a null terminated string
	virtual size_t sendStr(const std::string &str) { return SocketIO::sendStr(str); }

	//recv until null terminator is encountered
	virtual size_t recvStr(std::string &str) { return SocketIO::recvStr(str); }

	virtual size_t sendLarge(const char* msg, size_t offset, size_t len) { return SocketIO::sendLarge(msg, offset, len); }

	virtual size_t recvLarge(char* msg, size_t offset, size_t len) { return SocketIO::recvLarge(msg, offset, len); }

	//send a files length followed by a file read in 5mb chunks
	virtual size_t sendFile(const std::string &filePath) { return SocketIO::sendFile(filePath); }

	//recv a files length followed by a file written in 5mb chunks
	virtual size_t recvFile(const std::string &filePath) { return SocketIO::recvFile(filePath); }

};
