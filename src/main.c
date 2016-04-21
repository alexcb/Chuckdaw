#include <stdio.h>
#include <zmq.h>
#include <stdlib.h>

#include "node.h"


int main()
{
	//int numNodes = 10;
	//struct SelfNode nodes[numNodes];
	//for( int i = 0; i < numNodes; i++ ) {
	//	initNode(&nodes[i], i);
	//}

	//for( int i = 0; i < numNodes; i++ ) {
	//	for( int j = 0; j < numNodes; j++ ) {
	//		if( i != j ) {
	//			addNode(&nodes[i], nodes[j].ID);
	//		}
	//	}
	//}

	//for(;;) {
	//	for( int i = 0; i < numNodes; i++ ) {
	//		pumpNode(&nodes[i]);
	//	}
	//}



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
