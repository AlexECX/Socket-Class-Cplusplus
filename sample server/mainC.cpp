#include <iostream>
#include <string>
//#include <windows.h>
#include <thread>
#include "../SocketC++.h"
#include <limits>
#include <istream>

using namespace std;
using namespace cppsock;

#define PORT "8130"

void runServer(const char* nPort);

int main(int argc, char **argv)
{
	const char* nPort;
	//
	// Check for port argument
	//
	if (argc > 2) {
		cout << "\nSyntax: PortNumber"; 
		cout << endl;
		return 0;
	}
	else if (argc < 2)
	{
		nPort = PORT;
	}
	else
	{
		nPort = argv[1];
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
		cout << endl << e.what();
	}
	return 0;
}

void runServer(const char* nPort)
{	
	try
	{
		ServerSocket server = ServerSocket(nPort);
		if (!server.valid_socket()) 
			throw SocketException(server.getSocketErr(), TRACEBACK);

        cout << "\nAttente d'une connection" << endl;
        Socket client = server.accept();
        if (!client.valid_socket()) {
            throw SocketException(client.getSocketErr(), TRACEBACK);
        }
        cout << "\nReception en cours..." << endl;
		SocketIO stream = client.getStream();
		string message = "";
        /*stream.recvStr(message);
        cout << "\nMessage recu: "+message << endl;
        cout << "\nEnvoie au client..." << endl;*/
        //stream.send("this is server");
		cout << endl << stream.sendFile("../../Debug/server.mp4");
		
		if (stream.ioInterupt())
		{
			cout << endl << stream.getIOError();
			cout << endl << stream.getSocketErr();
		}	
	}
	catch (const SocketException& e)
	{
		cout << endl << e.what();
	}	
}
