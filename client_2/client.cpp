/* Made by Liran Dagan 215609397 */
#include "client.h"

int main()
{
	// step 1: Start winsock
	if (!initialize()) {
		cout << "Failed to start the application." << WSAGetLastError() << endl;
		return -1;
	}

	// step 2: Create TCP type server socket
	SOCKET serverSocket;
	if (!createTCPsocket(serverSocket))
	{
		cout << "Failed to create socket " << WSAGetLastError() << endl;
		return -1;
	}

	// step 3.1: Assign the server address details we wish to connect to
	sockaddr_in serverAddr;
	//const string ip = "127.0.0.1"; // UNCOMMENT FOR LOOPBACK
	const string ip = "192.168.1.15"; // local ip address
	const int port = 12345; // Our assigned port number
	if (!createAddress(serverAddr, ip, port))
	{
		cout << "Setting address failed " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	// step 3.2: Connect client to the server socket
	if (!connectToServer(serverSocket, serverAddr))
	{
		cout << "Connection failed to establish: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}
	// Connection is created, we're ready to communicate with the server
	cout << "Connection to server established" << endl;

	// We split the application's attention to both send messages to the server and receive messages from the server, simultaneously
	thread senderthread(sendMessage, serverSocket);
	thread receiverthread(receiveMessage, serverSocket);
	senderthread.join();
	receiverthread.join();

	// When both threads finish (by typing 'Quit' or by server connection error)
	// in other words when the connection to the server stops, the program ends.
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}