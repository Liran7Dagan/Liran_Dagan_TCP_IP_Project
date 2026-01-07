/* Made by Liran Dagan 215609397 */
#pragma once
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <list>

using namespace std;

// Each client has a name and a socket to which the server sends info to and receives info from
struct ClientInfo {
	string name;
	SOCKET socket;

	bool operator==(const ClientInfo&);
};
	
ClientInfo* findByName(const string&, list<ClientInfo>&);



