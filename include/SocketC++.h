#pragma once
#include "WSA_Utils.h"
#include <stdexcept>
#include <string>
#include <memory>


namespace cppsock {

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

    struct connectionInfo
    {
        std::string IP;
        u_short Port;
    };

    class SocketException : public std::runtime_error
    {
    private:
        std::string stream_Msg;

    public:
        SocketException(const std::string& msg);
        SocketException(const std::string& msg, const char *file, 
                            int line, std::string f);
                
        virtual char const* what() const throw();

        ~SocketException() throw();
    };

    class SocketIO 
    {
    private:
        //std::shared_ptr<SocketWrap> mySocket_ptr = nullptr;
        SOCKET mySocket;
		int nRet = 1;
		int flags = 0; //flags parameter of winsock functions
		char iobuffer[4096];

    public:
        SocketIO(const SOCKET &socket);
        virtual ~SocketIO();

		//negative if error, 0 if closed by peer
		int getStatus() { return nRet; }

		//flags argument of the winsock functions
		void setFlags(int flags) { this->flags = flags; }

		//send until len characters are sent
		int send(const char* msg, int len);

		//winsock::recv wrapper, receives only once
		int recv(char* msg, int len);

		//recv until len characters are received
		int recv(char* msg, int offset, int len);

		/*send until sizeof(T) bytes are sent*/
		template <class T>
		int send(const T i) 
		{
			return send((char *)&i, sizeof(T));
		}

		//recv until sizeof(T) bytes are received
		template <class T>
		int recv(T& i) 
		{
			return recv((char*)&i, sizeof(T));
		}

		//send until len characters starting at offset position are sent
		size_t send(const std::string &str, size_t offset, size_t len);

		//send the content of a string w/out std::string null terminator
		size_t send(const std::string &str);

		//recv into string container until len characters are received
		size_t recv(std::string &str, size_t len);

		//recv into string container until connection is closed
		size_t recv(std::string &str);

		//send a null terminated string
		size_t sendStr(const std::string &str);

		//recv until null terminator is encountered
		size_t recvStr(std::string &str);

		size_t sendLarge(const char* msg, size_t offset, size_t len);

		size_t recvLarge(const char* msg, size_t offset, size_t len);

		//send a files length followed by a file read in 5mb chunks
		size_t sendFile(const std::string &filePath);

		//recv a files length followed by a file written in 5mb chunks
		size_t recvFile(const std::string &filePath);
    };

    class BaseSocket
    {

    private:
    protected:
		bool autoclose = true;
		int wsa_err = 0;
        SOCKET mySocket;
		addrinfo addrInfo = { 0 };
		sockaddr addrSock = { 0 };
        std::string socket_err = "";

        void socketError(/*const std::string &msg, const std::string &f*/);

    public:
		BaseSocket(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
		BaseSocket(const BaseSocket &other);
		BaseSocket(BaseSocket &&other);
		BaseSocket &operator=(const BaseSocket &other);
		BaseSocket &operator=(BaseSocket &&other);

        virtual ~BaseSocket();

        connectionInfo getIPinfo();

        connectionInfo getIPinfoLocal();

        std::string getErrString();

		int getErr() { return wsa_err; }

        int shutdown(int how);

        void close();

        bool valid_socket() { return !(mySocket == INVALID_SOCKET || mySocket == SOCKET_ERROR); }

		SocketIO getStream();
    };


    class Socket : public BaseSocket 
	{
	friend class ServerSocket;

	private:
		Socket(const SOCKET&, const addrinfo&, const sockaddr&);

    public:
        Socket();
        Socket(const char* server_addr, const char* cPort);
        virtual ~Socket();

		Socket &operator=(const Socket& other) {
			BaseSocket::operator=(other);
			return *this;
		}

        int connect(const char* server_addr, const char* cPort);

		int connect(const sockaddr* name, int namelen);
    };


	class ServerSocket : public BaseSocket
	{
	public:
		ServerSocket();
		ServerSocket(const char* cPort);
		ServerSocket(const char* server_addr, const char* cPort);
		virtual ~ServerSocket();

		ServerSocket &operator=(const ServerSocket& other) {
			BaseSocket::operator=(other);
			return *this;
		}

		int bind(const char* server_addr, const char* cPort, unsigned queue_size = SOMAXCONN);

		int bind(const sockaddr* name, int namelen, unsigned queue_size = SOMAXCONN);

		Socket accept();
	};

}