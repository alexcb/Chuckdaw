#include "node.h"
#include "message.h"
#include "clock.h"
#include "consts.h"
#include "uuid_util.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <uuid/uuid.h>

void boardcastNodeMembers(struct NodeState *node);
void addVotee(struct NodeState *node, uuid_t voteeID);
void sendHeartBeat(struct NodeState *node);
void resetTimeout(struct NodeState *node);


double randUnitInterval()
{
    return (double)rand() / (double)RAND_MAX ;
}

void initNode(struct NodeState *node, uuid_t ID)
{
	//node->state = follower;
	//node->timeout = getClock() + timeoutTime * (1.5 + randUnitInterval() / 2.0);
	//node->termNumber = 0;
	////node->self->ID = ID;
	//node->numNodes = 0;
	//node->numVotes = 0;
	//node->leader = NULL;
}


// This is called when creating a new cluster. No node will attempt to become
// a leader until it knows of at least 'numNodes' nodes (including itself)
void bootstrapNode(struct NodeState *node, int numNodes)
{
	memset(node, 0, sizeof(struct NodeState));
	node->state = bootstrap;
	node->expectedNumInitialNodes = numNodes;
	node->self = &node->nodes[0];
	node->numNodes = 1;
	uuid_generate(node->self->ID);

	NODE_DEBUG(node, "Bootstrap complete");
}

void addNode(struct NodeState *node, uuid_t otherNode)
{
	uuid_to_string(other_uuid_str, otherNode)

	// ignore nodes we already know about
	if( getNode( node, otherNode ) != NULL ) {
		return;
	}

	NODE_DEBUG(node, "Adding node %s to list of known nodes", other_uuid_str);
	uuid_copy(node->nodes[node->numNodes].ID, otherNode);
	node->numNodes += 1;
}

// joinCluster adds the otherNode to it's list of nodes
// and sends a join request message to it
// the node will then broadcast a clusterMembershipUpdate
void joinCluster(struct NodeState *node, uuid_t otherNode)
{
	uuid_to_string(other_uuid_str, otherNode)
	NODE_DEBUG(node, "Attempting to join with node %s", other_uuid_str);

	addNode( node, otherNode );

	struct Message message = {
		.type = joinRequest,
		};
	uuid_copy( message.nodeID, node->self->ID );
	sendMessage( otherNode, message );
}

void handleJoinRequest(struct NodeState *node, struct Message *message)
{
	uuid_to_string( message_node_uuid_str, message->nodeID )
	NODE_DEBUG(node, "got message joinRequest from %s", message_node_uuid_str);

	addNode( node, message->nodeID );
	boardcastNodeMembers( node );
}

void handleClusterMembershipUpdate(struct NodeState *node, struct Message *message)
{
	uuid_to_string(message_node_uuid_str, message->nodeID)
	NODE_DEBUG(node, "got message clusterMembershipUpdate from %s", message_node_uuid_str);

	for( int i = 0; i < message->numNodes; i++ ) {
		addNode( node, message->nodes[i] );
	}
	NODE_DEBUG(node, "total known nodes: %d", node->numNodes);
}

void handleLeaderRequest(struct NodeState *node, struct Message *message)
{
	uuid_to_string(message_node_uuid_str, message->nodeID)
	NODE_DEBUG(node, "got leaderRequest from %s", message_node_uuid_str);

	//if( node->state == bootstrap ) {
	//	node->state = follower;
	//}

	struct Message response;
	uuid_copy(response.nodeID, node->self->ID);
	response.type = leaderResponse;
	response.termNumber = message->termNumber;
	if( message->termNumber > node->termNumber ) {
		response.ok = 1;
		node->termNumber = message->termNumber;
		NODE_DEBUG(node, "voting for %s in term %lld", message_node_uuid_str, node->termNumber);
	} else {
		response.ok = 0;
		NODE_DEBUG(node, "voting against %s in term %lld", message_node_uuid_str, node->termNumber);
	}
	sendMessage( message->nodeID, response );
}

void handleLeaderResponse(struct NodeState *node, struct Message *message)
{
	uuid_to_string(message_node_uuid_str, message->nodeID)
	NODE_DEBUG(node, "got leaderResponse from %s for term: %lld", message_node_uuid_str, message->termNumber);

	if( message->termNumber == node->termNumber ) {
		if( message->ok ) {
			addVotee( node, message->nodeID );
			if( node->numVotes > (node->numNodes / 2 + 1) && node->state == candidate) {
				node->state = leader;
				node->leader = node->self;
				NODE_DEBUG(node, "declaring self winner for term %lld", node->termNumber);
				sendHeartBeat(node);
			}
		}
	}
}

