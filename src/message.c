#include "message.h"
#include <stdlib.h>

struct element {
	struct element *next;
	uuid_t nodeID;
	struct Message message;
};


struct element *root = NULL;
struct element *last = NULL;

int readMessage(uuid_t node, struct Message *message) {
	struct element *prev = NULL;
	for( struct element *p = root; p != NULL; p = p->next ) {
		if( uuid_compare(p->nodeID, node) == 0 ) {
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

void sendMessage(uuid_t to_node, struct Message message)
{
	struct element *e = malloc(sizeof(struct element));
	e->next = NULL;
	uuid_copy(e->nodeID, to_node);
	e->message = message;
	if( root == NULL ) {
		last = root = e;
	} else {
		last->next = e;
		last = e;
	}

}

int numMessages()
{
	int num = 0;
	for( struct element *p = root; p != NULL; p = p->next ) {
		num++;
	}
	return num;
}
