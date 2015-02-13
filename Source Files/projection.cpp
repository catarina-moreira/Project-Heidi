/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* projection.cpp
* This file contains the definition of the functions that are used from projecting the database
* either usinng an orthogonal projection or PCA.
*
* Author: Catarina Moreira
* Personal Page: http://web.ist.utl.pt/~catarina.p.moreira/index.html
*
* Supervisor: Andreas Wichert
* Personal Page: http://web.ist.utl.pt/~andreas.wichert/
*
* ====================================================================================== */

#include "projection.hpp"

/* ======================================================================================
*
* project_database: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void project_database(HDBC hdbc)
{
	/* if the index option is not set, then the program returns without indexing the database */
	if (!PERFORM_INDEX_PHASE)
		return;

	sql_fill_database(hdbc, TOTAL_DIMENSIONS);

	/* compute the new dimensions according to the window sizes */
	int prev_dim = TOTAL_DIMENSIONS;
	int current_dim = TOTAL_DIMENSIONS / WINDOWS[0];
	
	/* start projecting the dataset. 
	* for each projection step, iteratively load chunks of data from the database with 
	* dimensions DATA_CHUNK x WINDOWS[proj_step]
	* then, multiply this piece of data by the orthogonal projection matrix and apply
	* a computational norm (either L1 or L2)
	* finally, save the projected dataset into a file and load it into the database */
	int proj_step;
	for (proj_step = 0; proj_step < NUM_PROJECTIONS; proj_step++)
	{
		/* select the current window size */
		int window = WINDOWS[proj_step];

		/* calculate the dimensions of the current and the previous projections
		 * if the proj_step is zero, then nothing is computed   */
		compute_dimensions(proj_step, &prev_dim, &current_dim);

		/* change the dataset path according to the current projection step */
		update_dataset_path(current_dim);

		/* change the DATASET_ROOT_NAME according to the updated DATASET_PATH */
		update_dataset_root_name( );

		/* create a file to temporarily store the projected data */
		FILE *projected_dataset_file = open_file();

		/* compute the number of times we need to partition the dataset according to a CHUNK_SIZE */
		int chunks_to_read = compute_num_chunks();

		/* Compute orthogonal projection matrix */
		gsl_matrix *projection_matrix = orthogonal_projection_matrix(window, window);

		/* for each projection step, iteratively load chunks of data from the database with
		* dimensions DATA_CHUNK x WINDOWS[proj_step] */
		int chunk_indx;
		
		for (chunk_indx = 0; chunk_indx < chunks_to_read; chunk_indx++)
		{
			/* compute the number of vectors to read in the chunk */
			long remaining_vecs = compute_num_vecs_to_load(chunk_indx, chunks_to_read);

			/* structure that will hold the projected data */
			gsl_matrix * projected_data = gsl_matrix_alloc(remaining_vecs, current_dim);

			/* load the chunk of data */
			gsl_matrix *database_matrix = load_data_chunk(hdbc, chunk_indx, chunks_to_read, remaining_vecs, prev_dim );

			/*  multiply this piece of data by the orthogonal projection matrix */
			compute_orthogonal_projection(projection_matrix, database_matrix, &projected_data, current_dim,
				window, chunk_indx, chunks_to_read, remaining_vecs);

			/* clear the memory */
			gsl_matrix_free(database_matrix);

			/* Write projected data to file */
			write_projection_data(projected_dataset_file, projected_data, remaining_vecs, current_dim);
			
			/* clear memory */
			gsl_matrix_free(projected_data);
		}

		fclose(projected_dataset_file);

		/* transfer data to database */
		update_table_name(current_dim);
		sql_fill_database(hdbc, current_dim);

		/* delete projected file */
		remove(DATASET_PATH);
	}
}

