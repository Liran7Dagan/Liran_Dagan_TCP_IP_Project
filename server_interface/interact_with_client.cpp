/* Made by Liran Dagan 215609397 */
#include "interact_with_client.h"

using namespace std;

// return str1==str2 case isensitive ('a'=='A' for example)
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
bool isQuit(string& message, ClientInfo& client)
{
	if (equalStrings(message, "QUIT"))
	{
		const string quitProg = "S:quit";
		send(client.socket, quitProg.c_str(), quitProg.length(), 0);
		cout << "'" << client.name << "' Disconnected" << endl;
		return true;
	}
	return false;
}
ClientInfo createClient(string& name, SOCKET& socket, list<ClientInfo>& all_clients)
{
	ClientInfo client;
	client.name = name;
	client.socket = socket;
	all_clients.push_front(client);
	cout << "New client joined: '" << name << "'" << endl;
	return client;
}
bool isError(string& targetName, ClientInfo& client, list<ClientInfo>& all_clients)
{
	// Side Note: We give 'serverReply' the prefix "S:" and later we give messages from 
	// clients the prefix "C:". Why is that? Without these tags, clients could send fake
	// errors to each other pretending to be the server. We don't want that to happen
	// so we tag server replies and client messages with different prefixes.
	bool isErrorFound;
	string serverReply;
	if (targetName == client.name) // send 'SameUser' reply if the client chose himself
	{
		isErrorFound = true;
		serverReply = "S:SameUser";
		cout << client.name << " Tried to message himself" << endl;
	}
	else if (!findByName(targetName, all_clients)) // send 'UserNotFound' reply if the target is not in the list 
	{
		isErrorFound = true;
		serverReply = "S:UserNotFound";
		cout << client.name << " Tried to reach a non existent client: " << targetName << endl;
	}
	else // Else we send 'UserFound' reply, we found the target we were looking for in the list
	{
		isErrorFound = false;
		serverReply = "S:UserFound";
		cout << "'" << client.name << "' Reaches '" << targetName << "'. Server is ready to transmit messages!" << endl;
	}
	send(client.socket, serverReply.c_str(), serverReply.length(), 0);
	return isErrorFound;
}
void deleteClient(ClientInfo& client, list<ClientInfo>& all_clients)
{
	list<ClientInfo>::iterator removeLocation = find(all_clients.begin(), all_clients.end(), client);
	closesocket(client.socket);
	if (removeLocation != all_clients.end())
		all_clients.erase(removeLocation);
}
/* step 6: use 'recv','send' and additional logic to communicate with every client
*  summary:
   'Interact' Handles the server's actions with a single client and forwards it's messages to whoever it wishes
   If needed, the server sends feedback messages regarding errors.
   We save a list of all the active clients including the name and socket of each one */
void Interact(SOCKET clientSocket, list<ClientInfo>& all_clients)
{
	int mode = 0;
	/* mode gets the following values:
	0 = We don't know client, so we create and add them as a new client to the list
	1 = Figure out target that client is trying to reach
	2 = Deliver client's messages to target
	*/
	int bytesRecved;
	char buffer[4096];
	ClientInfo client;
	string targetName;
	string message; // Contain's the client input 
	string request_status; // Notify the client (by 'send') on his request for a target (feedback message)
	while (true) {
		// recv gets message (string) from client and loads it to buffer
		bytesRecved = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRecved <= 0) // If an unexpected error occurs 
		{
			cout << "'" << client.name << "' DISCONNECTED" << endl;
			break;
		}
		message = string(buffer, bytesRecved);
		if (isQuit(message, client)) // If client sent 'Exit' then quit the loop
			break;

		switch (mode) {
		case 0: // create the client struct {name , socket}, Add it to the list and procced to mode 1
			client = createClient(message, clientSocket, all_clients);
			mode = 1;
			break;

		case 1: // Figure out the target our client tries to reach
			// Send feedback on search error/success and procced to mode 2 (if target was found) 
			if (!isError(message, client, all_clients))
			{
				targetName = message;
				mode = 2;
			}
			break;

		case 2: // Message target client by forwarding ('send') client's message (client -> server (we're here) -> target)
			if (equalStrings(message, "BACK"))
			{
				mode = 1;
				break;
			}
			ClientInfo* sendTo = findByName(targetName, all_clients);
			if (!sendTo) {
				cout << client.name << " Tried to reach a non existent client: '" << targetName << "'" << endl;
				mode = 1;
			}
			else
			{
				// Why "C:" ? See isError function definition's note 
				string display = "C:" + client.name + ": " + message; // the message we send to the clients for printing. For example - 'Liran: Hello there'
				if (send(sendTo->socket, display.c_str(), display.length(), 0) == SOCKET_ERROR)
					cout << client.name << " Failed to send message to " << sendTo->name;
				else
					cout << client.name << " TO " << sendTo->name << ": " << message << endl;
			}
			break;
		}
	}
	// If client disconnected we close his socket and remove him from the active clients list	
	deleteClient(client, all_clients);
}