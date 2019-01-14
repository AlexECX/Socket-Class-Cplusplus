#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include "../SocketException.h"
#include "../ServerSocket.h"


using namespace std;

#define HARD_CODED
#define PORT 2030

void runServer(unsigned nPort);
void scriptedConvoTest(Socket clientA, Socket clientB);
string getFileName(const string& s);

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
	WSACleanup();
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
        client.recvStr_Ex(message);
        cout << "\nMessage recu: "+message << endl;
        cout << "\nEnvoie au client..." << endl;
        client.sendStr_Ex("this is server");
        
		
	}
	catch (const SocketException& e)
	{
		cout << endl << e.what();
	}	
}
