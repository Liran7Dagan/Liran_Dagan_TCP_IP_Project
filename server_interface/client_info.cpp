/* Made by Liran Dagan 215609397 */
#include "client_info.h"

bool ClientInfo::operator==(const ClientInfo& b) {
	return name == b.name && socket == b.socket;
}

ClientInfo* findByName(const string& name, list<ClientInfo>& clients) {
	for (auto& c : clients)
	{
		if (c.name == name)
			return &c;
	}
	return NULL;
}

