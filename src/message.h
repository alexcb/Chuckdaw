#pragma once
#include <uuid/uuid.h>

enum MessageType {
	leaderRequest,
	leaderResponse,
	leaderHeartbeat
};


struct Message {
	uuid_t nodeID;
	long long termNumber;
	enum MessageType type;

	// leaderResult fields
	int ok;

	// leader declaration
};

int readMessage(uuid_t node, struct Message *message);
void sendMessage(uuid_t node, struct Message message);
