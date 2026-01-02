/* Made by Liran Dagan 215609397 */
#include "interact_with_server.h"

/* 'mode' gets the values:
0: Enter client's name to be stored at server
1: Search for a specified receiver side by his name
2: Deliver messages to the receiver (By delivering them to the server to handle): (Client (we're here) -> Server -> Receiver)
*/
int mode = 0;
/* Connection to server. When false (by typing 'quit'), the program ends
(if connected == false then threads sendMessage, receiveMessage finish their run) */
bool connected = true;
/* in mode 1 we send a target name to the server who then replies with feedback(success / error) : This flag is the stores the answer */
bool foundTarget = false;


bool equalStrings(const string& str1, const string& str2)
{
	int len = str1.length();
	if (len != str2.length())
		return false;
	for (int i = 0; i < len; i++)
	{
		if (toupper(str1[i]) != toupper(str2[i]))
			return false;
	}
	return true;
}

bool registerClient(SOCKET& server)
{
	string name;
	cout << "Welcome to the chat! What is your name?" << endl;
	do {
		getline(cin, name);
	} while (name.empty() || equalStrings(name, "QUIT"));
	cout << "Hello " + name + "! Have fun chatting." << endl << "You can quit any time by typing 'QUIT'." << endl << "-----------------------------------------" << endl;
	if (send(server, name.c_str(), name.length(), 0) == SOCKET_ERROR) {
		cout << "Error sending name" << endl;
		return false;
	}
	return true;
}

bool searchTarget(SOCKET& server)
{
	string targetName;
	cout << "Who do you want to message?" << endl;
	do {
		getline(cin, targetName);
	} while (targetName.empty() || equalStrings(targetName, "QUIT"));
	if (send(server, targetName.c_str(), targetName.length(), 0) == SOCKET_ERROR)
	{
		cout << "Error sending name request" << endl;
		return false;
	}
	Sleep(500);
	return true;
}

void sendMessage(SOCKET server)
{
	string input;
	while (connected)
	{
		switch (mode)
		{
		case 0: // Send name to server
			if (registerClient(server))
				mode = 1;
			else
				connected = false;
			break;
		case 1: // We contact a desired target by name and wait for an approval from the server (see function 'receiveMessage' shortly)
			if (!foundTarget)
			{
				if (!searchTarget(server))
					connected = false;
			}
			break;
		case 2: // Deliver messages to server, it then will handle the forwarding to our desired target
			do {
				getline(cin, input);
			} while (input.empty());
			cout << endl;
			if (equalStrings(input, "BACK")) // special case: 'Back' to mode 1 if we wish to swap between desired targets. Without this - We are limited to access one client only in the program. (Server is not needed here)
			{
				mode = 1;
				foundTarget = false;
			}
			else if (equalStrings(input, "QUIT")) // special case: 'Quit' if we want to quit the program 
			{
				connected = false;
				//break;
			}
			if (send(server, input.c_str(), input.length(), 0) == SOCKET_ERROR)
				cout << "Error sending message" << endl;
			break;
		}
	}
}

void receiveMessage(SOCKET server)
{
	char buffer[4096];
	int bytesRecv;
	string message; // Server's message
	while (connected) {
		bytesRecv = recv(server, buffer, sizeof(buffer), 0);
		if (bytesRecv <= 0) // An unexpected disconnection / server error 
		{
			cout << "Disconnected from server" << endl;
			connected = false;
			break;
		}
		message = string(buffer, bytesRecv);
		if (message == "S:SameUser") // If the user chose himself as a target, We return to mode 1 in 'SendMessage'
		{
			cout << "Please Message a Different User." << endl << "-----------------------------------------" << endl;
			foundTarget = false;
			mode = 1;
		}
		else if (message == "S:UserNotFound") // If the user chose a non existing target (a name that is not in the chat), We return to mode 1 in 'SendMessage'
		{
			cout << "User Was Not Found." << endl << "-----------------------------------------" << endl;
			foundTarget = false;
			mode = 1;
		}
		else if (message == "S:UserFound") // If we successfully found our target, we procced to mode 2 in 'SendMessage'
		{
			cout << "Connection created" << endl << "-----------------------------------------" << endl;
			foundTarget = true;
			mode = 2;
		}
		else if (message == "S:quit") // If we quit the program then stop looping
			connected = false;
		// else this is not a server reply but an incoming message by a different client! In fact 'sendMessage' was applied on us by a different host
		// and we are the receiver "target" side (client (we're here) <- server <- other client)
		// The server tags other clients' messages with a prefix "C:", so we print from index 2
		else
			cout << message.c_str() + 2 << endl << endl;
	}
}