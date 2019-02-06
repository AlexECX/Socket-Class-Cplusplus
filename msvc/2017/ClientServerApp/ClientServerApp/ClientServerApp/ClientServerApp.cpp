
#include "pch.h"
#include "SocketC++.h"
#include <iostream>
#include <string>
#include <thread>
#include <regex>

using namespace std;
using namespace cppsock;

#define PORT "8130"
#define HOST "127.0.0.1"

int main(int argc, char ** argv);

void testClient(const char* host, const char* nPort);
void testServer(const char* host, const char* nPort);
void sendFileClient(const char* host, const char* nPort, const string& filePath);
void sendFileServer(const char* host, const char* nPort);
void getFileClient(const char* host, const char* nPort, const string& filePath);
void getFileServer(const char* host, const char* nPort);

template <typename T>
void printLn(T t);

template<typename T, typename... Args>
void printLn(T t, Args... args);

void help();

int main(int argc, char **argv)
{
	string nPort;
	string host;
	string app;
	string task;
	string task_arg;
	string args = "";
	for (size_t i = 1; i < argc; i++)
	{
		args.append(argv[i]);
		args.append(" ");
	}
	args.pop_back();

	auto pattern = std::regex("(?:(server|client)(?: ((?:http://)?(?:www[0-9]{0,3}\\.)?(?:[A-z0-9][A-z0-9.-]*)))?(?: ([0-9]{1,5}))?(?: (\\-[A-z]{1,13}))?(?: (.*))?)");
	//auto pattern = std::regex("(server|client)");
	//auto pattern2 = std::regex("(?:(?:server|client)(?: (?:http://)?(?:www[0-9]{0,3}\\.)?(?:[A-z0-9][A-z0-9.\\-]*))?(?: [0-9]{1,5})?(?: (\\-[a-z]{1,10}))?(?: (.*))?)");
	std::smatch pattern_match;
	//
	// Check for port argument
	//
	if (!args.empty()) {
		if (args == "-v" || args == "-version")
		{
			printLn("version 0.1");
			return 0;
		}
		else if (std::regex_match(args, pattern_match, pattern))
		{
			app = pattern_match[1].str();

			if (pattern_match[5].length() > 0 && pattern_match[4].length() < 2) {
				help();
				return 0;
			}
			if (pattern_match[2].length() == 0)
			{
				if (app == "client") {
					host = HOST;
				}
				else
				{
					host = "";
				}
			}
			else
			{
				host = pattern_match[2].str();
			}
			if (pattern_match[3].length() == 0)
			{
				nPort = PORT;
			}
			else
			{
				nPort = pattern_match[3].str();
			}
			if (pattern_match[4].length() == 0)
			{
				task = "-test";
				task_arg = "";
			}
			else
			{
				task = pattern_match[4].str();
				task_arg = pattern_match[5].str();
			}
		}
		else
		{
			help();
			return 0;

		}
	}
	else
	{
		help();
		return 0;
	}
	/*printLn(app);
	printLn(host);
	printLn(nPort);
	printLn(task);
	printLn(task_arg);
	return 0;*/

	try
	{
		WSA_Utils::initWSA();

		if (task.compare("-test") == 0)
		{
			if (app.compare("client") == 0)
			{
				testClient(host.c_str(), nPort.c_str());
			}
			else
			{
				testServer(host.c_str(), nPort.c_str());
			}
		}
		else if (task.compare("-sendFile") == 0)
		{
			if (app.compare("client") == 0)
			{
				sendFileClient(host.c_str(), nPort.c_str(), task_arg);
			}
			else
			{
				sendFileServer(host.c_str(), nPort.c_str());
			}
		}
		else if (task.compare("-getFile") == 0)
		{
			if (app.compare("client") == 0)
			{
				getFileClient(host.c_str(), nPort.c_str(), task_arg);
			}
			else
			{
				getFileServer(host.c_str(), nPort.c_str());
			}
		}
		else
		{
			help();
		}

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

void testClient(const char * host, const char * nPort)
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
		cout << "\nMessage recu: " + packet << endl;

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

void testServer(const char * host, const char * nPort)
{
	try
	{
		ServerSocket server = ServerSocket(nPort);
		if (!server.valid_socket())
			throw SocketException(server.getErrString(), TRACEBACK);

		//Test send(int) et recv(int)//
		printLn("Test send(int) et recv(int)");
		Socket client = server.accept();
		if (!client.valid_socket()) {
			throw SocketException(client.getErrString(), TRACEBACK);
		}
		SocketIO stream = client.getStream();
		int integer = 0;
		printLn("Reception en cours...");
		stream.recv(integer);
		printLn("\nMessage recu: ", integer);
		printLn("\nEnvoie au client...");
		stream.send(integer);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR);
		}

		//Test sendStr() et recvStr()//
		printLn("Test sendStr() et recvStr()");
		client = server.accept();
		if (!client.valid_socket()) {
			throw SocketException(client.getErrString(), TRACEBACK);
		}
		stream = client.getStream();
		string message = "";
		printLn("Reception en cours...");
		stream.recvStr(message);
		printLn("\nMessage recu: ", message);
		printLn("\nEnvoie au client...");
		stream.sendStr("this is server");

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR);
		}

		//Test send(string, len) et recvStr(string, len)//
		printLn("Test send(string, len) et recvStr(string, len)");
		client = server.accept();
		if (!client.valid_socket()) {
			throw SocketException(client.getErrString(), TRACEBACK);
		}
		stream = client.getStream();
		message = "";
		integer = 0;
		printLn("Reception taiile message");
		stream.recv(message);
		printLn("\nMessage recu: ", message);
		printLn("\nEnvoie au client...");
		stream.send("this is server", 14);

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

