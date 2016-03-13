#include <stdio.h>
#include <stdlib.h>
#include "Util/CmdLineParser.h"
#include "Util/Geometry.h"
#include "Util/Ply.h"


////////////////////////////
// Command line parsing info
cmdLineParameter< char* > In( "in" ) , Out( "out" );
cmdLineReadable ForVisualization( "viewable" );
cmdLineReadable* params[] = { &In , &Out , &ForVisualization , NULL };

void ShowUsage( const char* ex )
{
	printf( "Usage %s:\n" , ex );
	printf( "\t --%s <input 2D polygon vertices>\n" , In.name );
	printf( "\t[--%s <output 2D triangulation>]\n" , Out.name );
	printf( "\t[--%s]\n" , ForVisualization.name );
}
// Command line parsing info
////////////////////////////

using namespace Geometry;

template< class CType >
void Triangulate( const std::vector< Point< 2 , CType > >& polygon , std::vector< Triangle >& triangles )
{
	/////////////////////////////
	// You need to implement this
	/////////////////////////////
}

int main( int argc , char* argv[] )
{
	cmdLineParse( argc-1 , argv+1 , params );
	if( !In.set )
	{
		ShowUsage( argv[0] );
		return EXIT_FAILURE;
	}

	// Information for how the vertices are described in a PLY file
	std::vector< Point< 2 , int > > polygonVertices;
	std::vector< Triangle > triangles;
	int file_type;

	PlyProperty Point2iProperties[] =
	{
		{ "x" , PLY_INT , PLY_INT , int( offsetof( Point2i , coordinates[0] ) ) , 0 , 0 , 0 , 0 } ,
		{ "y" , PLY_INT , PLY_INT , int( offsetof( Point2i , coordinates[1] ) ) , 0 , 0 , 0 , 0 }
	};

	PLY::Read( In.value , polygonVertices , NULL , NULL , NULL , Point2iProperties , NULL , 2 , file_type );

	Triangulate( polygonVertices , triangles );

	if( Out.set )
	{
		if( ForVisualization.set )
		{
			PlyProperty Point3fProperties[] =
			{
				{ "x" , PLY_FLOAT , PLY_FLOAT , int( offsetof( Point3f , coordinates[0] ) ) , 0 , 0 , 0 , 0 } ,
				{ "y" , PLY_FLOAT , PLY_FLOAT , int( offsetof( Point3f , coordinates[1] ) ) , 0 , 0 , 0 , 0 } ,
				{ "z" , PLY_FLOAT , PLY_FLOAT , int( offsetof( Point3f , coordinates[2] ) ) , 0 , 0 , 0 , 0 }
			};

			std::vector< std::vector< unsigned int > > polygons( triangles.size() );
			std::vector< Point3f > _polygonVertices( polygonVertices.size() );
			for( int i=0 ; i<triangles.size() ; i++ )
			{
				polygons[i].resize(3);
				for( int j=0 ; j<3 ; j++ ) polygons[i][j] = triangles[i][j];
			}
			for( int i=0 ; i<polygonVertices.size() ; i++ )
			{
				_polygonVertices[i][0] = (float) polygonVertices[i][0];
				_polygonVertices[i][1] = (float) polygonVertices[i][1];
				_polygonVertices[i][2] = 0.f;
			}
			PLY::Write( Out.value , _polygonVertices , polygons , Point3fProperties , 2 , file_type );
		}
		else PLY::Write( Out.value , polygonVertices , NULL , &triangles , NULL , Point2iProperties , 2 , file_type );
	}

	return EXIT_SUCCESS;
}

