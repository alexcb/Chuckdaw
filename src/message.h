#pragma once
#include "consts.h"
#include <uuid/uuid.h>

enum MessageType {
	joinRequest,
	clusterMembershipUpdate,
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

	// clusterMembershipUpdate
	uuid_t nodes[maxNodes];
	int numNodes;
};

int readMessage(uuid_t node, struct Message *message);
void sendMessage(uuid_t node, struct Message message);
int numMessages();
