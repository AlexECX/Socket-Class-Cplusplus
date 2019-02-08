//#pragma comment(lib, "gtestd.lib")
//#pragma comment(lib, "gtest_maind.lib")
#pragma comment(lib, "gmock.lib")
#pragma comment(lib, "gmock_main.lib")
#pragma comment(lib, "CppSocketLib.lib")

//#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "hippomocks.h"
#include "SocketC++.h"
#include <string>
#include <iostream>

#include "CppSocketMocks.h"
#include "winsock_mocks.h"

using namespace cppsock;

TEST(temp, temp) {
	std::cout << std::endl << sizeof sockaddr_in;
	std::cout << std::endl << sizeof sockaddr_in6;
}

TEST(SocketInstanciate, Socket_constructs) {
	MockRepository mocks;
	int mocksocket = 100;

	mocks.ExpectCallFunc(socket).Return(mocksocket);
	{
		MockSocket sock;
		EXPECT_TRUE(sock.valid_socket());
		ASSERT_EQ(mocksocket, sock.getMySocket());
	}

	mocks.ExpectCallFunc(socket).Return(++mocksocket);
	{
		MockSocket sock = MockSocket();
		EXPECT_TRUE(sock.valid_socket());
		ASSERT_EQ(mocksocket, sock.getMySocket());
	}

	mocks.ExpectCallFunc(socket).Return(++mocksocket);
	{
		MockSocket sock;
		EXPECT_TRUE(sock.valid_socket());
		ASSERT_EQ(mocksocket, sock.getMySocket());

		mocks.ExpectCallFunc(socket).Return(++mocksocket);
		sock = MockSocket();
		EXPECT_TRUE(sock.valid_socket());
		ASSERT_EQ(mocksocket, sock.getMySocket());
		
	}

	mocks.ExpectCallFunc(socket).Return(++mocksocket);
	{
		MockSocket sock = MockSocket();
		MockSocket sock2 = sock;
		EXPECT_TRUE(sock.valid_socket() && sock2.valid_socket());
		ASSERT_EQ(mocksocket, sock.getMySocket());
		ASSERT_EQ(sock.getMySocket(), sock2.getMySocket());
	}


	mocks.ExpectCallFunc(socket).Return(++mocksocket);
	mocks.ExpectCallFunc(socket).Return(++mocksocket);
	{
		MockSocket sock = MockSocket();
		MockSocket sock2 = MockSocket();
		EXPECT_TRUE(sock.valid_socket() && sock2.valid_socket());
		ASSERT_NE(sock.getMySocket(), sock2.getMySocket());
	}

	mocks.ExpectCallFunc(socket).Return(++mocksocket);
	mocks.ExpectCallFunc(socket).Return(++mocksocket);
	{
		MockSocket sock = MockSocket();
		MockSocket sock2 = MockSocket();
		sock = sock2;
		EXPECT_TRUE(sock.valid_socket() && sock2.valid_socket());
		ASSERT_EQ(sock.getMySocket(), sock2.getMySocket());
	}

	/*mocks.ExpectCallFunc(accept).Return(mocksocket);
	{
		MockServerSocket ssock;
		MockSocket sock = ssock.accept();
		EXPECT_TRUE(sock.valid_socket());
		EXPECT_EQ(mocksocket++, sock.getMySocket());
	}*/
	
}


TEST(SocketAutoclose, Instanciate_with_copy_assignement) {
	MockRepository mocks;

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		{
			MockSocket sockcopy = sock;
			EXPECT_EQ(sockcopy.closeCount, 0);
			EXPECT_FALSE(sockcopy.getAutoClose());
			EXPECT_TRUE(sock.getAutoClose());
		}
	}
	
	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			sock = MockSocket();
			EXPECT_EQ(sock.closeCount, 1);
			EXPECT_TRUE(sock.getAutoClose());
		}
	}
}


TEST(SocketAutoclose, Instanciate_with_move_assignement) {
	MockRepository mocks;

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		{
			MockSocket sockcopy = std::move(sock);
			EXPECT_EQ(sockcopy.closeCount, 0);
			EXPECT_TRUE(sockcopy.getAutoClose());
			EXPECT_FALSE(sock.getAutoClose());
		}
	}

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			sock = std::move(MockSocket());
			EXPECT_EQ(sock.closeCount, 1);
			EXPECT_TRUE(sock.getAutoClose());
		}
	}
}