/* ======================================================================================
*
* project_database: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
long *perform_query(HDBC hdbc)
{
	/* if the index option is not set, then the program returns without indexing the database */
	if (!PERFORM_QUERY)
		return NULL;

	long *IDs;
	long *final_IDs = NULL;

	/* read the query vector */
	double *query = assign_query();

	/* compute query subspaces */
	gsl_matrix *query_matrix = computeSubspace( query );

	/* constant C */
	int const_c = 2;

	/* total number of similar vectors returned */
	NUM_ITEMS = 0;

	/* first projection dimension */
	int current_dim = TOTAL_DIMENSIONS/WINDOWS[0];

	/* compute the dimension of the lowest projection */
	int w;
	for( w = 1; w < NUM_PROJECTIONS; w++ )
		current_dim /= WINDOWS[w];

	int curr_step = 0;

	/* structure to hold the IDs of the most similar vectors returned */
	long size = 100000;
	final_IDs = (long *)malloc(sizeof(long)*( size ) );

	int i = 0; int dataset_tables;
	( BILLION_DATASET == 0 ) ? dataset_tables = 1 : dataset_tables = 30;

	for( i = 1; i <= dataset_tables; i++ )
	{

		/* update db_table_name for current dimension */
		DB_TABLE_NAME = (char *)malloc(sizeof(char)*(40));

		if( BILLION_DATASET == 0 )
			sprintf(DB_TABLE_NAME, "[dbo].[%s_%d_%s]", DATASET_ROOT_NAME, current_dim, NORM_TYPE);
		else
			sprintf(DB_TABLE_NAME, "[dbo].[%dbillion_%d_%s]", i, current_dim, NORM_TYPE);

		printf( "\n%s\n", DB_TABLE_NAME );

		/* compute the most similar vectors to the query vector */
		IDs = sql_compute_distances(hdbc, query_matrix, i, current_dim);

		/* add vectors IDs to final array */
		int j;
		for( j = 0; j < NUM_ITEMS; j++, curr_step++ )
			final_IDs[curr_step] = IDs[j];

		curr_step++;
		
		/* free temporary vector ID list */
		free( IDs );
	}

	/* update the global variable with the toal vectors returned */
	NUM_ITEMS = curr_step;

	return final_IDs;
}

/* ======================================================================================
*
* project_database: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
int flength_ids( long *IDs )
{
	int l;
	while(IDs[l] != NULL )
		l++;

	return l+1;
}


/* ======================================================================================
*
* computeQuerySubspace: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/


gsl_matrix *compute_subspace( double *query )
{
	/* convert double *query to gsl_matrix 
	 * fill just the first line of the matrix
	 */
	int i;
	gsl_matrix *query_matrix = gsl_matrix_alloc( NUM_PROJECTIONS+1, TOTAL_DIMENSIONS );
	for( i = 0; i < TOTAL_DIMENSIONS; i++ )
		gsl_matrix_set( query_matrix, 0, i, (double)query[i] );

	/*  */
	gsl_matrix *subquery = gsl_matrix_alloc( 1, TOTAL_DIMENSIONS );
	for( i = 0; i < TOTAL_DIMENSIONS; i++ )
		gsl_matrix_set( subquery, 0, i, (double)query[i] ); 

	int query_indx = 1;

	/* compute the new dimensions according to the window sizes */
	int prev_dim = TOTAL_DIMENSIONS;
	int current_dim = TOTAL_DIMENSIONS / WINDOWS[0];

	int proj_step;
	for( proj_step = 0; proj_step < NUM_PROJECTIONS; proj_step++ )
	{
		int window = WINDOWS[proj_step];

		/* calculate the dimensions of the current and the previous projections
		 * if the proj_step is zero, then nothing is computed   */
		compute_dimensions(proj_step, &prev_dim, &current_dim);

		/* Compute orthogonal projection matrix */
		gsl_matrix *projection_matrix = orthogonal_projection_matrix(window, window);

		gsl_matrix *projected_data = gsl_matrix_alloc(1, current_dim);

		/*  multiply this piece of data by the orthogonal projection matrix */
		compute_orthogonal_projection_query(projection_matrix, subquery, &projected_data, current_dim, window);

		subquery = gsl_matrix_alloc( 1, current_dim );

		/* update the query to the new projection */
		for( int q = 0; q < current_dim; q++ )
		{
			gsl_matrix_set( query_matrix, query_indx, q, gsl_matrix_get(projected_data, 0, q)); 
			gsl_matrix_set( subquery, 0, q, gsl_matrix_get(projected_data, 0, q));
		}

		query_indx++;
	}

	return query_matrix;
}

