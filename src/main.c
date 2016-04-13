#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>

#include "clock.h"

const long timeoutTime = 1000;
const unsigned int maxNodes = 1024;
#define MAXNODES 1024

enum NodeState {
	follower,
	candidate,
	leader
};

enum MessageType {
	leaderRequest,
	leaderResponse
};

// Information about other nodes in the cluster
struct Node {
	long long ID;
};

struct SelfNode {
	enum NodeState state;
	long long ID;
	long long timeout;
	long long termNumber;

	// points to index of log items which have been acked by majority of cluster
	long matchIndex;

	// points to current log items which are awaiting ack by majority of cluster
	long nextIndex;

	unsigned int numNodes;
	struct Node nodes[MAXNODES];

	unsigned int numVotes;
	long long votees[MAXNODES];
};

struct Message {
	long long nodeID;
	long long termNumber;
	enum MessageType type;

	// leaderResult fields
	int ok;
};

struct element {
	struct element *next;
	long long nodeID;
	struct Message message;
};

double randUnitInterval()
{
    return (double)rand() / (double)RAND_MAX ;
}

void initNode(struct SelfNode *node, long long ID) {
	node->state = follower;
	node->timeout = getClock() + timeoutTime * (1.5 + randUnitInterval() / 2.0);
	node->termNumber = 0;
	node->ID = ID;
	node->numNodes = 0;
	node->numVotes = 0;
}

struct element *root = NULL;
struct element *last = NULL;
void sendMessage(long long node, struct Message message) {
	struct element *e = malloc(sizeof(struct element));
	e->next = NULL;
	e->nodeID = node;
	e->message = message;
	if( root == NULL ) {
		last = root = e;
	} else {
		last->next = e;
		last = e;
	}

}

void addNode(struct SelfNode *node, long long otherNodeID)
{
	unsigned int i = node->numNodes;
	//assert( i < MAXNODES );
	node->nodes[i].ID = otherNodeID;
	node->numNodes += 1;
}

void addVotee(struct SelfNode *node, long long voteeID)
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


int readMessage(long long node, struct Message *message) {
	struct element *prev = NULL;
	for( struct element *p = root; p != NULL; p = p->next ) {
		if( p->nodeID == node ) {
			*message = p->message;
			if( prev == NULL ) {
				root = p->next;
			} else {
				prev->next = p->next;
			}
			free(p);
			return 1;
		}
		prev = p;
	}
	return 0;
}

void broadcastMessage(struct SelfNode *node, struct Message message) {
	for( unsigned int i = 0; i < node->numNodes; i++ ) {
		sendMessage(node->nodes[i].ID, message);
	}
}

void sendHeartBeat(struct SelfNode *node) {
	printf("sending heartbeat\n");
}

void resetTimeout(struct SelfNode *node) {
	node->timeout = getClock() + timeoutTime * (1.5 + randUnitInterval() / 2.0);
}

void becomeCandidate(struct SelfNode *node) {
	printf("candidate mode: %lld\n", node->timeout);
	node->state = candidate;
	node->termNumber++;
	resetTimeout(node);

	struct Message message;
	message.type = leaderRequest;
	message.nodeID = node->ID;
	message.termNumber = node->termNumber;
	broadcastMessage(node, message);
}

void pumpNode(struct SelfNode *node) {
	struct Message message;
	struct Message response;
	response.nodeID = node->ID;
	int ok = readMessage(node->ID, &message);
	if( ok ) {
		printf("got message: %d\n", message.type);
		switch( message.type ) {
			case leaderRequest:
				response.type = leaderResponse;
				if( message.termNumber > node->termNumber ) {
					response.ok = 1;
					response.termNumber = message.termNumber;
					node->termNumber = message.termNumber;
				} else {
					response.ok = 0;
					response.termNumber = message.termNumber;
				}
				sendMessage( message.termNumber, response );
				break;

			case leaderResponse:
				if( message.termNumber == node->termNumber ) {
					if( message.ok ) {
						addVotee( node, message.nodeID );
					}
				}
				break;
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

int main()
{
	int numNodes = 10;
	struct SelfNode nodes[numNodes];
	for( int i = 0; i < numNodes; i++ ) {
		initNode(&nodes[i], i);
	}

	for( int i = 0; i < numNodes; i++ ) {
		for( int j = 0; j < numNodes; j++ ) {
			if( i != j ) {
				addNode(&nodes[i], nodes[j].ID);
			}
		}
	}

	for(;;) {
		for( int i = 0; i < numNodes; i++ ) {
			pumpNode(&nodes[i]);
		}
	}



    //void *context = zmq_ctx_new ();
    //void *responder = zmq_socket (context, ZMQ_REP);
    //int rc = zmq_bind (responder, "tcp://*:5555");

    //while (1) {
    //    char buffer [10];
    //    zmq_recv (responder, buffer, 10, 0);
    //    printf ("Received Hello\n");
//  //      sleep (1);          //  Do some 'work'
    //    zmq_send (responder, "World", 5, 0);
    //}
	//return 0;
}