void sendFileClient(const char * host, const char * nPort, const string & filePath)
{
	try
	{
		Socket client = Socket();
		if (!client.valid_socket())
			throw SocketException(client.getErrString(), TRACEBACK);

		while (true) {
			printLn("connection a ", host, " sur port ", nPort);
			if (client.connect(host, nPort) < 0) {
				printLn(client.getErrString());
				this_thread::sleep_for(2s);
			}
			else {
				break;
			}
		}
		printLn("Connecte.");
		SocketIO stream = client.getStream();
		printLn("Sending file path: ", filePath);
		stream.sendStr(filePath);
		printLn("Envoie en cours...");
		printLn(stream.sendFile(filePath));
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

void sendFileServer(const char * host, const char * nPort)
{
	ServerSocket server = ServerSocket(nPort);
	if (!server.valid_socket())
		throw SocketException(server.getErrString(), TRACEBACK);
	while (true)
	{
		try {
			printLn("Attente d'une connection");
			Socket client = server.accept();
			if (!client.valid_socket()) {
				throw SocketException(client.getErrString(), TRACEBACK);
			}
			printLn("Connected");
			SocketIO stream = client.getStream();
			string filePath = "";
			printLn("Receiving file path");
			stream.recvStr(filePath);
			printLn("File path: ", filePath);
			printLn("Envoie au client...");
			printLn(stream.sendFile(filePath));
			if (stream.getStatus() < 1)
			{
				printLn(stream.getStatus(), " ", WSA_ERROR);
			}
		}
		catch (const SocketException& e) {
			printLn(e.what());
		}
	}

}

void getFileClient(const char * host, const char * nPort, const string& filePath)
{
	try
	{
		Socket client = Socket();
		if (!client.valid_socket())
			throw SocketException(client.getErrString(), TRACEBACK);

		while (true) {
			printLn("connection a ", host, " sur port ", nPort);
			if (client.connect(host, nPort) < 0) {
				printLn(client.getErrString());
				this_thread::sleep_for(2s);
			}
			else {
				break;
			}
		}
		printLn("Connecte.");
		SocketIO stream = client.getStream();
		printLn("Sending file path: ", filePath);
		stream.sendStr(filePath);
		printLn("Reception en cours...");
		printLn(stream.recvFile(filePath));
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

void getFileServer(const char * host, const char * nPort)
{
	ServerSocket server = ServerSocket(nPort);
	if (!server.valid_socket())
		throw SocketException(server.getErrString(), TRACEBACK);
	while (true)
	{
		try {
			printLn("Attente d'une connection");
			Socket client = server.accept();
			if (!client.valid_socket()) {
				throw SocketException(client.getErrString(), TRACEBACK);
			}
			printLn("Connected");
			SocketIO stream = client.getStream();
			printLn("Reception file path");
			string filePath = "";
			stream.recvStr(filePath);
			printLn("File path: ", filePath);
			printLn("Reception en cours...");
			printLn(stream.recvFile(filePath));
			if (stream.getStatus() < 1)
			{
				printLn(stream.getStatus(), " ", WSA_ERROR);
			}
		}
		catch (const SocketException& e) {
			printLn(e.what());
		}
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

void help() {
	printLn("Syntax: [server | client] Host Port (Task)");
	printLn("Tasks:");
	printLn("-sendFile <insert file path>");
	printLn("-test");
}