void compute_orthogonal_projection_query(gsl_matrix *projection_matrix, gsl_matrix *query, gsl_matrix **projected_data, int dim, int window )
{
	/* get projection matrix */
	gsl_vector * v = gsl_vector_alloc(window*window);
	gsl_matrix_get_row(v, projection_matrix, 0);

	/* put the projection matrix in the form window x window */
	gsl_matrix_view B = gsl_matrix_view_vector(v, window, window);

	int col_end_indx = window;
	int i = 0, row_begin_indx = 0, row_end_idx = 0;
	for (i = 0; i < dim; i++)
	{	
		int col_begin_indx = i*window;

		/* get first window of data from the database */
		gsl_matrix_view A = gsl_matrix_submatrix(query, row_begin_indx, col_begin_indx, 1, col_end_indx);

		/* allocate memory to hold the projected data */
		gsl_matrix *temp_projection = gsl_matrix_alloc(1, window);

		/* perform multiplication */
		gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, &A.matrix, &B.matrix, 0.0, temp_projection);

		/* compute norm */
		compute_norm(temp_projection, projected_data, 0, window, i);
	
		/* free everything */
		gsl_matrix_free(temp_projection);
	}
}

/* ======================================================================================
*
* build_query_to_select_data: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void compute_orthogonal_projection(gsl_matrix *projection_matrix, gsl_matrix *matrix_database, gsl_matrix **projected_data, int dim,
		int window, int chunk_indx, int number_chunks_to_read, long number_remaining_vectors)
{
	/* get projection matrix */
	gsl_vector * v = gsl_vector_alloc(window*window);
	gsl_matrix_get_row(v, projection_matrix, 0);

	/* put the projection matrix in the form window x window */
	gsl_matrix_view B = gsl_matrix_view_vector(v, window, window);

	int col_end_indx = window;
	int i = 0, j = 0, row_begin_indx = 0, row_end_idx = 0;
	for( j = 0; j < number_remaining_vectors; j++ )
	{
		for (i = 0; i < dim; i++)
		{		
			int col_begin_indx = i*window;

			/*  adjust the row index */
			//(chunk_indx + 1 == number_chunks_to_read) ? row_end_idx = number_remaining_vectors : row_end_idx = CHUNK_SIZE;

			/* get first window of data from the database */
			gsl_matrix_view A = gsl_matrix_submatrix(matrix_database, j, col_begin_indx, 1, col_end_indx);

			/* allocate memory to hold the projected data */
			gsl_matrix *temp_projection = gsl_matrix_alloc(1, window);

			/* perform multiplication */
			gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, &A.matrix, &B.matrix, 0.0, temp_projection);

			/* compute norm */
			compute_norm(temp_projection, projected_data, j, window, i);

			/* free everything */
			gsl_matrix_free(temp_projection);
		}
	}
}

/* ======================================================================================
*
* compute_num_vecs_to_load: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ====================================================================================== */
int compute_num_vecs_to_load( int current_chunk, int total_chunks )
{
	int remaining_vecs;

	if ((current_chunk == total_chunks - 1) && ( (TOTAL_VECTORS % CHUNK_SIZE) != 0 ) )
		remaining_vecs = abs(TOTAL_VECTORS - CHUNK_SIZE*total_chunks);
	else
		remaining_vecs = CHUNK_SIZE;

	return remaining_vecs;
}

/* ======================================================================================
*
* build_query_to_select_data: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
gsl_matrix *load_data_chunk(HDBC hdbc, int indx, int total_chunks, long num_vecs, int dims)
{
	gsl_matrix *matrix_database;

	/* retrieve chunk of data from database */
	matrix_database = sql_get_database(hdbc, num_vecs, dims, indx);

	/* print matrix for debuggin purposes */
	if ( DEBUG_OPTION > 1 )
		print_gsl_matrix(matrix_database, num_vecs, dims);

	return matrix_database;
}

/* ======================================================================================
*
* build_query_to_select_data: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
int compute_num_chunks()
{
	int chunks_to_read = TOTAL_VECTORS / CHUNK_SIZE;

	/* solution found, since for some reason, I cannot use the round function */
	if (TOTAL_VECTORS % CHUNK_SIZE != 0)
		chunks_to_read = TOTAL_VECTORS / CHUNK_SIZE + 1;

	return chunks_to_read;
}

/* ======================================================================================
*
* build_query_to_select_data: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void compute_dimensions(int proj_step, int *prev_dim, int *current_dim)
{
	if (proj_step == 0)
		return;
	
	(*prev_dim) = (*current_dim);
	(*current_dim) /= WINDOWS[proj_step];
}

/* ======================================================================================
*
* build_query_to_select_data: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ====================================================================================== */
FILE * open_file()
{
	/* if the projected database file already exists, remove it*/
	if (FILE * file = fopen(DATASET_PATH, "r")){
		fclose(file);
		remove(DATASET_PATH);
	}

	FILE *file = fopen(DATASET_PATH, "a+");

	/* print DATASET_PATH for debugging purposes */
	if (DEBUG_OPTION > 1)
		printf("\nDatabase Copy: vector file: %s\n", DATASET_PATH);

	return file;
}

