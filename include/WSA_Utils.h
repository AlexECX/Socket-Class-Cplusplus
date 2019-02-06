#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define WSA_ERROR WSA_Utils::formatWSAError(WSAGetLastError())

namespace WSA_Utils {
	void initWSA();
	void cleanWSA();
	std::string LastError();
	std::string formatWSAError(int error_code);
}
