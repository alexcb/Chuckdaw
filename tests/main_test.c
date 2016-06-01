#include <stdio.h>
#include <check.h>

#include "node.h"
#include "message.h"
#include "clock.h"
#include "uuid_util.h"

void pumpNodes( int numNodes, struct NodeState *nodes, int numPumps, long long addClockValue )
{
	while( numPumps-- > 0 ) {
		for( int j = 0; j < 100; j++ ) {
			for( int i = 0; i < numNodes; i++ ) {
				pumpNode(&nodes[i]);
			}
		}
		if( addClockValue ) {
			addClock( addClockValue );
		}
	}
}

START_TEST(test_cluster)
{
	int numNodes = 10;
	struct NodeState nodes[numNodes];
	for( int i = 0; i < numNodes; i++ ) {
		bootstrapNode( &nodes[i], numNodes );
	}

	// tell other nodes about node[0]
	for( int i = 1; i < numNodes - 1; i++ ) {
		joinCluster( &nodes[i], nodes[0].self->ID );
		pumpNodes( numNodes, nodes, 10, 0 );
	}

	// ensure no leaders have been elected as the numNodes bootstrap has not yet been reached
	for( int i = 0; i < numNodes - 1; i++ ) {
		ck_assert_msg( nodes[i].termNumber == 0, "bad term number for node %d", i );
		ck_assert_msg( nodes[i].leader == NULL, "leader exists for node %d", i );
		ck_assert_msg( nodes[i].state == bootstrap, "bad state for node %d", i );
	}

	// add final node
	joinCluster( &nodes[numNodes-1], nodes[0].self->ID );

	pumpNodes( numNodes, nodes, 50, timeoutTime / 10 );
	
	// extra pumps to clear up unhandled messages that may have just fired due to a timeout
	pumpNodes( numNodes, nodes, 50, 0 );
	

	if( numMessages() != 0 ) {
		ck_abort_msg("not all messages were consumed");
	}

	for( int i = 0; i < numNodes; i++ ) {
		uuid_to_string(nodeID, nodes[i].self->ID)
		uuid_to_string(leaderID, nodes[i].leader->ID)
		printf("%s: leader for term %lld is %s\n", nodeID, nodes[i].termNumber, leaderID );

		ck_assert_msg( nodes[i].termNumber == nodes[0].termNumber, "bad term number for node %d", i );
		ck_assert_msg( nodes[i].leader != NULL, "no leader for node %d", i );
		ck_assert_msg( uuid_compare( nodes[i].leader->ID, nodes[0].leader->ID) == 0, "leader missmatch %d", i );
	}
}
END_TEST

Suite * unamed_test_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("unamed test suite");

	/* Core test case */
	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_cluster);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s;
	SRunner *sr;
	
	s = unamed_test_suite();
	sr = srunner_create(s);
	
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed != 0);
}
