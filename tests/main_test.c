#include <stdio.h>
#include <check.h>

#include "node.h"

START_TEST(test_cluster)
{
	int numNodes = 10;
	struct NodeState nodes[numNodes];
	for( int i = 0; i < numNodes; i++ ) {
		bootstrapNode(&nodes[i], numNodes);
		//initNode(&nodes[i], i);
	}

	for( int i = 1; i < numNodes; i++ ) {
		connectNode(&nodes[i], nodes[i].self->ID);
	}

	int max_pumps = 1000;
	for(int pumps_left = max_pumps;;pumps_left--) {
		for( int i = 0; i < numNodes; i++ ) {
			pumpNode(&nodes[i]);
		}
		if( nodes[0].leader ) {
			break;
		}
		if( pumps_left == 0 ) {
			ck_abort_msg("no leader elected after %d pumps", max_pumps);
		}
	}

	for( int i = 0; i < numNodes; i++ ) {
		ck_assert( uuid_compare( nodes[i].leader->ID, nodes[0].leader->ID ) );
	}




	int x = 12;
	ck_assert_int_eq(x, 5);
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
