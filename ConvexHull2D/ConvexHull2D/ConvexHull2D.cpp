#include <stdio.h>
#include <stdlib.h>
#include <unordered_set>
#include <stack>
#include <queue>
#include <algorithm>
#include "Util/CmdLineParser.h"
#include "Util/Geometry.h"
#include "Util/Timer.h"
#include "Util/Ply.h"
#include "DataStructuresandHelper.cpp"


////////////////////////////
// Command line parsing info
enum
{
	ALGORITHM_NAIVE ,
	ALGORITHM_QUICK_HULL ,
	ALGORITHM_GRAHAM ,
	ALGORITHM_COUNT
};
const char* AlgorithmNames[] = { "Naive" , "QuickHull" , "Graham" };

cmdLineParameter< char* > Out( "out" );
cmdLineParameter< int > Count( "count" ) , AlgorithmType( "aType" , ALGORITHM_NAIVE ) , RandomSeed( "sRand" , 0 ) , Resolution( "res" , 1024 );
cmdLineReadable ForVisualization( "viewable" );
cmdLineReadable* params[] = { &Count , &Out , &Resolution , &AlgorithmType , &RandomSeed , &ForVisualization , NULL };

void ShowUsage( const char* ex )
{
	printf( "Usage %s:\n" , ex );
	printf( "\t --%s <input vertex count>\n" , Count.name );
	printf( "\t[--%s <output 2D triangulation>]\n" , Out.name );
	printf( "\t[--%s <algorithm type>=%d]\n" , AlgorithmType.name , AlgorithmType.value );
	for( int i=0 ; i<ALGORITHM_COUNT ; i++ ) printf( "\t\t%d] %s\n" , i , AlgorithmNames[i] );
	printf( "\t[--%s <random seed>=%d]\n" , RandomSeed.name , RandomSeed.value );
	printf( "\t[--%s <grid resolution>=%d]\n" , Resolution.name , Resolution.value );
	printf( "\t[--%s]\n" , ForVisualization.name );
}
// Command line parsing info
////////////////////////////

using namespace Geometry;

long long Area2( const Point2i p[3] )
{
	long long a = 0;
	a += ( (long long)( p[1][0] + p[0][0] ) ) * ( p[1][1] - p[0][1] );
	a += ( (long long)( p[2][0] + p[1][0] ) ) * ( p[2][1] - p[1][1] );
	a += ( (long long)( p[0][0] + p[2][0] ) ) * ( p[0][1] - p[2][1] );
	return a;
}

long long Area2( Point2i p1 , Point2i p2 , Point2i p3 )
{
	Point2i p[] = { p1 , p2 , p3 };
	return Area2( p );
}

long long Dist2( Point2i p1 , Point2i p2 )
{
	long long dx = p1[0] - p2[0] , dy = p1[1] - p2[1];
	return dx*dx + dy*dy;
}

void RandomPoints( std::vector< Point2i >& points , int count , int seed , int res )
{
	srand( seed );

	// Add distinct random points in a disk
	points.reserve( count );
	Point2i center;
	center[0] = center[1] = res/2;
	long long r = res / 2;
	std::unordered_set< long long > usedPoints;
	while( points.size()<count )
	{
		Point2i p;
		p[0] = rand() % res , p[1] = rand() % res;
		{
			long long d[] = { center[0] - p[0] , center[1] - p[1] };
			if( d[0]*d[0] + d[1]*d[1]>r*r ) continue;
		}
		long long key = ( ( long long )p[0] ) << 32 | ( ( long long )p[1] );
		if( usedPoints.find( key )==usedPoints.end() ) points.push_back( p ) , usedPoints.insert( key );
	}
}

