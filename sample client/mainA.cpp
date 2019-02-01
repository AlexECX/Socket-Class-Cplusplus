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
			throw SocketException(client.getSocketErr(), TRACEBACK);

		while (true) {
			cout << "\nconnection a " << host << " sur port " << nPort;
			if (client.connect(host, nPort) < 0) {
				cout << endl << client.getSocketErr();
				this_thread::sleep_for(2s); 
			}
			else {
				break;
			}
		}
		cout << "\nConnecte." << endl;
        cout << "\nEnvoie message" << endl;
		SocketIO stream = client.getStream();
		const char* str = "this is client";
		/*stream.sendStr("this is client");
        std::string packet;
        cout << "\nReception en cours..." << endl;
        stream.recvStr(packet);
        cout << "\nMessage recu: "+packet << endl;*/
		/*char t[4096];
		unsigned i = 0;
		int nRet;
		(nRet = stream.recv(t, 0, 4096));
		while ((nRet = stream.recv(t, 0, 4096)) > 0)
		{
			i += nRet;
		}
		cout << endl << i;
		cout << endl << nRet;*/
		cout << endl << stream.recvFile("./client.mp4");
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