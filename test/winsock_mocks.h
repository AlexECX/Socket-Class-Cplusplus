#pragma once

namespace winsockmock {

	SOCKET socket(int af, int type, int protcol) {
		static SOCKET sock = 0;
		if (sock >= INVALID_SOCKET)
		{
			sock = 0;
		}
		return sock++;
	}

	constexpr auto BUFFER_LIMIT = 1000000;

	int send(SOCKET s, const char* buff, int len, int flags) {
		int i = 0;
		if (i < 0)
		{
			i = 0;
		}
		else if (len > BUFFER_LIMIT)
		{
			i = BUFFER_LIMIT;
		}
		else
		{
			i = len;
		}
		return i;
	}

	int recv(SOCKET s, char* buff, int len, int flags) {
	int i = 0;
	if (i < 0)
	{
		i = 0;
	}
	else if (len > BUFFER_LIMIT)
	{
		i = BUFFER_LIMIT;
	}
	else
	{
		i = len;
	}
	return i;
}

}