template< class CType >
void NaiveAlgorithm( std::vector< Point< 2 , CType > >& points , std::vector< Point< 2 , CType > >& hull )
{
	// Find the bottom-most (left-most) point
	int idx = 0;
	for( int i=1 ; i<points.size() ; i++ ) if( points[i][1]<points[idx][1] || ( points[i][1]==points[idx][1] && points[i][0]<points[idx][0] ) ) idx = i;

	// Remove it from the list of points
	hull.push_back( points[idx] ) , points[idx] = points.back() , points.pop_back();

	// Add the vertex making the right-most angle until either all the points are used, or you circle back around
	while( points.size() )
	{
		idx = 0;
		for( int i=1 ; i<points.size() ; i++ )
		{
			long long a = Area2( hull.back() , points[idx] , points[i] );
			if( a<0 || ( a==0 && Dist2( hull.back() , points[i] )>Dist2( hull.back() , points[idx] ) ) ) idx = i;
		}
		long long a = Area2( hull.back() , points[idx] , hull[0] );
		if( a<0 || ( a==0 && Dist2( hull.back() , hull[0] )>Dist2( hull.back() , points[idx] ) ) ) break;
		hull.push_back( points[idx] ) , points[idx] = points.back() , points.pop_back();
	}
}







//*********************************************************************************************************************
/*
	GLOBALS
*/
std::vector<Point<2, int> > quick_hull;
std::vector<Point<2, int> > quick_hull_top;
std::vector<Point<2, int> > quick_hull_bottom;
std::vector<Point<2, int> > vertices;

Point<2, int> get_left(std::vector<Point<2, int> > vecs) {
	Point<2, int> p;
	p[0] = 100000;
	p[1] = 100000;
	while(!vecs.empty()) {
		if(vecs[0][0] < p[0]) {
			p = vecs[0];
		}
		vecs.erase(vecs.begin());
	}
	return p;
}

Point<2, int> get_right(std::vector<Point<2, int> > vecs) {
	Point<2, int> p;
	p[0] = -1;
	p[1] = -1;
	while(!vecs.empty()) {
		if(vecs[0][0] > p[0]) {
			p = vecs[0];
		}
		vecs.erase(vecs.begin());
	}
	return p;
}

std::vector<Point<2, int> > partition(Point<2, int> v1, Point<2, int> v2, std::vector<Point<2, int> > verts) {
	std::vector<Point<2, int> > temp;
	while(!verts.empty()) {
		if (Area2(v1, v2, verts[0]) > 0) {
			temp.push_back(verts[0]);
		}
		verts.erase(verts.begin());
	}
	return temp;
}

void enumerate(std::vector<Point<2, int> > verts) {
	while (!verts.empty()) {
		who_are_you(verts[0]);
		verts.erase(verts.begin());
	}
}

std::vector<Point<2, int> > remove_by_element(Point<2, int> p, std::vector<Point<2, int> > verts) {
	int i;
	for(i = 0; i < verts.size(); i++) {
		if (check_points(verts[i], p)) {
			break;
		}
	}
	verts.erase(verts.begin() + i);
	return verts;
}

void compute_hull(Point<2, int> v1, Point<2, int> v2, std::vector<Point<2, int> > verts, std::vector<Point<2, int> > &hull_verts) {
	if (verts.empty()) {
		return;
	}
	Point<2, int> next;
	std::vector<Point<2, int> > verts_preserve = verts;
	long long max_area = 0;
	long long area = 0;
	while(!verts.empty()) {
		Point<2, int> v3 = verts[0];
		verts.erase(verts.begin());
		area = Area2(v1, v2, v3);
		if (area > max_area) {
			max_area = area;
			next = v3;
		}
	}
	verts = verts_preserve;
	if(hull_verts.empty()) {
		hull_verts.push_back(next);
		return;
	} else {
		for(int i = 0; i < hull_verts.size(); i++) {
			if(check_points(hull_verts[i], v1)) {
				hull_verts.insert(hull_verts.begin() + i + 1, next);
				break;
			}
		}
	}

	//We've added the max distance candidate to the hull now, we have to get rid of the interior points
	long long area1;
	long long area2;
	long long area3;
	verts = remove_by_element(next, verts);
	verts_preserve = verts;
	while(!verts_preserve.empty()) {
		Point<2, int> candidate = verts_preserve[0];
		verts_preserve.erase(verts_preserve.begin());
		area1 = Area2(v1, v2, candidate);
		area2 = Area2(v2, next, candidate);
		area3 = Area2(next, v1, candidate);
		if((area1 > 0) && (area2 > 0) && (area3 > 0)) {
			verts = remove_by_element(candidate, verts);
		}
	}
	compute_hull(v1, next, partition(v1, next, verts), hull_verts);
	compute_hull(next, v2, partition(next, v2, verts), hull_verts);
}

