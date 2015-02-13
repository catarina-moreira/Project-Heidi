/*
 * auxiliaryFunctions.c
 *
 *  Created on: 29 de Set de 2012
 *      Author: twi
 */


#include "dataStructures.h"
#include "fileHandling.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#define L1_MODE "L1"

#define DIM "dim"
#define TXT ".txt"
#define QUERY_TIMES_FILE "queryTimes"

#define MKDIR "mkdir"

#define FORWARD_SLASH '\\'
#define DELIMITER ","

#define APPEND_MODE "a"

#define VECTOR "Vector"




/* ----------------------------------------------------------------------------
	save_database_path
---------------------------------------------------------------------------- */
int save_database_path( char ** filename, char * file_path )
{

	/* verify input */
	if( filename == NULL || file_path == NULL )
	{ printf( "\nError: save_database_path - invalid input\n" ); return 1; }



	(* filename ) = (char *) malloc( strlen( file_path ) + 1 * sizeof( char ) );


	/* copy the file path */
	strcpy( (* filename ), file_path );


	return 0;
}


/* ----------------------------------------------------------------------------
	find_working_directory
---------------------------------------------------------------------------- */
int find_working_directory( char * full_path, char ** directory_save_location, int debug_on )
{
	char * forward_slash_pointer = NULL;

	int forward_slash_index = -1;


	/* verify input */
	if( full_path == NULL || directory_save_location == NULL )
	{ puts( "\nError: find_working_directory - invalid input!\n" ); return 1; }



	if( debug_on > 0 )
	{ printf( "\nDatabase Path: %s\n\n", full_path ); }



	/* search for location of the final '/' */
	forward_slash_pointer = strrchr( full_path, FORWARD_SLASH );

	forward_slash_index = ( forward_slash_pointer - full_path ) + 1;


	/* printf( "\nfound at %i\n", forward_slash_index ); */


	(* directory_save_location ) = ( char * ) malloc( ( forward_slash_index + 1 ) * sizeof( char ) );


	strncpy( (* directory_save_location ), full_path, forward_slash_index );

	(* directory_save_location )[forward_slash_index] = '\0';


	if( debug_on > 0 )
	{ printf( "\nWorking Directory: %s\n\n", (* directory_save_location ) ); }


	return 0;
}


/* ----------------------------------------------------------------------------
	count_char_in_string
---------------------------------------------------------------------------- */
int count_char_in_string( char * string, char * char_to_count )
{
	int i = 0;
	int counter = 0;


	/* verify input */
	if( string == NULL || char_to_count == NULL || strlen( char_to_count ) > 1 )
	{ printf( "\nError: count_char_in_string - invalid input" ); return -1; }


	if( string[0] == '-' )
	{
		return 0;
	}


	for( i = 0, counter = 0; string[i] != '\0'; i++ )
	{
		if( string[i] == char_to_count[0] )
		{
			counter++;
		}
	}


	return counter + 1;
}


/* ----------------------------------------------------------------------------
	set_number_projections
---------------------------------------------------------------------------- */
int set_number_projections( int * number_projections_to_make, char * window_values, int debug_on )
{
	/* verify input */
	if( number_projections_to_make == NULL || window_values == NULL )
	{ printf( "\nError: count_char_in_string - invalid input\n" ); return 1; }



	/* find number of window values */
	(* number_projections_to_make) = count_char_in_string( window_values, DELIMITER );


	if( debug_on > 0 )
	{
		printf("\n# of Projections: %i\n", (* number_projections_to_make ) );
	}


	return 0;
}


/* ----------------------------------------------------------------------------
	set_window_values
---------------------------------------------------------------------------- */
int set_window_values( char * argv, int ** window_values, int number_projections_to_make, int debug_on )
{
	int i = 0;

	char * token;


	if( number_projections_to_make == 0 )
	{ return 0; }



	/* verify input */
	if( argv == NULL || window_values == NULL || number_projections_to_make < 1 )
	{ printf( "\nError: set_window_values - invalid input\n" ); return 1; }



	/* create structure to save window values */
	(* window_values) = ( int * ) malloc( number_projections_to_make * sizeof( int ) );



	/* read and store window values */

	token = strtok( argv, DELIMITER );


	for( i = 0; i < number_projections_to_make; i++ )
	{
		(* window_values)[i] = atoi( token );

		token = strtok( NULL, DELIMITER );
	}


	if( debug_on > 0 )
	{
		puts( "\nWindow Values: \n" );

		for( i = 0; i < number_projections_to_make; i++ )
		{ printf( "%i ", (* window_values)[i]); }
	}

	putchar('\n');


	return 0;
}