void handleLeaderHeartbeat(struct NodeState *node, struct Message *message)
{
	uuid_to_string(message_node_uuid_str, message->nodeID)
	NODE_DEBUG(node, "got leaderHeartbeat from %s", message_node_uuid_str);

	if( message->termNumber >= node->termNumber ) {
		NODE_DEBUG(node, "setting leader to %s for term %lld", message_node_uuid_str, message->termNumber);
		node->termNumber = message->termNumber;
		node->state = follower;
		node->leader = getNode(node, message->nodeID);

		resetTimeout(node);
	}
}


struct Node* getNode(struct NodeState *node, uuid_t ID)
{
	for( unsigned int i = 0; i < node->numNodes; i++ ) {
		if( uuid_compare(ID, node->nodes[i].ID) == 0 )
			return &node->nodes[i];
	}
	return NULL;
}

void addVotee(struct NodeState *node, uuid_t voteeID)
{
	for( unsigned int i = 0; i < node->numVotes; i++ ) {
		if( uuid_compare(voteeID, node->votees[i]) == 0 )
			return;
	}
	unsigned int i = node->numVotes;
	//assert( i < MAXNODES );
	uuid_copy( node->votees[i], voteeID);
	node->numVotes += 1;
}


void resetTimeout(struct NodeState *node) {
	if( node->state == leader ) {
		node->timeout = getClock() + timeoutTime;
	} else {
		node->timeout = getClock() + timeoutTime * (1.5 + randUnitInterval() / 2.0);
	}
}

void broadcastMessage(struct NodeState *node, struct Message message) {
	struct Node *p = NULL;
	for( unsigned int i = 0; i < node->numNodes; i++ ) {
		p = &node->nodes[i];
		if( p != node->self ) {
			sendMessage(node->nodes[i].ID, message);
		}
	}
}


void sendHeartBeat(struct NodeState *node) {
	NODE_DEBUG(node, "sending heartbeat");

	struct Message response;
	uuid_copy(response.nodeID, node->self->ID);
	response.type = leaderHeartbeat;
	response.termNumber = node->termNumber;
	broadcastMessage(node, response);

	resetTimeout(node);
}


void becomeCandidate(struct NodeState *node) {
	node->state = candidate;
	node->termNumber++;
	NODE_DEBUG(node, "candidate mode, timeout val: %lld, term number: %lld", node->timeout, node->termNumber);

	struct Message message;
	message.type = leaderRequest;
	uuid_copy(message.nodeID, node->self->ID);
	message.termNumber = node->termNumber;
	broadcastMessage(node, message);
}

void boardcastNodeMembers(struct NodeState *node) {
	NODE_DEBUG(node, "broadcasting known nodes");

	struct Message message;
	uuid_copy( message.nodeID, node->self->ID );
	message.type = clusterMembershipUpdate;
	message.numNodes = node->numNodes;
	for( int i = 0; i < node->numNodes; i++ ) {
		uuid_copy( message.nodes[i], node->nodes[i].ID );
	}

	broadcastMessage(node, message);
}

void pumpNode(struct NodeState *node) {
	struct Message message;
	int ok = readMessage(node->self->ID, &message);
	uuid_to_string(message_node_uuid_str, message.nodeID)
	if( ok ) {
		switch( message.type ) {
			case joinRequest:
				handleJoinRequest(node, &message);
				break;

			case clusterMembershipUpdate:
				handleClusterMembershipUpdate(node, &message);
				break;

			case leaderRequest:
				handleLeaderRequest(node, &message);
				break;

			case leaderResponse:
				handleLeaderResponse(node, &message);
				break;

			case leaderHeartbeat:
				handleLeaderHeartbeat(node, &message);
				break;

			default:
				NODE_DEBUG(node, "got message unhandled: %d from %s", message.type, message_node_uuid_str);
				break;
		}

	}

	if( node->timeout < getClock() ) {
		NODE_DEBUG(node, "timeout");
		switch( node->state ) {
			case bootstrap:
				if( node-> numNodes == node->expectedNumInitialNodes ) {
					becomeCandidate(node);
				} else {
					boardcastNodeMembers(node);
				}
				break;
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
		resetTimeout(node);
	}
}

