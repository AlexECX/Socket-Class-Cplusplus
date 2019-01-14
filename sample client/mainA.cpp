#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include "../SocketException.h"
#include "../Socket.h"

using namespace std;

#define HARD_CODED
#define HOST "127.0.0.1" //Can be Name or IP address
#define PORT 2030

void runClient(unsigned nPort, const string& host);

int main(int argc, char **argv)
{
	unsigned nPort;
	string host;

	//
	// Check for the host and port arguments
	//
#ifdef HARD_CODED
	nPort = PORT;
	host = HOST;
#else
	if (argc != 3) {
		cout << "\nSyntax: ServerName PortNumber";
		cout << endl;
		return 0;
	}
	nPort = atoi(argv[2]);
	host = argv[1];

#endif // HARD_CODED

	WSA_Utils::initWSA();

	runClient(nPort, host);

	// Release WinSock
	//
	WSACleanup();
	return 0;

}

void runClient(unsigned nPort, const string& host)
{

	try
	{
		Socket client = Socket();
		if (!client.valid_socket())
			throw SocketException(client.getSocketErr(), TRACEBACK);

		while (true) {
			cout << "\nconnection a " + string(host) + " sur port " << nPort;
			if (client.connect(host, nPort) < 0) {
				this_thread::sleep_for(2s); 
			}
			else {
				break;
			}
		}
		cout << "\nConnecte." << endl;
        cout << "\nEnvoie message" << endl;
		client.sendStr_Ex("this is client");
        std::string packet;
        cout << "\nReception en cours..." << endl;
        client.recvStr_Ex(packet);
        cout << "\nMessage recu: "+packet << endl;

	}
	catch (const SocketException& e)
	{
		cout << endl << e.what();
	}
	

}