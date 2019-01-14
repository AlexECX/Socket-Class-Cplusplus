#include "Traceback.h"
#include <string>

using namespace std;

Traceback::Traceback()
{
}

Traceback::~Traceback()
{
}

std::string Traceback::toString()
{
	return "Traceback";
}


void Traceback::buildTraceBack(const string & msg, const char* file, int line, string f)
{
	stream_Msg = "Traceback:";
	stream_Msg += "\n  File \"" + string(file) + "\", line " + to_string(line) + " in " + f;
	stream_Msg += "\n" + this->toString() + ": " + msg;
}
