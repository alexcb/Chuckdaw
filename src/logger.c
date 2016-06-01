#include "logger.h"
#include "node.h"
#include "clock.h"
#include "uuid_util.h"

#include <stdarg.h>
#include <stdio.h>

#define BUFSIZE 1024

//void node_debug(struct NodeState *nodeState, char *fmt, ...)
void node_debug(struct NodeState *nodeState, ...)
{
	va_list arg;

	char s[BUFSIZE];

	va_start(arg, nodeState);
	char *fmt = va_arg(arg, char*);
	vsnprintf(s, BUFSIZE, fmt, arg);
	va_end(arg);

	uuid_to_string(nodeID, nodeState->self->ID)

	printf("%lld %s: %s\n", getClock(), nodeID, s);
}
