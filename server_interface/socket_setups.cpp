/* Made by Liran Dagan 215609397 */
#include "socket_setups.h"

// step 1
bool initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

// step 2
bool createTCPsocket(SOCKET& listener)
{
	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return listener != INVALID_SOCKET;
}

// step 3.1
bool createAddress(sockaddr_in& serverAddr, const string& ip, const int portNumber)
{
	serverAddr.sin_family = AF_INET; // Server connection family, TCP or UDP
	serverAddr.sin_port = htons(portNumber);// "host to network short" function translates the allocated port number to the server
	// InetPton gives serverAddr the binary ip of 0.0.0.0 which binds all the local IP addresses to it
	return inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) == 1;
}

// step 3.2
bool assignAddress(SOCKET& listenSocket, sockaddr_in& serverAddr)
{
	return bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != SOCKET_ERROR;
}