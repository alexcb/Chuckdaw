struct NodeState;


void node_debug(struct NodeState *nodeState, ...);

// I can't include `fmt` as the second argument due to compiler issues I don't understand.
// I really want this method to look like:
//    void node_debug(struct NodeState *nodeState, char *fmt, ...) __attribute__((format (printf, 2, 3)));
// but can't as calling node_debug(node, "hello world") causes an error.
// see: http://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick

#define NODE_DEBUG(nodeState, ...) node_debug(nodeState, ##__VA_ARGS__ )
