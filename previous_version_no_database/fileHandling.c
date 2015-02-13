/*
 * fileHandling.c
 *
 *  Created on: 29 de Jun de 2012
 *      Author: twi
 */


#include "auxiliaryFunctions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW "window"
#define TXT ".txt"
#define DIM "dim"
#define TXT ".txt"
#define EPSILON_VALUE "epsilonValue"
#define QUERY_RESULTS "queryResults"

#define READ_MODE "r"
#define WRITE_MODE "w"

#define COVARIANCE_MATRICES_FILENAME "covarianceMatrices"
#define PROJECTION_MATRICES_FILENAME "projectionMatrices"

#define NO_RESULTS "No vector within epsilon value range"

#define VECTOR "Vector"


/* ----------------------------------------------------------------------------
   open_file
---------------------------------------------------------------------------- */
int open_file( char * file_path, char * access_mode, FILE ** file )
{
	/* validate input */
	if( file_path == NULL || access_mode == NULL || file == NULL )
	{ puts( "\nERROR: open_file - invalid filename to open!\n" ); return 1; }


	/* open the file */
	(* file) = fopen( file_path, access_mode );

	if( (* file) == NULL )
	{
		printf( "\nERROR: open_file - problem while opening the data file:\n%s\n", file_path );

		return 1;
	}

	return 0;
}


/* ----------------------------------------------------------------------------
   close_file
---------------------------------------------------------------------------- */
int close_file( FILE * file )
{
	/* validate the input */
	if( file == NULL )
	{ puts( "\nERROR: open_file - invalid filename to close!\n" ); return 1; }


	/* close the file */
	return fclose( file );
}


/* ----------------------------------------------------------------------------
   rewind_file
---------------------------------------------------------------------------- */
FILE * rewind_file( FILE * data_file )
{
	/* validate the input */
	if( data_file == NULL )
	{ puts( "\nERROR: rewind_file - invalid input!\n" ); return NULL; }


	/* reset file position indicator to start of the file */
	rewind( data_file );


	/* discard first two values (number of existing vectors and original vectors length) */
	fscanf( data_file, "%*i %*i" );


	return data_file;
}


/* ----------------------------------------------------------------------------
   set_up_mean_values_structure
---------------------------------------------------------------------------- */
int set_up_mean_values_structure( int vectors_lenght, double ** mean_values )
{
	/* validate the input */
	if( vectors_lenght < 1 || mean_values == NULL )
	{ puts( "\nERROR: initialize_system - invalid input!\n" ); return 1; }



	/* create the structure to hold the mean values */
	(* mean_values ) = ( double * ) malloc( vectors_lenght * sizeof( double ));


	return 0;
}


