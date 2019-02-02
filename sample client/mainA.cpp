#include <iostream>
#include <string>
//#include <windows.h>
#include <thread>
#include "../SocketC++.h"

using namespace std;
using namespace cppsock;

#define HOST "127.0.0.1" //Can be Name or IP address
#define PORT "8130"

void runClient(const char* host, const char* nPort);

int main(int argc, char **argv)
{
	const char* nPort;
	const char* host;

	//
	// Check for the host and port arguments
	//
	if (argc == 1) {
		nPort = PORT;
		host = HOST;
	}
	else if (argc != 3)
	{
		cout << "\nSyntax: ServerName PortNumber";
		cout << endl;
		return 0;
	}
	else
	{
		nPort = argv[2];
		host = argv[1];
	}
	
	try
	{
		WSA_Utils::initWSA();

		runClient(host, nPort);

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

void runClient(const char* host, const char* nPort)
{

	try
	{
		Socket client = Socket();
		if (!client.valid_socket())
			throw SocketException(client.getErrString(), TRACEBACK);

		while (true) {
			cout << "\nconnection a " << host << " sur port " << nPort;
			if (client.connect(host, nPort) < 0) {
				cout << endl << client.getErrString();
				this_thread::sleep_for(2s); 
			}
			else {
				break;
			}
		}
		cout << "\nConnecte." << endl;
        cout << "\nEnvoie message" << endl;
		SocketIO stream = client.getStream();

		stream.sendStr("this is client");
        std::string packet;
        cout << "\nReception en cours..." << endl;
        stream.recvStr(packet);
        cout << "\nMessage recu: "+packet << endl;
		//cout << endl << stream.recvFile("./client.mp4");
		if (stream.getStatus() < 1)
		{
			cout << endl << stream.getStatus();
			cout << endl << WSA_ERROR;
		}

	}
	catch (const SocketException& e)
	{
		cout << endl << e.what();
	}
	

}