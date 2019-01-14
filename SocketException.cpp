#include "SocketException.h"
#include <string>


using namespace std;


SocketException::SocketException(const string & msg) :
	std::runtime_error(msg) {
	stream_Msg = "Traceback: \n";
	stream_Msg += msg + ": SocketException";
}

SocketException::SocketException(const string& msg, const char *file, int line, string f) :
	std::runtime_error(msg) {
	stream_Msg = "Traceback:";
	stream_Msg += "\n  File \"" + string(file) + "\", line " + to_string(line) + " in " + f;
	stream_Msg += "\nSocketException: " + msg;

}

char const * SocketException::what() const throw()
{
	return stream_Msg.c_str();
}

SocketException::~SocketException()
{
}