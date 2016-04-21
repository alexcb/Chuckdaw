#pragma once

#define MAXNODES 1024

enum NodeStateEnum {
	follower,
	candidate,
	leader
};

// Information about other nodes in the cluster
struct Node {
	long long ID;
};

// Each node has it's own state
// this state is not shared between nodes
struct NodeState {
	enum NodeStateEnum state;
	long long ID;
	long long timeout;
	long long termNumber;

	struct Node *leader;

	// points to index of log items which have been acked by majority of cluster
	long matchIndex;

	// points to current log items which are awaiting ack by majority of cluster
	long nextIndex;

	unsigned int numNodes;
	struct Node nodes[MAXNODES];

	unsigned int numVotes;
	long long votees[MAXNODES];
};


void initNode(struct NodeState *node, long long ID);
void pumpNode(struct NodeState *node);
void addNode(struct NodeState *node, long long otherNodeID);
