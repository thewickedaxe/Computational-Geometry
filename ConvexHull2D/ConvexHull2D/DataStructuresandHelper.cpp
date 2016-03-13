#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "GeometricHelpers.cpp"
#include "Util/Geometry.h"

using namespace Geometry;

struct PointContainer {
	Point<2, int> point;
	int index;
	PointContainer* next;
};


//*********************************************************************************************************************

struct Edge_Space_Combo {
	PointContainer* v1;
	PointContainer* v2;
	PointContainer* point_space_head;
	Edge_Space_Combo* next;
};


//*********************************************************************************************************************
struct Point_Slope {
	Point<2, int> point;
	float slope;

	bool operator < (const Point_Slope& str) const
    {
        if (slope != str.slope) {
        	return (slope <= str.slope);
        } else {
        	return (point[1] <= str.point[1]);
        }
    }
};