/* ----------------------------------------------------------------------------
	compute_current_previous_dimension
---------------------------------------------------------------------------- */
int compute_current_previous_dimension( int projection_step, int * previous_dimension, int * current_dimension, int vectors_original_lenght,
		int * window_values, int debug_on )
{
	/* verify input */
	if( projection_step < 0 || previous_dimension == NULL || current_dimension == NULL || vectors_original_lenght < 1 || window_values == NULL )
	{ printf( "\nError: compute_current_previous_dimension - invalid input\n" ); return 1; }



	if( projection_step == 0 )
	{
		(* previous_dimension) = vectors_original_lenght;

		(* current_dimension) = vectors_original_lenght / window_values[projection_step];
	}
	else
	{
		(* previous_dimension) = (* current_dimension);

		(* current_dimension) /= window_values[projection_step];
	}


	if( debug_on > 1 )
	{
		printf( "\nProjection step: %i\n", projection_step );
		printf( "Previous Dimension: %i\nCurrent Dimension: %i\n", (* previous_dimension), (* current_dimension) );
	}

	return 0;
}


/* ----------------------------------------------------------------------------
	compute_query_dimension
---------------------------------------------------------------------------- */
int compute_query_dimension( int query_step, int * current_dimension, int vectors_original_lenght,	int * window_values, int number_projections_to_make, int debug_on )
{
	int iteration_index = 0;


	/* verify input */
	if( query_step < 0 || current_dimension == NULL || vectors_original_lenght < 1 )
	{ printf( "\nError: compute_query_dimension - invalid input (Mark 1)\n" ); return 1; }


	if( window_values == NULL || number_projections_to_make < 1 )
	{
		if( number_projections_to_make > 0 )
		{ printf( "\nError: compute_query_dimension - invalid input (Mark 2)\n" ); return 1; }
	}



	(* current_dimension) = vectors_original_lenght;


	for( iteration_index = 0; iteration_index < (number_projections_to_make - query_step); iteration_index++ )
	{
		(* current_dimension) /= window_values[iteration_index];
	}


	if( debug_on > 1 )
	{
		printf( "\nProjection step: %i\n", query_step );
		printf( "Current Dimension: %i\n", (* current_dimension) );
	}

	return 0;
}


/* ----------------------------------------------------------------------------
	print_mean_values

	prints the mean values existing in the structure
---------------------------------------------------------------------------- */
void print_mean_values( double * mean_values, int number_of_values )
{
	int i = 0;

	puts( "\n--- Printing Mean Values ---\n" );

	for( i = 0; i < number_of_values; i++ )
	{
		printf( "#%i: %f\n", i + 1, mean_values[i] );
	}

	puts( "\n--- End of Mean Values ---\n\n" );
}


/* ----------------------------------------------------------------------------
	print_covariance_matrices
---------------------------------------------------------------------------- */
void print_covariance_matrices( double *** covariance_matrices, int number_existing_matrices, int matrix_size )
{
	int matrix_index = 0;
	int line_index = 0;
	int column_index = 0;


	puts( "\n--- Printing Covariance Matrices ---\n" );

	for( matrix_index = 0; matrix_index < number_existing_matrices; matrix_index++ )
	{
		printf( "\nCovariance Matrix#%i\n", matrix_index + 1 );

		for( line_index = 0; line_index < matrix_size; line_index++ )
		{
			for( column_index = 0; column_index < matrix_size; column_index++ )
			{
				printf( "%13f ", covariance_matrices[matrix_index][line_index][column_index] );
			}

			putchar( '\n' );
		}

		putchar( '\n' );
	}

	puts( "\n--- End of Covariance Matrices ---\n\n" );

}


/* ----------------------------------------------------------------------------
	print_eigen_values_vectors
---------------------------------------------------------------------------- */
void print_eigen_values_vectors( double ** eigen_values, double ** eigen_vectors, int number_existing_matrices, int window_size )
{
	int matrix_index = 0;
	int vector_index = 0;

	puts( "\n--- Printing Eigen Values and Eigen Vectors ---" );

	for( matrix_index = 0; matrix_index < number_existing_matrices; matrix_index ++ )
	{
		printf( "\nMatrix #%i", matrix_index + 1 );

		printf( "\nEigen Value: %g", eigen_values[matrix_index][0] );

		printf( "\nEigen Vector: " );

		for( vector_index = 0; vector_index < window_size; vector_index++ )
		{
			printf( "%f ", eigen_vectors[matrix_index][vector_index] );
		}

		putchar( '\n' );
	}

	puts( "\n--- End of Eigen Values and Eigen Vectors ---\n\n" );
}


