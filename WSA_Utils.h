#pragma once
#include <string>

#define WSA_ERROR WSA_Utils::formatWSAError(WSAGetLastError())

namespace WSA_Utils {
	void initWSA();
	void cleanWSA();
	std::string LastError();
	std::string formatWSAError(int error_code);
}