TEST(SocketAutoclose, Assignement_with_copy_assignement) {
	MockRepository mocks;

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			MockSocket sockcopy;
			sockcopy = sock;
			EXPECT_EQ(sockcopy.closeCount, 1);
			EXPECT_TRUE(sockcopy.getAutoClose());
			EXPECT_FALSE(sock.getAutoClose());
		}
	}

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			MockSocket sockcopy;
			sock = sockcopy;
			EXPECT_EQ(sock.closeCount, 1);
			EXPECT_FALSE(sockcopy.getAutoClose());
			EXPECT_TRUE(sock.getAutoClose());
		}
	}

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			sock = MockSocket();
			EXPECT_EQ(sock.closeCount, 1);
			EXPECT_TRUE(sock.getAutoClose());
		}
	}
	
}


TEST(SocketAutoclose, Assignement_with_move_assignement) {
	MockRepository mocks;

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			MockSocket sockcopy;
			sockcopy = std::move(sock);
			EXPECT_EQ(sockcopy.closeCount, 1);
			EXPECT_TRUE(sockcopy.getAutoClose());
			EXPECT_FALSE(sock.getAutoClose());
		}
	}

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			MockSocket sockcopy;
			sock = std::move(sockcopy);
			EXPECT_EQ(sock.closeCount, 1);
			EXPECT_FALSE(sockcopy.getAutoClose());
			EXPECT_TRUE(sock.getAutoClose());
		}
	}

	{
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		MockSocket sock = MockSocket();
		mocks.ExpectCallFunc(socket).Do(winsockmock::socket);
		{
			sock = std::move(MockSocket());
			EXPECT_EQ(sock.closeCount, 1);
			EXPECT_TRUE(sock.getAutoClose());
		}
	}

}


TEST(SocketInOutput, send_success) {
	MockRepository mocks;
	MockSocketIO stream = MockSocketIO(100);
	mocks.OnCallFunc(send).Do(winsockmock::send);

	for (size_t i = 0; i < 5; i++)
	{
		mocks.ExpectCallFunc(send).Do(winsockmock::send);
	}
	
	EXPECT_EQ(12, stream.send("this is test", 12));
	EXPECT_EQ(sizeof(int), stream.send(1));
	EXPECT_EQ(12, stream.send("this is test", 0, 12));
	EXPECT_EQ(12, stream.send("this is test"));
	EXPECT_EQ(12 + 1, stream.sendStr("this is test"));

	size_t bigsize = INT_MAX;
	bigsize += 1000;
	char bigbuffer[10];
	memset(bigbuffer, '1', 10);
	mocks.OnCallFunc(send).Do(winsockmock::send);
	EXPECT_EQ(bigsize, stream.sendLarge(bigbuffer, 0, bigsize));

}


TEST(SocketInOutput, send_failure) {
	MockRepository mocks;
	MockSocketIO stream = MockSocketIO(100);
	mocks.OnCallFunc(send).Do(winsockmock::send);

	for (size_t i = 0; i < 6; i++)
	{
		mocks.ExpectCallFunc(send).Do(winsockmock::send).Return(-1);
	}

	ASSERT_EQ(-1, stream.send("this is test", 12));
	EXPECT_EQ(-1, stream.send(1));
	EXPECT_EQ(0, stream.send("this is test", 0, 12));
	EXPECT_TRUE(stream.getStatus() < 0);
	EXPECT_EQ(0, stream.send("this is test"));
	EXPECT_TRUE(stream.getStatus() < 0);
	EXPECT_EQ(0, stream.sendStr("this is test"));
	EXPECT_TRUE(stream.getStatus() < 0);
	size_t bigsize = INT_MAX;
	bigsize += 1000;
	char bigbuffer[10];
	memset(bigbuffer, '1', 10);
	EXPECT_EQ(0, stream.sendLarge(bigbuffer, 0, bigsize));
	EXPECT_TRUE(stream.getStatus() < 0);

}