/* ----------------------------------------------------------------------------
	print_projection_matrices
---------------------------------------------------------------------------- */
void print_projection_matrices( double *** projection_matrices, int number_existing_matrices, int matrix_size )
{
	int matrix_index = 0;
	int line_index = 0;
	int column_index = 0;


	puts( "\n--- Printing Projection Matrices ---\n" );

	for( matrix_index = 0; matrix_index < number_existing_matrices; matrix_index++ )
	{
		printf( "\nProjection Matrix#%i\n", matrix_index + 1 );

		for( line_index = 0; line_index < matrix_size; line_index++ )
		{
			for( column_index = 0; column_index < matrix_size; column_index++ )
			{
				printf( "%13f ", projection_matrices[matrix_index][line_index][column_index] );
			}

			putchar( '\n' );
		}

		putchar( '\n' );
	}

	puts( "\n--- End of Projection Matrices ---\n\n" );

}


/* ----------------------------------------------------------------------------
   compute_vector_length
---------------------------------------------------------------------------- */
int compute_vector_length( double * input_vector, int vector_size, char * computation_mode, double * vector_length )
{
	int i = 0;

	double vector_length_aux = 0;


	/* verify input */
	if( input_vector == NULL || vector_size < 1 || computation_mode == NULL || vector_length == NULL )
	{ puts( "\nError: compute_vector_length - invalid input!\n" ); return 1; }



	if( strcmp( computation_mode, L1_MODE ) == 0 )
	{
		/* compute the vector's length using L1 */

		for( i = 0; i < vector_size; i++ )
		{
			vector_length_aux += fabs( input_vector[i] );
		}
	}
	else
	{
		/* compute the vector's length using L2 */

		for( i = 0; i < vector_size; i++ )
		{
			vector_length_aux += pow( input_vector[i], 2.0 );
		}

		vector_length_aux = sqrt( vector_length_aux );
	}


	(* vector_length) = vector_length_aux;


	return 0;
}


/* ----------------------------------------------------------------------------
   compute_vector_distance
---------------------------------------------------------------------------- */
int compute_vector_distance( double component1, double component2, char * computation_mode, double * result, int vector_end )
{
	/* validate the input */
	if( computation_mode == NULL || result == NULL )
	{ puts( "\nERROR: compute_vector_distance - invalid input!\n" ); return 1; }



	if( strcmp( computation_mode, L1_MODE ) == 0 )
	{
		(* result) += fabs( component1 - component2 );
	}
	else
	{
		(* result) += pow( component1 - component2, 2.0 );

		if( vector_end )
		{
			(* result) = sqrt( (* result) );
		}
	}

	return 0;
}


/* ----------------------------------------------------------------------------
   absoluteValue
---------------------------------------------------------------------------- */
double absoluteValue( double value )
{
	return fabs( value );
}


/* ----------------------------------------------------------------------------
   count_number_digits
---------------------------------------------------------------------------- */
int count_number_digits( int number )
{
	int digits = 0;

	while( number != 0 )
	{
		digits++;
		number /= 10;
	}

	return digits;
}


/* ----------------------------------------------------------------------------
	create_dimension_folder
---------------------------------------------------------------------------- */
int create_dimension_folder( char * working_directory, int dimension, int debug_on )
{
	char * path;
	char * command;


	/* verify input */
	if( working_directory == NULL || dimension < 1 )
	{ printf( "\nError: create_dimension_folder - invalid input" ); return -1; }


	path = ( char * ) malloc( strlen( working_directory ) + strlen( DIM ) + count_number_digits( dimension ) + 1 * sizeof( char ) );


	sprintf( path, "%s%s%i", working_directory, DIM, dimension );


	command = ( char * ) malloc( strlen( MKDIR ) + strlen( path ) + 2 * sizeof( char ) );


	sprintf( command, "%s %s", MKDIR, path );


	if( debug_on > 1 )
	{ printf( "\nCommand: %s\n", command ); }


	system( command );


	free( path );
	free( command );


	return 0;
}


/* ----------------------------------------------------------------------------
	clear_mean_values_info
---------------------------------------------------------------------------- */
int clear_mean_values_info( double * mean_values )
{
	/* verify input */
	if( mean_values == NULL )
	{ printf( "\nError: clear_covariance_eigen_info - invalid input" ); return 1; }


	/* free the memory used for the mean values */
	free( mean_values );


	return 0;
}


