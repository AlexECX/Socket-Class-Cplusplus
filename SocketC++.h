#pragma once
#include <winsock.h>
#include <stdexcept>
#include <string>
#include <memory>
#include "WSA_Utils.h"

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
        char *IP;
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

    public:
        SocketIO(const SOCKET &socket);
        virtual ~SocketIO();

		std::string getSocketErr();

		//send a files length followed by a file read in 5mb chunks
        int sendFile(const std::string &filePath);

		//recv a files length followed by a file written in 5mb chunks
        int recvFile(const std::string &filePath);

		//send a null terminated string
		int sendStr(const std::string &str);

		//recv until null terminator is encountered
		int recvStr(std::string &str);

		//send until sizeof(int) bytes are sent
		int send(const int i);
    
		//winsock::send wrapper, sends only once
		int send(const char* msg, unsigned offset, unsigned len);

		//send until len characters are sent
		int send(const char* msg, unsigned len);
		
		//send until len characters starting at offset position are sent
		int send(const std::string &str, unsigned offset, unsigned len);

		//send the content of a string, no std::string null terminator
		int send(const std::string &str);

		//recv until sizeof(int) bytes are received
		int recv(int& i);

		//winsock::recv wrapper, receives only once
        int recv(char* msg, unsigned offset, unsigned len);

		//recv until len characters are received
		int recv(char* msg, unsigned len);

		//recv into string container until connection is closed
        int recv(std::string &str);

    };

    class BaseSocket
    {

    private:
    protected:
        int af, type, protocol;
		bool autoclose = true;
        SOCKET mySocket;
        SOCKADDR_IN addrInfo = {0};
        std::string socket_err = "";

        BaseSocket(const SOCKET &socket);
        void socketError(const std::string &msg, std::string f);

    public:
		BaseSocket(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
		BaseSocket(const BaseSocket &other);
		BaseSocket(BaseSocket &&other);
		BaseSocket &operator=(const BaseSocket &other);
		BaseSocket &operator=(BaseSocket &&other);

        virtual ~BaseSocket();

        connectionInfo getIPinfo();

        connectionInfo getIPinfoLocal();

        std::string getSocketErr();

        int shutdown(int how);

        void close();

        bool valid_socket() { return mySocket > 0; }

		SocketIO getStream();

        //int sendFile(std::string FilePath);

        //int recvFile(std::string FilePath);

        ////permet d'envoyer une string. Si trop grand,
        ////l'envoie en plusieurs paquets.
        //int sendStr(const std::string &message);

        ////permet de recevoir une string. Si trop grand,
        ////est pret a recevoir en plusieurs paquets.
        //int recvStr(std::string &message);

        //void sendStr_Ex(const std::string &message);

        //void recvStr_Ex(std::string &message);

        //std::string recvStr_Ex();

        
    };


    class Socket : public BaseSocket 
	{
	friend class ServerSocket;

	private:
		Socket(const SOCKET &socket);

    public:
        Socket();
        Socket(const char* server_addr, unsigned short cPort);
        virtual ~Socket();

		Socket &operator=(const Socket& other) {
			BaseSocket::operator=(other);
			return *this;
		}

        int connect(const char* server_addr, unsigned short cPort);
    };


	class ServerSocket : public BaseSocket
	{
	public:
		ServerSocket();
		ServerSocket(unsigned short cPort);
		ServerSocket(const char* server_addr, unsigned short cPort);
		virtual ~ServerSocket();

		ServerSocket &operator=(const ServerSocket& other) {
			BaseSocket::operator=(other);
			return *this;
		}

		int bind(const char* server_addr, unsigned short cPort, unsigned queue_size = 5);

		Socket accept();
	};

}