template< class CType >
void QuickHullAlgorithm( std::vector< Point< 2 , CType > >& points , std::vector< Point< 2 , CType > >& hull )
{
	vertices = points;
	Point<2, int> left_sentinel = get_left(vertices);
	Point<2, int> right_sentinel = get_right(vertices);
	quick_hull_top.push_back(left_sentinel);
	quick_hull_bottom.push_back(right_sentinel);
	compute_hull(left_sentinel, right_sentinel, partition(left_sentinel, right_sentinel, vertices), quick_hull_top);
	compute_hull(right_sentinel, left_sentinel, partition(right_sentinel, left_sentinel, vertices), quick_hull_bottom);
	/*enumerate(quick_hull_top);
	enumerate(quick_hull_bottom);*/	
	for (int i = 0; i < quick_hull_top.size(); i++) {
		quick_hull.push_back(quick_hull_top[i]);
	}
	for (int i = 0; i < quick_hull_bottom.size(); i++) {
		quick_hull.push_back(quick_hull_bottom[i]);
	}
	// enumerate(quick_hull);
	while(!quick_hull.empty()) {
		//who_are_you(graham_hull[0]);
		hull.push_back(quick_hull[0]);
		quick_hull.erase(quick_hull.begin());
	}
}


//*********************************************************************

/*
	Globals
*/
std::vector<Point<2, int> > graham_hull;
std::vector<Point<2, int> > graham_clone;
std::vector<Point_Slope> sorted_points;

float slope(Point<2, int> a, Point<2, int> b) {
	float ydiff = a[1] - b[1];
	float xdiff = a[0] - b[0];
	float sl = ydiff / xdiff;
	return sl;
}

template< class CType >
void GrahamsAlgorithm( std::vector< Point< 2 , CType > >& points , std::vector< Point< 2 , CType > >& hull )
{
	int idx = 0;
	for( int i=1 ; i < points.size() ; i++ ) if( points[i][1]<points[idx][1] || ( points[i][1]==points[idx][1] && points[i][0]<points[idx][0] ) ) idx = i;
	Point<2, int> sentinel = points[idx];
	
	// Push the sentinel point to the hull stack
	graham_hull.push_back(sentinel);

	// Dot products are hard to manage, using slope instead
	std::vector<Point_Slope> positive_slope_points;
	std::vector<Point_Slope> negative_slope_points;

	// Calc slope for each point in the space and add them in the sorted order of angle to a vector
	// O(n)
	for (int i = 0; i < points.size(); i++) {
		if(check_points(sentinel, points[i])) {			
			continue;
		}
		Point_Slope p;
		p.point = points[i];
		p.slope = slope(points[i], sentinel);
		if (p.slope <= 0) {
			negative_slope_points.push_back(p);
		} else {
			positive_slope_points.push_back(p);
		}		
	}

	// O(nlogn)
	std::sort(positive_slope_points.begin(), positive_slope_points.end());
	std::sort(negative_slope_points.begin(), negative_slope_points.end());	
	for (int i = 0; i < positive_slope_points.size(); i++) {
		sorted_points.push_back(positive_slope_points[i]);
	}
	for (int i = 0; i < negative_slope_points.size(); i++) {
		sorted_points.push_back(negative_slope_points[i]);
	}
	
	// Add the first two points to the prospective hull
	// Remove them from the points of contention
	// No pun intended
	graham_hull.push_back(sorted_points[0].point);
	graham_hull.push_back(sorted_points[1].point);
	sorted_points.erase(sorted_points.begin());
	sorted_points.erase(sorted_points.begin());
	
	while(!sorted_points.empty()) {
		Point<2, int> v3 = sorted_points[0].point;
		sorted_points.erase(sorted_points.begin());
		long long area = -1;
		do {
			Point<2, int> v1 = graham_hull.end()[-2];
			Point<2, int> v2 = graham_hull.end()[-1];
			area = Area2(v1, v2, v3);
			if (area <= 0) {			
				graham_hull.pop_back();
			}
		} while(area < 0);
		graham_hull.push_back(v3);
	}
	//One final check for any stray reflex vertices that might have crept in
	graham_clone = graham_hull;
	int j = 0;
	Point<2, int> x = graham_clone[j];
	Point<2, int> y = graham_clone[j + 1];
	Point<2, int> z= graham_clone[j + 2];
	while(j < graham_clone.size() - 2) {
		x = graham_clone[j];
		y = graham_clone[j + 1];
		z = graham_clone[j + 2];
		long long a = Area2(x, y, z);
		if(a <= 0) {			
			graham_clone = remove_by_element(y, graham_clone);
			j = 0;
		} else {
			j++;
		}
	}
	graham_hull = graham_clone;
	while(!graham_hull.empty()) {
		//who_are_you(graham_hull[0]);
		hull.push_back(graham_hull[0]);
		graham_hull.erase(graham_hull.begin());
	}
}