/* ----------------------------------------------------------------------------
   save_number_vectors_and_length
---------------------------------------------------------------------------- */
int save_number_vectors_and_length( char * database_path, int * number_existing_vectors, int * vectors_lenght, int debug_on )
{
	FILE * database_file = NULL;


	/* validate the input */
	if( database_path == NULL || number_existing_vectors == NULL || vectors_lenght == NULL )
	{ puts( "\nERROR: save_number_vectors_and_length - invalid input!\n" ); return 1; }


	if( open_file( database_path, READ_MODE, &database_file ) )
	{ puts( "\nERROR: save_number_vectors_and_length - problem while opening file!\n" ); return 1; }



	/* read number of existing vectors */
	fscanf( database_file, "%i", &(* number_existing_vectors ) );

	/* read vectors' length */
	fscanf( database_file, "%i", &(* vectors_lenght ) );


	if( debug_on > 0 )
	{
		printf( "\nNr of existing vectors: %i\nDimension of each vector: %i\n\n", (* number_existing_vectors), (* vectors_lenght ) );
	}


	close_file( database_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   save_covariance_matrices
---------------------------------------------------------------------------- */
int save_covariance_matrices( char * working_directory, double *** covariance_matrices, int number_existing_matrices, int window_size, int current_dimension,
		int debug_on )
{
	int matrix_index = 0;
	int row_index = 0;
	int column_index = 0;

	char * filename = NULL;

	FILE * save_file = NULL;


	/* validate the input */
	if( working_directory == NULL || covariance_matrices == NULL || number_existing_matrices < 1 || window_size < 2 || current_dimension < 1 )
	{ puts( "\nERROR: save_covariance_matrices - invalid input!\n" ); return 1; }




	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( COVARIANCE_MATRICES_FILENAME ) +
			1 + strlen( WINDOW ) + count_number_digits( window_size ) + strlen( TXT ) + 1 ) * sizeof( char ) );


	/* build the path */
	sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, COVARIANCE_MATRICES_FILENAME, WINDOW, window_size, TXT );



	if( debug_on > 1 )
	{ printf( "\nsave_covariance_matrices - save file: %s\n", filename ); }




	/* open save file */
	if( ( save_file = fopen( filename, "w" ) ) == NULL )
	{ printf( "\nERROR: save_covariance_matrices - cannot create file %s!\n", filename ); return 1;	}


	free( filename );


	/* save the number of existing matrices */
	fprintf( save_file, "%i\n", number_existing_matrices );



	/* save the information */
	for( matrix_index = 0; matrix_index < number_existing_matrices; matrix_index++ )
	{
		if( debug_on > 0 )
		{
			if( (matrix_index + 1) % ( ( number_existing_matrices * window_size ) / 10 ) == 0 )
			{
				printf( "\nsave_covariance_matrices - matrix#%i\n", matrix_index + 1 );
			}
		}


		for( row_index = 0; row_index < window_size; row_index++ )
		{
			for( column_index = 0; column_index < window_size; column_index++ )
			{
				fprintf( save_file, "%f", covariance_matrices[matrix_index][row_index][column_index] );

				if( ( column_index + 1 ) < window_size )
				{ fprintf( save_file, " " ); }
			}

			if( ( row_index + 1 ) < window_size )
			{ fprintf( save_file, " " ); }
		}

		fprintf( save_file, "\n" );
	}


	/* close the file */
	fflush( save_file );
	fclose( save_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   save_eigen_vectors
---------------------------------------------------------------------------- */
int save_eigen_vectors( char * working_directory, double ** eigen_vectors, int number_existing_vectors, int window_size, int current_dimension,
		int debug_on )
{
	int vector_index = 0;
	int row_index = 0;

	char * filename = NULL;

	FILE * save_file = NULL;


	/* validate the input */
	if( working_directory == NULL || eigen_vectors == NULL || number_existing_vectors < 1 || window_size < 2 || current_dimension < 1 )
	{ puts( "\nERROR: save_eigen_vectors - invalid input!\n" ); return 1; }




	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( PROJECTION_MATRICES_FILENAME ) +
			1 + strlen( WINDOW ) + count_number_digits( window_size ) + strlen( TXT ) + 1 ) * sizeof( char ) );


	/* build the path */
	sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, PROJECTION_MATRICES_FILENAME, WINDOW, window_size, TXT );



	if( debug_on > 1 )
	{ printf( "\nsave_eigen_vectors - save file: %s\n", filename ); }




	/* open save file */
	if( ( save_file = fopen( filename, "w" ) ) == NULL )
	{ printf( "\nERROR: save_eigen_vectors - cannot create file %s!\n", filename ); return 1;	}


	free( filename );


	/* save the number of existing matrices */
	fprintf( save_file, "%i\n", number_existing_vectors );



	/* save the information */
	for( vector_index = 0; vector_index < number_existing_vectors; vector_index++ )
	{
		if( debug_on > 0 )
		{
			if( (vector_index + 1) % ( ( number_existing_vectors * window_size ) / 10 ) == 0 )
			{
				printf( "\nsave_eigen_vectors - eigenVector#%i\n", vector_index + 1 );
			}
		}


		for( row_index = 0; row_index < window_size; row_index++ )
		{
			fprintf( save_file, "%f", eigen_vectors[vector_index][row_index] );

			if( ( row_index + 1 ) < window_size )
			{ fprintf( save_file, " " ); }
		}


		if( vector_index < number_existing_vectors )
		{ fprintf( save_file, "\n" ); }
	}


	/* close the file */
	fflush( save_file );
	fclose( save_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   save_projection_matrices
---------------------------------------------------------------------------- */
int save_projection_matrices( char * working_directory, double *** projection_matrices, int number_existing_matrices, int window_size, int current_dimension,
		int debug_on )
{
	int matrix_index = 0;
	int row_index = 0;
	int column_index = 0;

	int matrix_marker = 0;

	char * filename = NULL;

	FILE * save_file = NULL;


	/* validate the input */
	if( working_directory == NULL || projection_matrices == NULL || number_existing_matrices < 1 || window_size < 2 || current_dimension < 1 )
	{ puts( "\nERROR: save_projection_matrices - invalid input!\n" ); return 1; }




	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( PROJECTION_MATRICES_FILENAME ) +
			1 + strlen( WINDOW ) + count_number_digits( window_size ) + strlen( TXT ) + 1 ) * sizeof( char ) );


	/* build the path */
	sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, PROJECTION_MATRICES_FILENAME, WINDOW, window_size, TXT );



	if( debug_on > 1 )
	{ printf( "\nsave_projection_matrices - save file: %s\n", filename ); }




	/* open save file */
	if( ( save_file = fopen( filename, "w" ) ) == NULL )
	{ printf( "\nERROR: save_projection_matrices - cannot create file %s!\n", filename ); return 1;	}


	free( filename );


	/* save the number of existing matrices */
	fprintf( save_file, "%i\n", number_existing_matrices );



	if( debug_on > 0 )
	{ matrix_marker = ( number_existing_matrices * window_size ) / 10; }



	/* save the information */
	for( matrix_index = 0; matrix_index < number_existing_matrices; matrix_index++ )
	{
		if( debug_on > 0 )
		{
			if( matrix_marker > 0 )
			{
				if( (matrix_index + 1) % matrix_marker == 0 )
				{ printf( "\nsave_projection_matrices - eigenVector#%i\n", matrix_index + 1 ); }
			}
		}


		for( row_index = 0; row_index < window_size; row_index++ )
		{
			for( column_index = 0; column_index < window_size; column_index++ )
			{
				fprintf( save_file, "%f", projection_matrices[matrix_index][row_index][column_index] );

				if( ( column_index + 1 ) < window_size )
				{ fprintf( save_file, " " ); }
			}

			if( ( row_index + 1 ) < window_size )
			{ fprintf( save_file, " " ); }
		}

		fprintf( save_file, "\n" );
	}


	/* close the file */
	fflush( save_file );
	fclose( save_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   read_projection_matrices
---------------------------------------------------------------------------- */
int read_projection_matrices( FILE * load_file, double **** projection_matrices, int * number_existing_matrices, int window_size, int allocate_memory )
{
	int i = 0;
	int j = 0;

	int matrix_index = 0;
	int row_index = 0;
	int column_index = 0;


	/* validate the input */
	if( load_file == NULL || number_existing_matrices == NULL || window_size < 2 )
	{ puts( "\nERROR: read_projection_matrices - invalid input!\n" ); return 1; }



	/* load the number of existing matrices */
	fscanf( load_file, "%i", &(* number_existing_matrices ) );



	if( allocate_memory )
	{
		/* create structure to hold the projection matrices */
		(* projection_matrices) = ( double *** ) malloc( (* number_existing_matrices ) * sizeof( double ** ) );

		for( i = 0; i < (* number_existing_matrices ); i++ )
		{
			(* projection_matrices)[i] = ( double ** ) malloc( window_size * sizeof( double * ) );

			for( j = 0; j < window_size; j++ )
			{
				(* projection_matrices)[i][j] = ( double * ) malloc( window_size * sizeof( double ) );
			}
		}
	}
	else
	{
		if( (* projection_matrices) == NULL )
		{ puts( "\nERROR: read_projection_matrices - (* projection_matrices) == NULL!\n" ); return 1; }
	}



	/* load the information */
	for( matrix_index = 0; matrix_index < (* number_existing_matrices ); matrix_index++ )
	{
		for( row_index = 0; row_index < window_size; row_index++ )
		{
			for( column_index = 0; column_index < window_size; column_index++ )
			{
				fscanf( load_file, "%lf", &(* projection_matrices)[matrix_index][row_index][column_index] );
			}
		}
	}


	return 0;
}


/* ----------------------------------------------------------------------------
   load_projection_matrices
---------------------------------------------------------------------------- */
int load_projection_matrices( char * working_directory, double **** projection_matrices, int * window_values, int current_dimension, int current_projection_step,
		int * number_existing_matrices, int debug_on )
{
	int allocate_memory = 0;


	char * filename = NULL;


	FILE * input_file = NULL;


	/* validate the input */
	if( working_directory == NULL || projection_matrices == NULL || window_values == NULL || current_dimension < 2 )
	{ puts( "\nERROR: load_projection_matrices - invalid input!\n" ); return 1; }



	/*
	 * filename is something like: /working_directory/dimCURRENT_DIMENSION/projectionMatrices_windowWINDOW_VALUE.txt
	 */
	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( PROJECTION_MATRICES_FILENAME ) +
			1 + strlen( WINDOW ) + count_number_digits( window_values[ current_projection_step ] ) + strlen( TXT ) + 1 ) * sizeof( char ) );


	/* build the path */
	sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, PROJECTION_MATRICES_FILENAME, WINDOW, window_values[ current_projection_step ], TXT );



	if( debug_on > 1 )
	{ printf( "\nload_projection_matrices - input file: %s\n", filename ); }



	if( open_file( filename, READ_MODE, &input_file ) )
	{ puts( "\nError: load_projection_matrices - problem with opening input file!\n" ); return 1; }


	free( filename );



	if( current_projection_step == 0 )
	{
		allocate_memory = 1;
	}
	else
	{
		if( window_values[ current_projection_step - 1 ] == window_values[ current_projection_step ] )
		{
			allocate_memory = 0;
		}
		else
		{
			allocate_memory = 1;
		}
	}



	/* load the projection matrices */
	if( read_projection_matrices( input_file, projection_matrices, number_existing_matrices, window_values[ current_projection_step ], allocate_memory ) )
	{ puts( "Error: load_projection_matrices - error in read_projection_matrices!\n" ); return 1; }



	close_file( input_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   save_projection_value

   . this function is called when projecting the vector values into individual
   files, and also when projecting into the global file. When we are projecting
   into a individual file, when we reach the end of a vector, no action is
   needed. When we are writing into the global file, when the end of a vector
   is reached, it is necessary to change line for the next vector. The
   writing_into_global_file variable is used to make this distinction
---------------------------------------------------------------------------- */
int save_projection_value( FILE * save_file, double projection_value, int end_of_vector, int writing_into_global_file, int eof )
{

	/* validate the input */
	if( save_file == NULL )
	{ puts( "\nERROR: save_projected_window - invalid input!\n" ); return 1; }



	if( fprintf( save_file, "%f", projection_value ) < 0 )
	{
		puts( "\nERROR: save_projection_value - error while writing projection value to file!\n" );
		return 1;
	}


	if( !end_of_vector )
	{
		fprintf( save_file, " " );
	}
	else
	{
		if( writing_into_global_file && !eof )
		{
			fprintf( save_file, "\n" );
		}
	}


	return 0;
}


/* ----------------------------------------------------------------------------
   load_epsilon_value
---------------------------------------------------------------------------- */
int load_epsilon_value( char * working_directory, double * epsilon_value, int debug_on )
{
	char * filename = NULL;

	FILE * data_file = NULL;


	/* validate the input */
	if( working_directory == NULL || epsilon_value == NULL )
	{ puts( "\nERROR: load_epsilon_value - invalid input!\n" ); return 1; }



	/*
	 * the file path is something like: /working_directory/epsilonValue.txt
	 */

	filename = ( char * ) malloc( strlen( working_directory ) + strlen( EPSILON_VALUE ) + strlen( TXT ) + 1 * sizeof( char ) );


	/* build the path */
	sprintf( filename, "%s%s%s", working_directory, EPSILON_VALUE, TXT );


	if( debug_on > 1 )
	{ printf( "\nEpsilon Value file: %s\n", filename ); }


	if( open_file( filename, READ_MODE, &data_file ) )
	{ puts( "\nError: load_epsilon_value - problem with opening epsilon file!\n" ); return 1; }


	free( filename );


	fscanf( data_file, "%lf", &(* epsilon_value) );


	close_file( data_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   save_query_results
---------------------------------------------------------------------------- */
int save_query_results( char * working_directory, struct queryElement * result_list, char * computation_mode, char * query_vector_number, int debug_on )
{
	char * filename = NULL;

	FILE * save_file = NULL;

	struct queryElement * current_element = NULL;


	/* validate the input */
	if( working_directory == NULL || computation_mode == NULL || query_vector_number == NULL )
	{ puts( "\nERROR: save_query_results - invalid input!\n" ); return 1; }


	/*
	 * the file path is something like: /working_directory/queryResultsVector1.txt
	 */

	filename = ( char * ) malloc( strlen( working_directory ) + strlen( QUERY_RESULTS ) + strlen( VECTOR ) + strlen( query_vector_number ) +
			strlen( computation_mode ) + strlen( TXT ) + 1 * sizeof( char ) );


	/* build the path */
	sprintf( filename, "%s%s%s%s%s%s", working_directory, QUERY_RESULTS, VECTOR, query_vector_number, computation_mode, TXT );


	if( debug_on > 1 )
	{ printf( "\nQuery Results savefile: %s\n", filename ); }


	if( open_file( filename, WRITE_MODE, &save_file ) )
	{ puts( "\nError: save_query_results - problem with creating query results file!\n" ); return 1; }


	free( filename );




	if( result_list == NULL )
	{
		fprintf( save_file, "%s", NO_RESULTS );
	}


	current_element = result_list;

	while( current_element != NULL )
	{
		fprintf( save_file, "Vector #%i\tDistance Value: %f", current_element->vector_number + 1, current_element->distance_value );


		if( current_element->next_element != NULL )
		{
			fprintf( save_file, "\n" );
		}


		current_element = current_element->next_element;
	}


	close_file( save_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   save_distance_values
---------------------------------------------------------------------------- */
int save_distance_values( char * working_directory, struct queryElement * result_list, char * query_vector_number, int debug_on )
{
	char * filename = NULL;

	FILE * save_file = NULL;

	struct queryElement * current_element = NULL;


	/* validate the input */
	if( working_directory == NULL || query_vector_number == NULL )
	{ puts( "\nERROR: save_distance_values - invalid input!\n" ); return 1; }


	/*
	 * the file path is something like: /working_directory/queryResults.txt
	 */

	filename = ( char * ) malloc( strlen( working_directory ) + strlen( QUERY_RESULTS ) + strlen( VECTOR ) + strlen( query_vector_number ) + strlen( TXT ) + 1 * sizeof( char ) );


	/* build the path */
	sprintf( filename, "%s%s%s%s%s", working_directory, QUERY_RESULTS, VECTOR, query_vector_number, TXT );


	if( debug_on > 1 )
	{ printf( "\nQuery Results savefile: %s\n", filename ); }


	if( open_file( filename, WRITE_MODE, &save_file ) )
	{ puts( "\nError: save_query_results - problem with creating query results file!\n" ); return 1; }


	free( filename );




	if( result_list == NULL )
	{
		fprintf( save_file, "%s", NO_RESULTS );
	}


	current_element = result_list;

	while( current_element != NULL )
	{
		fprintf( save_file, "%f", current_element->distance_value );


		if( current_element->next_element != NULL )
		{
			fprintf( save_file, "\n" );
		}


		current_element = current_element->next_element;
	}


	close_file( save_file );


	return 0;
}


/* ----------------------------------------------------------------------------
   fileHandling.c
---------------------------------------------------------------------------- */
