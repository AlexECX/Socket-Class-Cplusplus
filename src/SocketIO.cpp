#include "SocketC++.h"
#include <fstream>
#include <iostream>
#include <istream>

using namespace std;
using namespace cppsock;

//SocketIO///////////////////////////////////////////////////////

SocketIO::SocketIO(const SOCKET &socket){
    //mySocket_ptr = socket_ptr;
	mySocket = socket;
}

SocketIO::~SocketIO()
{
}

int SocketIO::send(const char * msg, int len)
{
	int sent = 0;
	while (sent < len && (nRet = ::send(mySocket, msg + sent, len - sent, flags)) > 0)
	{
		sent += nRet;
	}
	return (nRet < 0) ? nRet : sent;
}

int SocketIO::recv(char * msg, int offset, int len)
{
	return ::recv(mySocket, msg, len, flags);
}

int SocketIO::recv(char * msg, int len)
{
	int recved = 0;
	while (recved < len && (nRet = recv(msg, recved, len - recved)) > 0) {
		recved += nRet;
	}
	return (nRet < 0) ? nRet : recved;
}

size_t SocketIO::send(const char * str)
{
	return send(static_cast<string>(str));
}

size_t SocketIO::send(const string &str, size_t offset, size_t len)
{
	return sendLarge(str.c_str(), offset, len);
}

size_t SocketIO::send(const string &str)
{
	return send(str, 0, str.size());
}

size_t SocketIO::recv(std::string & str, size_t len)
{
	size_t recved = 0;
	str.clear();

	if (len > this->bufflen)
	{
		auto buff = std::make_unique<char[]>(len);
		return recvLarge(buff.get(), 0, len);
	}
	else 
	{
		while (recved < len && (nRet = recv(this->iobuff, 0, this->bufflen)) > 0) {
			str.append(iobuff, nRet);
			recved += nRet;
		}
		return recved;
	}
}

size_t SocketIO::recv(string& str)
{
	size_t recved = 0;
	str.clear();

	while ((nRet = recv(this->iobuff, 0, this->bufflen)) > 0) {
		str.append(iobuff, nRet);
		recved += nRet;
	}
	return recved;
}

size_t SocketIO::sendStr(const std::string & str)
{
	return send(str, 0, str.size() + 1);
}

size_t SocketIO::recvStr(std::string & str)
{
	unsigned recved = 0;
	str.clear();

	while ((nRet = recv(this->iobuff, recved, this->bufflen)) > 0) {
		recved += nRet;
		if (this->iobuff[nRet - 1] == '\0')
		{
			str.append(this->iobuff, nRet - 1);
			break;
		}
		else
		{
			str.append(this->iobuff, nRet);
		}
	}

	return recved;
}

size_t SocketIO::sendLarge(const char * msg, size_t offset, size_t len)
{
	size_t sent = 0;
	while (INT_MAX < (len - sent))
	{
		if ((nRet = send(msg + sent, INT_MAX)) > 0)
		{
			sent += nRet;
		}
		else
		{
			return sent;
		}
		
	}
	while (sent < len && (nRet = send(msg + sent, int(len - sent))) > 0)
	{
		sent += nRet;
	}

	return sent;
}

size_t SocketIO::recvLarge(char * msg, size_t offset, size_t len)
{
	size_t recved = 0;
	while (INT_MAX < (len - recved))
	{
		if ((nRet = recv(msg + recved, INT_MAX)) > 0)
		{
			recved += nRet;
		}
		else
		{
			return recved;
		}
	}
	while (recved < len && (nRet = recv(msg + recved, int(len - recved))) > 0)
	{
		recved += nRet;
	}

	return recved;
}

size_t SocketIO::sendFile(const string &filePath)
{
	istream::pos_type file_size = 0;

	std::ifstream file_source(filePath.c_str(), std::ios::binary);
	if (file_source.fail() || file_source.tellg() < 0)
	{
		//Send 0 to client, to tell no file was found
		file_size = -1;
		send(file_size);
		throw SocketException("FileNotFound", TRACEBACK);
	}
	//Get the requested file

	//Get and send file size
	file_source.seekg(0, std::ios::end);
	file_size = file_source.tellg();

	if ((nRet = send(file_size)) <= 0) {
		return nRet;
	}

	//Send file in multiple packets
	size_t sent = 0;
	unsigned chunks = static_cast<unsigned>(file_size / 5000000);
	unsigned res = file_size % 5000000;
	
	std::unique_ptr<char[]> file_buffer;
	if (chunks == 0)
	{
		file_buffer = std::make_unique<char[]>(res);
	}
	else
	{
		file_buffer = std::make_unique<char[]>(5000000);
	}

	/** Tool for progress bar 1/2*/
	size_t progression = 0;
	size_t progresStep = file_size / 16;
	if (file_size % 16 > 0)
	{
		progresStep++;
	}
	cout << "Progression '*'x16 [ ";
	/**/

	size_t i = 0;
	do
	{
		file_source.seekg(sent, std::ios::beg);
		if (i >= chunks && res > 0)
		{
			file_source.read(file_buffer.get(), res);
			nRet = send(file_buffer.get(), res);
		}
		else
		{
			file_source.read(file_buffer.get(), 5000000);
			nRet = send(file_buffer.get(), 5000000);
		}

		if (nRet > 0)
		{
			sent += nRet;
		}

		/** Tool for progress bar 2 / 2*/
		if (sent > progresStep * progression)
		{
			while (progresStep * progression < sent)
			{
				if (progression > 14)
					cout << "* ]" << endl;
				else
					cout << "* ";
				progression++;
			}
		}
		/**/

	} while (i++ < chunks && nRet > 0);

	file_source.close();

	return sent;
}

size_t SocketIO::recvFile(const string &filePath)
{
	istream::pos_type file_size = -1;

	if ((nRet = recv(file_size)) < 0) {
		return nRet;
	}

	if (file_size < 0)
	{
		throw SocketException("RemoteFileNotFound", TRACEBACK);
	}

	ofstream IMG_dest(filePath.c_str(), ios::binary);
	if (IMG_dest.fail())
	{
		throw SocketException("CannotOpenFile", TRACEBACK);
	}

	//Receive file in multiple pakcets
	size_t recved = 0;
	unsigned size = 0;

	if (file_size < 5000000)
	{
		size = static_cast<unsigned>(file_size);
	}
	else
	{
		size = 5000000;
	}

	auto file_buffer = std::make_unique<char[]>(size);
	IMG_dest.seekp(0, ios::beg);

	/** Tool for progress bar 1/2*/
	size_t progresStep = file_size / 16;
	if (file_size % 16 > 0)
	{
		progresStep++;
	}
	size_t progression = 0;
	cout << "Progression '*'x16 [ ";
	/**/

	while (
		(istream::pos_type)recved < file_size 
		&& 
		(nRet = recv(file_buffer.get(), size)) > 0
		) {
		recved += nRet;
		IMG_dest.write(file_buffer.get(), nRet);

		/** Tool for progress bar 2 / 2*/
		if (recved > progresStep * progression)
		{
			while (progresStep * progression < recved)
			{
				if (progression > 14)
					cout << "* ]" << endl;
				else
					cout << "* ";
				progression++;
			}
		}
		/**/

	}
	IMG_dest.close();

	return recved;
}