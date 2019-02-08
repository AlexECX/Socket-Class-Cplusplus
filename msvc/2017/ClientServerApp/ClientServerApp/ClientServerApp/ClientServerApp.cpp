
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
	catch (...) {
		auto expPtr = std::current_exception();

		try
		{
			if (expPtr) std::rethrow_exception(expPtr);
		}
		catch (const std::exception& e) //it would not work if you pass by value
		{
			printLn(e.what());
		}
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

		int integer = 0;
		string message = "";

		//Test send(int) et recv(int)//
		printLn("Test send(int) et recv(int)");
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
		printLn("Connecte");

		SocketIO stream = client.getStream();
		printLn("Envoie message");
		stream.send(integer);
		printLn("Reception en cours...");
		stream.recv(integer);
		printLn("Message recu: ", integer);
		this_thread::sleep_for(0.5s);
		client.shutdown(SD_BOTH);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}

		//Test sendStr() et recvStr()//
		printLn("Test sendStr() et recvStr()");
		client = Socket();
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
		printLn("Connecte");

		stream = client.getStream();
		printLn("Envoie message");
		stream.sendStr("this is client");
		printLn("Reception en cours...");
		stream.recvStr(message);
		printLn("Message recu: ", message);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}

		//Test send(string, len) et recv(string, len)//
		printLn("Test send(string, len) et recv(string, len)");
		client = Socket();
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
		printLn("Connecte");

		stream = client.getStream();
		printLn("Envoie message");
		stream.send("this is client", 14);
		printLn("Reception en cours...");
		stream.recv(message, 14);
		printLn("Message recu: ", message);
		this_thread::sleep_for(0.5s);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}

		//Test send(string) then disconnect//
		printLn("Test send(string, len) et recv(string, len)");
		client = Socket();
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
		printLn("Connecte");

		stream = client.getStream();
		printLn("Envoie message");
		stream.send(string("this is client"));
		this_thread::sleep_for(0.5s);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}


		printLn("Test multiple connect()");
		for (size_t i = 0; i < 100; i++)
		{
			client = Socket(host, nPort);
			this_thread::sleep_for(1ms);
		}

	}
	catch (const SocketException& e)
	{
		printLn(e.what());
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
		printLn("Connected");
		SocketIO stream = client.getStream();
		int integer = 0;
		printLn("Reception en cours...");
		stream.recv(integer);
		printLn("Message recu: ", integer);
		printLn("Envoie au client...");
		integer++;
		stream.send(integer);
		this_thread::sleep_for(0.5s);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}

		//Test sendStr() et recvStr()//
		printLn("Test sendStr() et recvStr()");
		client = server.accept();
		if (!client.valid_socket()) {
			throw SocketException(client.getErrString(), TRACEBACK);
		}
		printLn("Connected");
		stream = client.getStream();
		string message = "";
		printLn("Reception en cours...");
		stream.recvStr(message);
		printLn("Message recu: ", message);
		printLn("Envoie au client...");
		stream.sendStr("this is server");
		this_thread::sleep_for(0.5s);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}

		//Test send(string, len) et recv(string, len)//
		printLn("Test send(string, len) et recv(string, len)");
		client = server.accept();
		if (!client.valid_socket()) {
			throw SocketException(client.getErrString(), TRACEBACK);
		}
		printLn("Connected");
		stream = client.getStream();
		message = "";
		printLn("Reception en cours...");
		stream.recv(message, 14);
		printLn("Message recu: ", message);
		printLn("Envoie au client...");
		stream.send("this is server", 14);
		this_thread::sleep_for(0.5s);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}

		//Test sendStr(string) et recvStr(string)//
		printLn("Test recvStr(string) until disconnect");
		client = server.accept();
		if (!client.valid_socket()) {
			throw SocketException(client.getErrString(), TRACEBACK);
		}
		printLn("Connected");
		stream = client.getStream();
		message = "";
		printLn("Reception en cours...");
		stream.recv(message);
		printLn("Message recu: ", message);
		this_thread::sleep_for(0.5s);

		if (stream.getStatus() < 1)
		{
			printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
		}

		printLn("TEST multiple bind()");
		for (size_t i = 0; i < 100; i++)
		{
			server = ServerSocket(nPort);
			this_thread::sleep_for(1ms);
		}

		/*printLn("Test multiple accept()");
		while (true)
		{
			client = server.accept();
			this_thread::sleep_for(1ms);
		}*/

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
				printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
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
				printLn(stream.getStatus(), " ", WSA_ERROR, ": ", __LINE__);
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