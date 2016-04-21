#include <stdio.h>
#include <check.h>

#include "node.h"

START_TEST(test_cluster)
{
	int numNodes = 10;
	struct NodeState nodes[numNodes];
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



	int x = 12;
	ck_assert_int_eq(x, 5);
}
END_TEST

Suite * money_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Money");

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
	
	s = money_suite();
	sr = srunner_create(s);
	
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed != 0);
}