//*********************************************************************************************************************








int main( int argc , char* argv[] )
{
	PlyProperty Point2iProperties[] =
	{
		{ "x" , PLY_INT , PLY_INT , int( offsetof( Point2i , coordinates[0] ) ) , 0 , 0 , 0 , 0 } ,
		{ "y" , PLY_INT , PLY_INT , int( offsetof( Point2i , coordinates[1] ) ) , 0 , 0 , 0 , 0 }
	};
	PlyProperty Point3fProperties[] =
	{
		{ "x" , PLY_FLOAT , PLY_FLOAT , int( offsetof( Point3f , coordinates[0] ) ) , 0 , 0 , 0 , 0 } ,
		{ "y" , PLY_FLOAT , PLY_FLOAT , int( offsetof( Point3f , coordinates[1] ) ) , 0 , 0 , 0 , 0 } ,
		{ "z" , PLY_FLOAT , PLY_FLOAT , int( offsetof( Point3f , coordinates[2] ) ) , 0 , 0 , 0 , 0 }
	};


	cmdLineParse( argc-1 , argv+1 , params );
	if( !Count.set )
	{
		ShowUsage( argv[0] );
		return EXIT_FAILURE;
	}

	std::vector< Point< 2 , int > > points , hullVertices;
	{
		Timer t;
		RandomPoints( points , Count.value , RandomSeed.value , Resolution.value );
		printf( "Got random points: %.2f(s)\n" , t.elapsed() );
	}
	{
		Timer t;
		switch( AlgorithmType.value )
		{
			case ALGORITHM_NAIVE:          NaiveAlgorithm( points , hullVertices ) ; break;
			case ALGORITHM_QUICK_HULL: QuickHullAlgorithm( points , hullVertices ) ; break;
			case ALGORITHM_GRAHAM:       GrahamsAlgorithm( points , hullVertices ) ; break;
			default: fprintf( stderr , "[ERROR] Unrecognized algorithm type: %d\n" , AlgorithmType.value ) , exit( 0 );
		}
		printf( "Computed hull %d -> %d in %.2f (s)\n" , Count.value , (int)hullVertices.size() , t.elapsed() );
	}

	if( Out.set )
	{
		if( ForVisualization.set )
		{
			std::vector< std::vector< unsigned int > > polygons(1);
			std::vector< Point3f > _hullVertices( hullVertices.size() );
			polygons[0].resize( hullVertices.size() );
			for( int i=0 ; i<hullVertices.size() ; i++ ) polygons[0][i] = i;
			for( int i=0 ; i<hullVertices.size() ; i++ )
			{
				_hullVertices[i][0] = (float) hullVertices[i][0];
				_hullVertices[i][1] = (float) hullVertices[i][1];
				_hullVertices[i][2] = 0.f;
			}
			PLY::Write( Out.value , _hullVertices , polygons , Point3fProperties , 2 , PLY_ASCII );
		}
		else PLY::Write( Out.value , hullVertices , NULL , NULL , NULL , Point2iProperties , 2 , PLY_ASCII );
	}

	return EXIT_SUCCESS;
}