TEST(SocketInOutput, recv_success) {
	MockRepository mocks;
	MockSocketIO stream = MockSocketIO(100);

	char buffer[10];
	int intbuffer = 0;
	std::string strbufer = "";
	mocks.OnCallFunc(recv).Do(winsockmock::recv);

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(12);
	EXPECT_EQ(12, stream.recv(buffer, 12));

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(sizeof(int));
	EXPECT_EQ(sizeof(int), stream.recv(intbuffer));

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(12);
	EXPECT_EQ(12, stream.recv(strbufer, 12));

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(12);
	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(0);
	EXPECT_EQ(12, stream.recv(strbufer));
	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(13);
	char* objbuff = stream.getIObuffer();
	objbuff[12] = '\0';
	EXPECT_EQ(12 + 1, stream.recvStr(strbufer));

	size_t bigsize = INT_MAX;
	bigsize += 1000;
	char bigbuffer[10];
	memset(bigbuffer, '1', 10);
	/*MockSocketIO* stream_mock = mocks.Mock<MockSocketIO>();
	
	mocks.ExpectCallOverload(
		stream_mock,
		(int (MockSocketIO::*)(char* msg, int len))&MockSocketIO::recv
	).Return(INT_MAX);
	mocks.ExpectCallOverload(
		stream_mock,
		(int (MockSocketIO::*)(char* msg, int len))&MockSocketIO::recv
	).Return(1000);
	mocks.OnCall(stream_mock, MockSocketIO::recvLarge);*/
	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(INT_MAX);
	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(1000);
	EXPECT_EQ(bigsize, stream.recvLarge(bigbuffer, 0, bigsize));

}


TEST(SocketInOutput, recv_failure) {
	MockRepository mocks;
	MockSocketIO stream = MockSocketIO(100);

	char buffer[10];
	int intbuffer = 0;
	std::string strbufer = "";
	mocks.OnCallFunc(recv).Do(winsockmock::recv);

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(-1);
	EXPECT_EQ(-1, stream.recv(buffer, 12));

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(-1);
	EXPECT_EQ(-1, stream.recv(intbuffer));

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(-1);
	EXPECT_EQ(0, stream.recv(strbufer, 12));
	EXPECT_TRUE(stream.getStatus() < 0);

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(-1);
	EXPECT_EQ(0, stream.recv(strbufer));
	EXPECT_TRUE(stream.getStatus() < 0);

	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(-1);
	char* objbuff = stream.getIObuffer();
	objbuff[12] = '\0';
	EXPECT_EQ(0, stream.recvStr(strbufer));
	EXPECT_TRUE(stream.getStatus() < 0);

	size_t bigsize = INT_MAX;
	bigsize += 1000;
	char bigbuffer[10];
	memset(bigbuffer, '1', 10);
	/*MockSocketIO* stream_mock = mocks.Mock<MockSocketIO>();

	mocks.ExpectCallOverload(
		stream_mock,
		(int (MockSocketIO::*)(char* msg, int len))&MockSocketIO::recv
	).Return(INT_MAX);
	mocks.ExpectCallOverload(
		stream_mock,
		(int (MockSocketIO::*)(char* msg, int len))&MockSocketIO::recv
	).Return(1000);
	mocks.OnCall(stream_mock, MockSocketIO::recvLarge);*/
	mocks.ExpectCallFunc(recv).Do(winsockmock::recv).Return(-1);
	EXPECT_EQ(0, stream.recvLarge(bigbuffer, 0, bigsize));
	EXPECT_TRUE(stream.getStatus() < 0);

}


TEST(SocketInOutput, int_overflow) {
	MockRepository mocks;
	mocks.ExpectCallFunc(socket).Return(100);
	MockSocket sock;
	SocketIO stream = sock.getStream();

	size_t bigsize = INT_MAX;
	bigsize += 1;
	char bigbuffer[10];

	mocks.NeverCallFunc(send).Do(winsockmock::send);
	ASSERT_NE(bigsize, stream.send(bigbuffer, bigsize));

	mocks.NeverCallFunc(recv).Do(winsockmock::recv);
	ASSERT_NE(bigsize, stream.recv(bigbuffer, bigsize));
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
