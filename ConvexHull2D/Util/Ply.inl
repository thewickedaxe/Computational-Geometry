/* -*- C++ -*-
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/
namespace PLY
{
	struct ply_simplex
	{
		unsigned int nr_vertices;
		unsigned int *vertices;
	};
	typedef ply_simplex ply_face;

	static PlyProperty simplex_props[] = { { "vertex_indices" , PLY_INT , PLY_INT , offsetof( ply_simplex , vertices ) , 1 , PLY_INT , PLY_INT , offsetof( ply_simplex , nr_vertices ) } };
	static PlyProperty    face_props[] = { { "vertex_indices" , PLY_INT , PLY_INT , offsetof( ply_face    , vertices ) , 1 , PLY_INT , PLY_INT , offsetof( ply_face    , nr_vertices ) } };

	template< class Vertex >
	bool Read
	(
		const char* fileName ,
		std::vector< Vertex >& vertices ,
		std::vector< Geometry::Edge >* edges ,
		std::vector< Geometry::Triangle >* triangles ,
		std::vector< Geometry::Tet >* tets ,
		PlyProperty* vertexProperties , bool* vertexPropertyFlags , int vertexPropertyNum ,
		int& file_type ,
		char*** comments , int* commentNum
	)
	{
		int nr_elems;
		char** elist;
		float version;

		PlyFile* ply = ply_open_for_reading( fileName , &nr_elems , &elist , &file_type , &version );
		if( !ply ) return false;

		vertices.clear();
		if( edges ) edges->clear();
		if( triangles ) triangles->clear();
		if( tets ) tets->clear();

		if( comments )
		{
			(*comments) = new char*[*commentNum+ply->num_comments];
			for( int i=0 ; i<ply->num_comments ; i++ ) (*comments)[i] = _strdup( ply->comments[i] );
			*commentNum = ply->num_comments;
		}

		for( int i=0 ; i<nr_elems; i++ )
		{
			char* elem_name = elist[i];
			int num_elems , nr_props;
			PlyProperty** plist = ply_get_element_description( ply , elem_name , &num_elems , &nr_props );

			if( !plist )
			{
				for( int i=0 ; i<nr_elems ; i++ )
				{
					free( ply->elems[i]->name );
					free( ply->elems[i]->store_prop );
					for( int j=0 ; j<ply->elems[i]->nprops ; j++ )
					{
						free( ply->elems[i]->props[j]->name );
						free( ply->elems[i]->props[j] );
					}
					free( ply->elems[i]->props );
				}
				for( int i=0 ; i<nr_elems ; i++ ) free( ply->elems[i] );
				free( ply->elems );
				for( int i=0 ; i<ply->num_comments ; i++ ) free( ply->comments[i] );
				free( ply->comments );
				for( int i=0 ; i<ply->num_obj_info ; i++ ) free(ply->obj_info[i]);
				free( ply->obj_info );
				ply_free_other_elements( ply->other_elems );
			
				for( int i=0 ; i<nr_elems ; i++ ) free(elist[i]);
				free( elist );
				ply_close( ply );
				return false;
			}
			if( equal_strings( "vertex" , elem_name ) )
			{
				for( int j=0 ; j<vertexPropertyNum ; j++ )
				{
					bool hasProperty = ply_get_property( ply , elem_name , &vertexProperties[j] );
					if( vertexPropertyFlags ) vertexPropertyFlags[j] = hasProperty;
				}
				vertices.resize( num_elems );
				for( int k=0 ; k<num_elems ; k++ ) ply_get_element( ply , (void*)&vertices[k] );
			}
			else if( equal_strings( "simplex" , elem_name ) && edges )
			{
				ply_simplex simplex;
				ply_get_property( ply , elem_name , &simplex_props[0] );
				for( int j=0 ; j<num_elems ; j++ )
				{
					ply_get_element( ply , (void*)&simplex );
					switch( simplex.nr_vertices )
					{
					case 2:
						if( edges )
						{
							Geometry::Simplex< 1 > e;
							e[0] = simplex.vertices[0] , e[1] = simplex.vertices[1];
							edges->push_back( e );
						}
						break;
					case 3:
						if( triangles )
						{
							Geometry::Simplex< 2 > t;
							t[0] = simplex.vertices[0] , t[1] = simplex.vertices[1] , t[2] = simplex.vertices[2];
							triangles->push_back( t );
						}
						break;
					case 4:
						if( tets )
						{
							Geometry::Simplex< 3 > t;
							t[0] = simplex.vertices[0] , t[1] = simplex.vertices[1] , t[2] = simplex.vertices[2] , t[3] = simplex.vertices[3];
							tets->push_back( t );
						}
						break;
					}
					delete[] simplex.vertices;
				}
			}
			else ply_get_other_element( ply , elem_name , num_elems );

			for( int j=0 ; j<nr_props ; j++ ) free( plist[j]->name ) , free( plist[j] );

			free( plist );
		}  // for each type of element
	
		for( int i=0 ; i<nr_elems ; i++ )
		{
			free( ply->elems[i]->name );
			free( ply->elems[i]->store_prop );
			for( int j=0 ; j<ply->elems[i]->nprops ; j++ ) free( ply->elems[i]->props[j]->name ) , free( ply->elems[i]->props[j] );
			if( ply->elems[i]->props && ply->elems[i]->nprops ) free(ply->elems[i]->props);
		}
		for( int i=0 ; i<nr_elems ; i++ ) free( ply->elems[i] );
		free( ply->elems );
		for( int i=0 ; i<ply->num_comments ; i++ ) free( ply->comments[i] );
		free( ply->comments );
		for( int i=0 ; i<ply->num_obj_info ; i++ ) free( ply->obj_info[i] );
		free( ply->obj_info );
		ply_free_other_elements( ply->other_elems );
	
	
		for( int i=0 ; i<nr_elems ; i++ ) free( elist[i] );
		free( elist );
		ply_close( ply );
		return true;
	}

	template< class Vertex >
	bool Write
	(
		const char* fileName ,
		const std::vector< Vertex >& vertices ,
		const std::vector< Geometry::Edge >* edges ,
		const std::vector< Geometry::Triangle >* triangles ,
		const std::vector< Geometry::Tet >* tets ,
		PlyProperty* vertexProperties , int vertexPropertyNum ,
		int file_type ,
		char** comments , int commentNum
	)
	{
		float version;
		char *elem_names[] = { "vertex", "simplex" };
		PlyFile *ply = ply_open_for_writing( fileName , 2 , elem_names , file_type , &version );
		if( !ply ) return false;
	
		{
			ply_element_count( ply , "vertex" , (int)vertices.size() );
			for( int i=0 ; i<vertexPropertyNum ; i++ ) ply_describe_property( ply , "vertex" ,  &vertexProperties[i] );
		}

		int sCount = 0;
		if( edges )     sCount += (int)    edges->size();
		if( triangles ) sCount += (int)triangles->size();
		if( tets )      sCount += (int)     tets->size();
		if( sCount )
		{
			ply_element_count( ply , elem_names[1] , sCount );
			ply_describe_property( ply , elem_names[1] , &simplex_props[0] );
		}

		// Write in the comments
		if( comments && commentNum ) for( int i=0 ; i<commentNum ; i++ ) ply_put_comment( ply , comments[i] );

		ply_header_complete( ply );
	
		// write vertices
		ply_put_element_setup( ply , "vertex" );
		for( int i=0 ; i<vertices.size() ; i++ ) ply_put_element( ply , (void*)&vertices[i] );

		// write faces
		if( sCount )
		{
			ply_put_element_setup( ply , elem_names[1] );

			ply_simplex simplex;

			if( edges )
			{
				unsigned int simplex_vertices[2];
				simplex.nr_vertices = 2;
				simplex.vertices = simplex_vertices;
				for( int i=0 ; i<edges->size() ; i++ )
				{
					simplex.vertices[0] = ( *edges )[i][0] , simplex.vertices[1] = ( *edges )[i][1];
					ply_put_element( ply , (void*)&simplex );
				}
			}
			if( triangles )
			{
				unsigned int simplex_vertices[3];
				simplex.nr_vertices = 3;
				simplex.vertices = simplex_vertices;
				for( int i=0 ; i<triangles->size() ; i++ )
				{
					simplex.vertices[0] = ( *triangles )[i][0] , simplex.vertices[1] = ( *triangles )[i][1] , simplex.vertices[2] = ( *triangles )[i][2];
					ply_put_element( ply , (void*)&simplex );
				}
			}
			if( tets )
			{
				unsigned int simplex_vertices[4];
				simplex.nr_vertices = 4;
				simplex.vertices = simplex_vertices;
				for( int i=0 ; i<tets->size() ; i++ )
				{
					simplex.vertices[0] = ( *tets )[i][0] , simplex.vertices[1] = ( *tets )[i][1] , simplex.vertices[2] = ( *tets )[i][2] , simplex.vertices[3] = ( *tets )[i][3];
					ply_put_element( ply , (void*)&simplex );
				}
			}
		}
		ply_close( ply );
		return true;
	}
	template< class Vertex >
	bool Write
	(
		const char* fileName ,
		const std::vector< Vertex >& vertices ,
		const std::vector< std::vector< unsigned int > >& faces ,
		PlyProperty* vertexProperties , int vertexPropertyNum ,
		int file_type ,
		char** comments , int commentNum
	)
	{
		float version;
		char *elem_names[] = { "vertex", "face" };
		PlyFile *ply = ply_open_for_writing( fileName , 2 , elem_names , file_type , &version );
		if( !ply ) return false;
	
		{
			ply_element_count( ply , "vertex" , (int)vertices.size() );
			for( int i=0 ; i<vertexPropertyNum ; i++ ) ply_describe_property( ply , "vertex" ,  &vertexProperties[i] );
		}

		if( faces.size() )
		{
			ply_element_count( ply , elem_names[1] , (unsigned int)faces.size() );
			ply_describe_property( ply , elem_names[1] , &simplex_props[0] );
		}

		// Write in the comments
		if( comments && commentNum ) for( int i=0 ; i<commentNum ; i++ ) ply_put_comment( ply , comments[i] );

		ply_header_complete( ply );
	
		// write vertices
		ply_put_element_setup( ply , "vertex" );
		for( int i=0 ; i<vertices.size() ; i++ ) ply_put_element( ply , (void*)&vertices[i] );

		// write faces
		if( faces.size() )
		{
			ply_put_element_setup( ply , elem_names[1] );

			ply_face face;

			for( int i=0 ; i<faces.size() ; i++ )
			{
				face.nr_vertices = (unsigned int)faces[i].size();
				face.vertices = new unsigned int[ face.nr_vertices ];
				for( int j=0 ; j<faces[i].size() ; j++ ) face.vertices[j] = faces[i][j];
				ply_put_element( ply , (void*)&face );
				delete[] face.vertices;
			}
		}
		ply_close( ply );
		return true;
	}
};
