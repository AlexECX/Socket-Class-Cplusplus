#pragma once

class Traceback
{
private:
	std::string stream_Msg;

public:
	Traceback();
	Traceback(const std::string& msg);
	Traceback(const std::string& msg, const char *file,
						 int line, std::string f);
	~Traceback();

	virtual void buildTraceBack(const std::string& msg, const char *file,
		int line, std::string f);

	virtual std::string toString();
};

