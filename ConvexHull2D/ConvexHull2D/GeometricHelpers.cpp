#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Util/Geometry.h"

using namespace Geometry;

void testline(int x) {
	std::cout<<"Test"<<x<<"\n";
}

void who_are_you(Point<2, int> p) {
	std::cout<<p[0]<<", "<<p[1]<<"\n";
}

bool check_points(Point<2, int> p1, Point<2, int> p2) {
	if((p1[0] == p2[0]) && (p1[1] == p2[1])) {
		return true;
	} else {
		return false;
	}
}