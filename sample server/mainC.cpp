#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include "../SocketC++.h"

using namespace std;
using namespace cppsock;

#define HARD_CODED
#define PORT 2030

void runServer(unsigned nPort);

int main(int argc, char **argv)
{
	unsigned nPort;

	//
	// Check for port argument
	//
#ifdef HARD_CODED
	nPort = PORT;
#else
	if (argc != 2) {
		cout << "\nSyntax: PortNumber"; 
		cout << endl;
		return 0;
	}
	nPort = atoi(argv[1]);

#endif // HARD_CODED

	WSA_Utils::initWSA();

	runServer(nPort);

	// Release WinSock
	//
	WSA_Utils::cleanWSA();
	return 0;
}

void runServer(unsigned nPort)
{
	string message = "";
	
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
        /*stream.recvStr(message);
        cout << "\nMessage recu: "+message << endl;
        cout << "\nEnvoie au client..." << endl;*/
        //stream.send("this is server");
		cout << endl << stream.sendFile("../Debug/server.mp4");
		
		
	}
	catch (const SocketException& e)
	{
		cout << endl << e.what();
	}	
}
