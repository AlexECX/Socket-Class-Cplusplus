#pragma once
#include "WSA_Utils.h"
#include <stdexcept>
#include <string>
#include <memory>


namespace cppsock {

    #define TRACEBACK __FILE__, __LINE__, __FUNCTION__

    //Wrapper to help manage SOCKET lifetime
    /*class SocketWrap
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
    };*/

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


	class SocketIOInterf
	{
	public:
		virtual int getStatus() = 0;
		virtual void setFlags(int flags) = 0;

	private:

	};


    class SocketIO
    {
	private:
    protected:
        //std::shared_ptr<SocketWrap> mySocket_ptr = nullptr;
        SOCKET mySocket;
		int nRet = 1;
		int flags = 0; //flags parameter of winsock functions
		static const int bufflen = 4096;
		char iobuff[bufflen];
    public:
        SocketIO(const SOCKET &socket);
        virtual ~SocketIO();

		//negative if error, 0 if closed by peer
		int getStatus() { return nRet; }

		//flags argument of the winsock functions
		void setFlags(int flags) { this->flags = flags; }

		//winsock::recv wrapper, receives only once
		int recv(char* msg, int offset, int len);

		//send until len characters are sent
		int send(const char* msg, int len);

		//recv until len characters are received
		int recv(char* msg, int len);

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
			return recv((char*)&i, 0, sizeof(T));
		}

		//Adapter for C strings
		size_t send(const char* str);

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

		size_t recvLarge(char* msg, size_t offset, size_t len);

		//send a files length followed by a file read in 5mb chunks
		size_t sendFile(const std::string &filePath);

		//recv a files length followed by a file written in 5mb chunks
		size_t recvFile(const std::string &filePath);
    };

    class BaseSocket
    {

    private:
    protected:
		bool		autoclose = true;
		int			wsa_err = 0;
        SOCKET		mySocket = INVALID_SOCKET;
		SOCKET_ADDRESS addrsock;
		int			ai_flags = 0;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
		int			ai_family = 0;      // PF_xxx
		int			ai_socktype = 0;    // SOCK_xxx
		int			ai_protocol = 0;    // 0 or IPPROTO_xxx for IPv4 and IPv6
		char *		ai_canonname = nullptr;   // Canonical name for nodename

        void socketError(/*const std::string &msg, const std::string &f*/);

    public:
		BaseSocket();
		BaseSocket(int af, int type, int protocol);
		BaseSocket(const SOCKET& socket, SOCKET_ADDRESS addr, int type, int protocol);
		BaseSocket(const BaseSocket &other);
		BaseSocket(BaseSocket &&other);
		BaseSocket &operator=(BaseSocket &other);
		BaseSocket &operator=(BaseSocket &&other);

        virtual ~BaseSocket();

        connectionInfo getIPinfo();

        connectionInfo getIPinfoLocal();

        std::string getErrString();

		int getErr() { return wsa_err; }

        int shutdown(int how);

        void close();

        bool valid_socket() { return !(mySocket == INVALID_SOCKET); }

		SocketIO getStream();
    };

	class SocketInterf
	{
	public:

		virtual int connect(const char* server_addr, const char* cPort) = 0;
		virtual int connect(const sockaddr* name, int namelen) = 0;
	};


    class Socket : public BaseSocket, public SocketInterf
	{
	
	protected:
		friend class ServerSocket;

    public:
		//Socket();
		Socket(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
		Socket(const SOCKET & socket, SOCKET_ADDRESS addr, int type, int protocol);
		Socket(const Socket& other);
		Socket(Socket&& other);
		Socket(const char* server_addr, const char* cPort);
		Socket(const sockaddr * name, int namelen);
		virtual ~Socket();

		Socket &operator=(Socket& other) {
			BaseSocket::operator=(other);
			return *this;
		}

		Socket &operator=(Socket&& other) {
			BaseSocket::operator=(std::move(other));
			return *this;
		}

        int connect(const char* server_addr, const char* cPort) override;
		int connect(const sockaddr* name, int namelen) override;
    };


	class ServerInterf
	{
	public:

		virtual int bind(const char* server_addr, const char* cPort, unsigned queue_size) = 0;
		virtual int bind(const sockaddr* name, int namelen, unsigned queue_size) = 0;
		virtual Socket accept() = 0;
	};


	class ServerSocket : public BaseSocket, public ServerInterf
	{
	public:
		//ServerSocket();
		ServerSocket(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
		ServerSocket(const SOCKET & socket, SOCKET_ADDRESS addr, int type, int protocol);
		ServerSocket(const ServerSocket& other);
		ServerSocket(ServerSocket&& other);
		ServerSocket(const char* cPort);
		ServerSocket(const char* server_addr, const char* cPort);
		virtual ~ServerSocket();

		ServerSocket &operator=(ServerSocket& other) {
			BaseSocket::operator=(other);
			return *this;
		}

		ServerSocket &operator=(ServerSocket&& other) {
			BaseSocket::operator=(std::move(other));
			return *this;
		}

		int bind(const char* server_addr, const char* cPort, unsigned queue_size = SOMAXCONN);

		int bind(const sockaddr* name, int namelen, unsigned queue_size = SOMAXCONN);

		Socket accept();
	};

}