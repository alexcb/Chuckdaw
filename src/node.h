#pragma once
#include <uuid/uuid.h>

#define MAXNODES 1024

enum NodeStateEnum {
	bootstrap,
	follower,
	candidate,
	leader
};

// Information about other nodes in the cluster
struct Node {
	uuid_t ID;
};

// Each node has it's own state
// this state is not shared between nodes
struct NodeState {
	enum NodeStateEnum state;
	long long timeout;
	long long termNumber;

	struct Node *self;
	struct Node *leader;

	// points to index of log items which have been acked by majority of cluster
	long matchIndex;

	// points to current log items which are awaiting ack by majority of cluster
	long nextIndex;

	unsigned int numNodes;
	struct Node nodes[MAXNODES];

	unsigned int numVotes;
	uuid_t votees[MAXNODES];

	unsigned int expectedNumInitialNodes;
};


void bootstrapNode(struct NodeState *node, int numNodes);
void initNode(struct NodeState *node, uuid_t ID);
void pumpNode(struct NodeState *node);
void addNode(struct NodeState *node, uuid_t otherNodeID);
void joinCluster(struct NodeState *node, uuid_t otherNode);
struct Node* getNode(struct NodeState *node, uuid_t ID);

