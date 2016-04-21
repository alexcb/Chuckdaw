#include "message.h"
#include <stdlib.h>

struct element {
	struct element *next;
	long long nodeID;
	struct Message message;
};


struct element *root = NULL;
struct element *last = NULL;

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

void sendMessage(long long node, struct Message message)
{
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