/* ======================================================================================
*
* build_query_to_select_data: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ====================================================================================== */
						
void compute_norm(gsl_matrix *projected_data, gsl_matrix **projected_data_full, int row_indx, int window_size, int column_indx)
{
	int j = 0, k; 

	float norm = 0;

	/* if normalization_type == L2, then compute L2 norm */
	if (strcmp(NORM_TYPE, "L2") == 0)
	{
		gsl_vector_view row = gsl_matrix_row(projected_data, j);
		norm = gsl_blas_dnrm2(&row.vector);
	}

	/* if normalization_type == L2, then compute L1 norm */
	if (strcmp(NORM_TYPE, "L1") == 0)
	{
		gsl_vector_view row = gsl_matrix_row(projected_data, j);

		int l;
		for (l = 0; l < window_size; l++)
			norm += abs(gsl_matrix_get(projected_data, j, l));
	}

	gsl_matrix_set( (*projected_data_full), row_indx, column_indx, norm);
}

/* ======================================================================================
*
* update_table_name: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void update_table_name(int dims)
{
	free(DB_TABLE_NAME);

	DB_TABLE_NAME = (char *)malloc(sizeof(char)*(50 + strlen(DATASET_ROOT_NAME)));
	sprintf(DB_TABLE_NAME, "[dbo].[%s_%d_%s]", DATASET_ROOT_NAME, dims, NORM_TYPE);

}

/* ======================================================================================
*
* update_dataset_path: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void update_dataset_path(int dims)
{
	free(DATASET_PATH);

	DATASET_PATH = (char *)malloc(sizeof(char)*(50 + strlen(DATASET_ROOT_NAME) + strlen(ROOT_DIR)));
	sprintf(DATASET_PATH, "%s%s_%d_%d", ROOT_DIR, DATASET_ROOT_NAME, TOTAL_VECTORS, dims);
}

/* ======================================================================================
*
* update_dataset_path: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void update_dataset_root_name( )
{
	/* update the dataset name */
	free(DATASET_NAME);
	assign_dataset_name();

	printf("\n\nDATASET_NAME = %s\n\n", DATASET_NAME);

	/* update the toor name */
	char *temp = (char *)malloc(sizeof(char)*(strlen(DATASET_NAME) + 10));
	strcpy(temp, DATASET_NAME);

	/* read and store window values */
	char *token = strtok(temp, "_");

	DATASET_ROOT_NAME = (char *)malloc(sizeof(strlen(token)+1));
	sprintf(DATASET_ROOT_NAME, "%s", token);

	free(token);
}

/* ======================================================================================
*
* orthogonal_projection_matrix: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
gsl_matrix * orthogonal_projection_matrix(int dims1, int dims2)
{
	gsl_matrix *projection_matrix = gsl_matrix_alloc(dims1,dims2*dims2);

	int i,j;
	for (i = 0; i < dims1; i++ )
		for (j = 0; j < dims2*dims2; j++)
			gsl_matrix_set(projection_matrix, i, j, 0.5);
	
		return projection_matrix;
}

/* ======================================================================================
*
* write_projection_data: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void write_projection_data(FILE *file, gsl_matrix *projected_data, long num_rows, long num_columns)
{
	long i = 0;
	long j = 0;

	for (i = 0; i < num_rows; i++)
	{
		fprintf(file, "%f", gsl_matrix_get(projected_data, i, 0));

		for (j = 1; j < num_columns; j++)
			fprintf(file, " %f", gsl_matrix_get(projected_data, i, j));
		
		fprintf(file, "\n");
	}
}

/* ======================================================================================
*
* print_gsl_matrix: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void print_gsl_matrix(gsl_matrix *matrix, int dim1, int dim2)
{
	int i, j;
	for (i = 0; i < dim1; i++)
	{
		for (j = 0; j < dim2; j++)
		{
			printf("%f ", gsl_matrix_get(matrix, i, j));
		}
		printf("\n");
	}
}

/* ======================================================================================
*
* print_gsl_vector: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void print_gsl_vector(gsl_vector *vector, int dim)
{
	int i;
	for (i = 0; i < dim; i++)
		printf("%f ", gsl_vector_get(vector, i));
	printf("\n");
}