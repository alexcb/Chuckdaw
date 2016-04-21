#include "node.h"
#include "message.h"
#include "clock.h"

#include <stdlib.h>
#include <stdio.h>

const long timeoutTime = 1000;
const unsigned int maxNodes = 1024;


double randUnitInterval()
{
    return (double)rand() / (double)RAND_MAX ;
}

void initNode(struct NodeState *node, long long ID)
{
	node->state = follower;
	node->timeout = getClock() + timeoutTime * (1.5 + randUnitInterval() / 2.0);
	node->termNumber = 0;
	node->ID = ID;
	node->numNodes = 0;
	node->numVotes = 0;
	node->leader = NULL;
}

void addNode(struct NodeState *node, long long otherNodeID)
{
	unsigned int i = node->numNodes;
	//assert( i < MAXNODES );
	node->nodes[i].ID = otherNodeID;
	node->numNodes += 1;
}

struct Node* getNode(struct NodeState *node, long long ID)
{
	for( unsigned int i = 0; i < node->numNodes; i++ ) {
		if( ID == node->nodes[i].ID )
			return &node->nodes[i];
	}
	return NULL;
}

void addVotee(struct NodeState *node, long long voteeID)
{
	for( unsigned int i = 0; i < node->numVotes; i++ ) {
		if( voteeID == node->votees[i] )
			return;
	}
	unsigned int i = node->numVotes;
	//assert( i < MAXNODES );
	node->votees[i] = voteeID;
	node->numVotes += 1;
}


void resetTimeout(struct NodeState *node, int heartbeat) {
	if( heartbeat ) {
		node->timeout = getClock() + timeoutTime;
	} else {
		node->timeout = getClock() + timeoutTime * (1.5 + randUnitInterval() / 2.0);
	}
}

void broadcastMessage(struct NodeState *node, struct Message message) {
	for( unsigned int i = 0; i < node->numNodes; i++ ) {
		sendMessage(node->nodes[i].ID, message);
	}
}


void sendHeartBeat(struct NodeState *node) {
	printf("%lld: sending heartbeat\n", node->ID);

	struct Message response;
	response.nodeID = node->ID;
	response.type = leaderHeartbeat;
	response.termNumber = node->termNumber;
	broadcastMessage(node, response);

	resetTimeout(node, 1);
}


void becomeCandidate(struct NodeState *node) {
	printf("%lld: candidate mode: %lld\n", node->ID, node->timeout);
	node->state = candidate;
	node->termNumber++;
	resetTimeout(node, 0);

	struct Message message;
	message.type = leaderRequest;
	message.nodeID = node->ID;
	message.termNumber = node->termNumber;
	broadcastMessage(node, message);
}

void pumpNode(struct NodeState *node) {
	struct Message message;
	int ok = readMessage(node->ID, &message);
	if( ok ) {
		switch( message.type ) {
			case leaderRequest:
				printf("%lld: got message leaderRequest from %lld\n", node->ID, message.nodeID);
				struct Message response;
				response.nodeID = node->ID;
				response.type = leaderResponse;
				response.termNumber = message.termNumber;
				if( message.termNumber > node->termNumber ) {
					response.ok = 1;
					node->termNumber = message.termNumber;
				} else {
					response.ok = 0;
				}
				sendMessage( message.nodeID, response );
				break;

			case leaderResponse:
				printf("%lld: got leaderResponse from %lld\n", node->ID, message.nodeID);
				if( message.termNumber == node->termNumber ) {
					if( message.ok ) {
						addVotee( node, message.nodeID );
						if( node->numVotes > (node->numNodes / 2 + 1) && node->state == candidate) {
							node->state = leader;
							sendHeartBeat(node);
						}
					}
				}
				break;

			case leaderHeartbeat:
				printf("%lld: got leaderHeartbeat from %lld\n", node->ID, message.nodeID);
				if( message.termNumber >= node->termNumber ) {
					node->termNumber = message.termNumber;
					node->state = follower;
					node->leader = getNode(node, message.nodeID);

					resetTimeout(node, 0);
				}
				break;

			default:
				printf("%lld: got message unhandled: %d from %lld\n", node->ID, message.type, message.nodeID);
		}

	}

	if( node->timeout < getClock() ) {
		switch( node->state ) {
			case follower:
				becomeCandidate(node);
				break;
			case candidate:
				becomeCandidate(node);
				break;
			case leader:
				sendHeartBeat(node);
				break;
		}
	}
}

