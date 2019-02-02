#include "../SocketC++.h"
#include <iostream>
#include <string>
//#include <windows.h>
#include <thread>
#include <initializer_list>

using namespace std;
using namespace cppsock;

#define PORT "8130"

void runServer(const char* nPort);

//template<typename T, typename... Args>
//void t_printLn(T t, Args... args);

template <typename T>
void printLn(T t);

template<typename T, typename... Args>
void printLn(T t, Args... args);

int main(int argc, char **argv)
{
	const char* nPort;
	//
	// Check for port argument
	//
	if (argc == 4) {
		printLn("Syntax: PortNumber"); 
		return 0;
	}
	else if (argc < 2)
	{
		nPort = PORT;
	}
	else if (argc == 2)
	{
		nPort = argv[1];
	}
	else
	{
		printLn("Syntax: PortNumber");
		return 0;
	}

	try
	{
		WSA_Utils::initWSA();

		runServer(nPort);

		// Release WinSock
		//
		WSA_Utils::cleanWSA();
	}
	catch (const std::exception& e)
	{
		printLn(e.what());
	}
	return 0;
}

void runServer(const char* nPort)
{	
	try
	{
		ServerSocket server = ServerSocket(nPort);
		if (!server.valid_socket()) 
			throw SocketException(server.getErrString(), TRACEBACK);

		printLn("\nAttente d'une connection");
        Socket client = server.accept();
        if (!client.valid_socket()) {
            throw SocketException(client.getErrString(), TRACEBACK);
        }
		printLn("\nReception en cours...");
		SocketIO stream = client.getStream();
		string message = "";
        stream.recvStr(message);
        cout << "\nMessage recu: "+message << endl;
        cout << "\nEnvoie au client..." << endl;
        stream.sendStr("this is server");
		//cout << endl << stream.sendFile("../../Debug/server.mp4");
		printLn(1.2, "hey", string("hello"), (unsigned long long)456789);
		printLn(1.2, "hey", string("hello"), (unsigned long long)456789);
		printLn(1.2, "hey", string("hello"), (unsigned long long)456789);
		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR);
		}	
	}
	catch (const SocketException& e)
	{
		printLn(e.what());
	}	
}


template <typename T>
void printLn(T t)
{
	std::cout << t << std::endl;
}


template<typename T, typename... Args>
void printLn(T t, Args... args) // recursive variadic function
{
	std::cout << t;
	printLn(args...);
}