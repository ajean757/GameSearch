#pragma once
#include "GameLibrary.h"

/*
* Priority Queue that behaves like a vector
* Public Members
*	vector<set> elem
*	
* Methods
*	top() - access top element
*	pop() - remove set from top of priority queue (if we can make this constant time, then we win)
*	push() - insert element
*/

class PriorityVector {

	vector<set<appid>> elem;

public:

	PriorityVector() {

	}

	set<appid> top() {

	}

	void pop() {

	}

	bool push(set<appid> s) {

	}
};