/* ----------------------------------------------------------------------------
	clear_covariance_eigen_info
---------------------------------------------------------------------------- */
int clear_covariance_eigen_info( double *** covariance_matrices, double ** eigen_values, double ** eigen_vectors, int number_existing_matrices, int window_size )
{
	int i = 0;
	int j = 0;



	/* verify input */
	if( covariance_matrices == NULL || eigen_values == NULL || eigen_vectors == NULL || number_existing_matrices < 1 || window_size < 2 )
	{ printf( "\nError: clear_covariance_eigen_info - invalid input" ); return 1; }



	/* free the memory used for the covariance matrices */
	for( i = 0; i < number_existing_matrices; i++ )
	{
		for( j = 0; j < window_size; j++ )
		{
			free( covariance_matrices[i][j] );
		}

		free( covariance_matrices[i] );
	}

	free( covariance_matrices );


	/* free the memory used for the eigen values */
	for( i = 0; i < number_existing_matrices; i++ )
	{
		free( eigen_values[i] );
	}

	free( eigen_values );


	/* free the memory used for the eigen vectors */
	for( i = 0; i < number_existing_matrices; i++ )
	{
		free( eigen_vectors[i] );
	}

	free( eigen_vectors );


	return 0;
}


/* ----------------------------------------------------------------------------
	mark_start_of_process
---------------------------------------------------------------------------- */
int mark_start_of_process( clock_t * startTime )
{
	/* verify input */
	if( startTime == NULL )
	{ printf( "\nError: mark_start_of_process - invalid input" ); return 1; }


	/* mark the start time of calculations */
	(* startTime) = clock();

	return 0;
}


/* ----------------------------------------------------------------------------
	mark_end_of_process
---------------------------------------------------------------------------- */
int mark_end_of_process( clock_t startTime, char * working_directory, char * query_vector_number )
{
	int hours = 0;
	int minutes = 0;
	int seconds = 0;

	double miliseconds = 0.0;

	float time = 0.0;

	char * filename = NULL;

	FILE * output_file = NULL;

	clock_t endTime;
	clock_t differenceTime;


	// verify input
	if( working_directory == NULL || query_vector_number == NULL )
	{ printf( "\nError: mark_end_of_process - invalid input" ); return 1; }



	/* register the conclusion of the process */
	endTime = clock();


	/* calculate the time difference */
	differenceTime = endTime - startTime;


	time = ( (float) differenceTime ) / CLOCKS_PER_SEC;



	/*
	 * hours is given by dividing the number of seconds by 3600
	 * (3600 is the number of seconds in an hour)
	 */
	hours = time / 3600;


	/* remove the amount of hours */
	time -= (hours * 3600);


	/*
	 * minutes is given by dividing the remaining seconds by 60
	 */
	minutes = time / 60;


	/* remove the amount of minutes */
	time -= (minutes * 60);


	seconds = time;


	time -= seconds;

	

	miliseconds = time * 1000;


	printf( "\nElapsed time: %ih %im %is %.0fms\n", hours, minutes, seconds, miliseconds );



	// save run time to file

	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( QUERY_TIMES_FILE ) + strlen( VECTOR ) + strlen( query_vector_number ) + strlen( TXT ) + 1 ) * sizeof( char ) );


	// filename is something like: /working_directory/queryTimes.txt build the path
	sprintf( filename, "%s%s%s%s%s", working_directory, QUERY_TIMES_FILE, VECTOR, query_vector_number, TXT );



	if( open_file( filename, APPEND_MODE, &output_file ) )
	{ puts( "\nError: mark_end_of_process - problem with opening input file!\n" ); return 1; }



	free( filename );



	fprintf( output_file, "%i h %i m %i s %.0f ms\n", hours, minutes, seconds, miliseconds );



	close_file( output_file );


	return 0;
}


/* ----------------------------------------------------------------------------
	shutdown
---------------------------------------------------------------------------- */
int shutdown_program( int * window_values, char * database_path, char * working_directory, struct queryElement ** result_list, int number_projections_to_make )
{
	/* verify input */
	if( database_path == NULL || working_directory == NULL || number_projections_to_make < 0 )
	{ printf( "\nError: shutdown - invalid input (Mark 1)\n" ); return 1; }

	if( window_values == NULL && number_projections_to_make > 0 )
	{ printf( "\nError: shutdown - invalid input (Mark 2)\n" ); return 1; }


	/* free the memory used to save the window values */
	if( number_projections_to_make > 0 )
	{ free( window_values ); }

	/* free the memory used to save the database path */
	free( database_path );

	/* free the memory used to save the database path */
	free( working_directory );

	/* free the memory used to save the result list */
	delete_list( result_list );

	return 0;
}


/* ----------------------------------------------------------------------------
   auxiliaryFunctions.c
---------------------------------------------------------------------------- */
