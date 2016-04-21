#pragma once

enum MessageType {
	leaderRequest,
	leaderResponse,
	leaderHeartbeat
};


struct Message {
	long long nodeID;
	long long termNumber;
	enum MessageType type;

	// leaderResult fields
	int ok;

	// leader declaration
};

int readMessage(long long node, struct Message *message);
void sendMessage(long long node, struct Message message);
