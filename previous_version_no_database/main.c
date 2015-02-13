/* ----------------------------------------------------------------------------

  main.c

---------------------------------------------------------------------------- */

#include "auxiliaryFunctions.h"
#include "dataOperations.h"
#include "dataStructures.h"
#include "fileHandling.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>



/* ----------------------------------------------------------------------------
   constants
---------------------------------------------------------------------------- */

#define DEBUG_OPTION_INDEX 1
#define DATABASE_PATH_INDEX 2
#define WINDOW_VALUES_INDEX 3
#define COMPUTATION_MODE_INDEX 4
#define PROJECT_DATABASE_INDEX 5
#define EXECUTE_QUERY_INDEX 6
#define SAVE_COVARIANCE_INDEX 7
#define VECTOR_NUMBER_INDEX 8





/* ----------------------------------------------------------------------------

   	main

   	agrv
   	[1] - 0/1/2 debug level. 0 - off; 1 - normal output; 2 - high output
   	[2] - database file path
   	[3] - window values eg: (2,2,3)
   	[4] - computation mode: L1 or L2
   	[5] - 0/1 compute projection matrices and project database
   	[6] - 0/1 execute query
   	[7] - 0/1 do not save/save the covariance matrices data
   	[8] - query vector number (in order to be possible to run several queries scripts)

   	run command:

   	./project.exe 2 /home/twi/project/data/data10/myData10.txt 2 L2 1 1 0 1
   	./project.exe 2 /home/twi/project/data/data12/myData12.txt 2,2 L2 1 0 0 1
   	./project.exe 2 /home/twi/project/data/data36/myData36.txt 2,3,2 L2 1 0 0 1
   	./project.exe 2 /home/twi/project/data/data36/myData36.txt -1 L2 0 1 0 1
   	./project.exe 2 /home/twi/project/data/data80/myData80.txt 2,2,2 L2 1 1 0 1
   	./project.exe 2 /home/twi/project/data/data360/myData360.txt 2,3,2,3 L2 1 1 0 1
   	./project.exe 1 /home/twi/project/data/data960/myData960.txt 2,2,2,2,2,3,2 L2 1 1 0 1

---------------------------------------------------------------------------- */

int main( int argc, char * argv[] )
{
	/* number of projections steps that will be executed */
	int number_projections_to_make = 0;

	/* structure to hold the window values to perform database projection */
	int * window_values = NULL;

	/* number of existing vectors */
	int number_existing_vectors = 0;

	/* length of each original vector */
	int vectors_original_lenght = 0;

	int current_projection_step = 0;

	int current_query_step = 0;

	int previous_dimension = 0;

	int current_dimension = 0;

	/* says in debug is on or off */
	int debug_option = 0;

	/* number of existing covariance matrices */
	int number_existing_matrices = 0;


	/* path where the database file is located */
	char * database_path = NULL;

	/* the working directory where files are loaded and saved */
	char * working_directory = NULL;


	/* structure to store the mean values */
	double * mean_values = NULL;

	/* structure to hold the covariance matrices */
	double *** covariance_matrices = NULL;

	/* structure to store the eigen values */
	double ** eigen_values = NULL;

	/* structure to store the eigen vectors */
	double ** eigen_vectors = NULL;

	/* structure to hold the projection matrices */
	double *** projection_matrices = NULL;

	double epsilon_value = 0.0;

	/* list with the vectors within epsilon range from query vector */
	struct queryElement * query_result_list = NULL;


	/* variable to hold the time of start of the process */
	clock_t startTime;




	/* mark the start time of calculations */
	if( mark_start_of_process( &startTime ) )
	{ puts( "\nERROR: main - problem in mark_start_of_process\n!" ); return 1; }







	/* save debug settings */
	debug_option = atoi( argv[DEBUG_OPTION_INDEX] );



	/* store the path where the database file is located */
	if( save_database_path( &database_path, argv[DATABASE_PATH_INDEX] ) )
	{ puts( "\nERROR: main - problem in save_database_path\n!" ); return 1; }



	/* find the location of the working directory in the given path */
	if( find_working_directory( database_path, &working_directory, debug_option ) )
	{ puts( "\nERROR: main - problem in find_working_directory function!" ); return 1; }



	/* find and save how many projection steps will be performed */
	if( set_number_projections( &number_projections_to_make, argv[WINDOW_VALUES_INDEX], debug_option ) )
	{ puts( "\nError: main - error in set_number_projections!\n" ); return 1; }



	/* analyze argv and store the window values */
	if( set_window_values( argv[WINDOW_VALUES_INDEX], &window_values, number_projections_to_make, debug_option ) )
	{ puts( "\nError: main - error in set_window_values!\n" ); return 1; }



	/* save the number of existing vectors and their length */
	if( save_number_vectors_and_length( database_path, &number_existing_vectors, &vectors_original_lenght, debug_option ) )
	{ puts( "\nError: main - error in save_number_vectors_and_length!\n" ); return 1; }




	/* if user wants compute projection matrices and project the database */
	if( atoi( argv[PROJECT_DATABASE_INDEX] ) )
	{
		if( number_projections_to_make > 0 )
		{
			/* create the folder in which the data of original dimension will be saved */
			if( create_dimension_folder( working_directory, vectors_original_lenght, debug_option ) )
			{ puts( "\nError: main - error in create_dimension_folder (Mark 1)!\n" ); return 1; }



			for( current_projection_step = 0; current_projection_step < number_projections_to_make; current_projection_step++ )
			{
				/* calculate the current and the previous database dimension */
				if( compute_current_previous_dimension( current_projection_step, &previous_dimension, &current_dimension, vectors_original_lenght, window_values, debug_option ) )
				{ puts( "\nError: main - error in compute_current_previous_dimension (Mark 1)!\n" ); return 1; }



				/* create the structure to hold the mean values */
				if( set_up_mean_values_structure( previous_dimension, &mean_values ) )
				{ puts( "\nError: main - error in initialize_system!\n" ); return 1; }



				/* calculate the mean values */
				if( calculate_mean_values( database_path, working_directory, number_existing_vectors, previous_dimension, mean_values, current_projection_step == 0 ? 1 : 0,
						previous_dimension, debug_option ) )
				{ puts( "\nError: main - error in calculate_mean_values!\n" ); return 1; }



				if( debug_option > 1 )
				{ print_mean_values( mean_values, previous_dimension ); }



				/* calculate covariance matrices */
				if( calculate_covariance_matrices( database_path, working_directory, number_existing_vectors, previous_dimension, window_values[current_projection_step], mean_values,
						&covariance_matrices, &number_existing_matrices, debug_option, current_projection_step == 0 ? 1 : 0, previous_dimension ) )
				{ puts( "\nError: main - error in calculate_covariance_matrices!\n" ); return 1; }



				/* clear the memory used for the mean values */
				if( clear_mean_values_info( mean_values ) )
				{ puts( "\nError: main - error in clear_mean_values_info!\n" ); return 1; }



				if( debug_option > 1 )
				{ print_covariance_matrices( covariance_matrices, number_existing_matrices, window_values[current_projection_step] ); }



				/* calculate the eigen values & eigen vectors */
				if( calculate_eigen_values_and_vectors( covariance_matrices, &eigen_values, &eigen_vectors, number_existing_matrices, window_values[current_projection_step],
						debug_option ) )
				{ puts( "\nError: main - error in calculate_eigen_values_and_vectors!\n" ); return 1; }



				if( debug_option > 1 )
				{ print_eigen_values_vectors( eigen_values, eigen_vectors, number_existing_matrices, window_values[current_projection_step] ); }



				/* calculate the projection matrices */
				if( calculate_projection_matrices( eigen_vectors, &projection_matrices, number_existing_matrices, window_values[current_projection_step], debug_option ) )
				{ puts( "\nError: main - error in calculate_projection_matrices!\n" ); return 1; }



				if( debug_option > 1 )
				{ print_projection_matrices( projection_matrices, number_existing_matrices, window_values[current_projection_step] ); }



				/* create the folder in which the data of this dimension will be saved */
				if( create_dimension_folder( working_directory, current_dimension, debug_option ) )
				{ puts( "\nError: main - error in create_dimension_folder (Mark 2)!\n" ); return 1; }



				if( atoi( argv[SAVE_COVARIANCE_INDEX] ) )
				{
					/* save the covariance matrices' data */
					if( save_covariance_matrices( working_directory, covariance_matrices, number_existing_matrices, window_values[current_projection_step], current_dimension,
							debug_option ) )
					{ puts( "\nError: main - error in save_covariance_matrices!\n" ); return 1; }
				}



				/* save the projection matrices' data */
				if( save_projection_matrices( working_directory, projection_matrices, number_existing_matrices, window_values[current_projection_step], current_dimension,
						debug_option ) )
				{ puts( "\nError: main - error in save_projection_matrices!\n" ); return 1; }



				/* clear the memory used for the covariance matrices, eigen values and eigen vectors */
				if( clear_covariance_eigen_info( covariance_matrices, eigen_values, eigen_vectors, number_existing_matrices, window_values[current_projection_step] ) )
				{ puts( "\nError: main - error in save_projection_matrices!\n" ); return 1; }



				/* project the database */
				if( project_database( working_directory, projection_matrices, window_values, vectors_original_lenght, number_projections_to_make, current_projection_step,
						argv[COMPUTATION_MODE_INDEX], number_existing_vectors, database_path, debug_option ) )
				{ puts( "\nError: main - error in project_database!\n" ); return 1; }



				/* clear the memory used for the projection matrices */
				if( clear_projection_matrices_memory( projection_matrices, number_existing_matrices, window_values[current_projection_step] ) )
				{ puts( "\nError: main - error in clear_projection_matrices_memory (Mark 1)!\n" ); return 1; }
			}
		}
		else
		{
			puts( "\nError: Bad execute command!Option to project database should be 0 since the number of projections is 0!\n" );
		}
	}


	/* if user wants to execute query */
	if( atoi( argv[EXECUTE_QUERY_INDEX] ) )
	{
		if( debug_option > 1 )
		{ puts( "\n--- Main - Project Query Vector Start ---\n" ); }

		if( argv[VECTOR_NUMBER_INDEX] != NULL )
		{
			for( current_projection_step = 0; current_projection_step < number_projections_to_make; current_projection_step++ )
			{
				/* calculate the current and the previous database dimension */
				if( compute_current_previous_dimension( current_projection_step, &previous_dimension, &current_dimension, vectors_original_lenght, window_values, debug_option ) )
				{ puts( "\nError: main - error in compute_current_previous_dimension (Mark 2)!\n" ); return 1; }



				/* load projection matrices for this dimension */
				if( load_projection_matrices( working_directory, &projection_matrices, window_values, current_dimension, current_projection_step, &number_existing_matrices,
						debug_option ) )
				{ puts( "\nError: main - error in load_projection_matrices!\n" ); return 1; }



				if( debug_option > 1 )
				{ print_projection_matrices( projection_matrices, number_existing_matrices, window_values[current_projection_step] ); }



				/* project the query vector */
				if( project_query_vector( working_directory, projection_matrices, window_values, vectors_original_lenght, number_projections_to_make, current_projection_step,
						argv[COMPUTATION_MODE_INDEX], argv[VECTOR_NUMBER_INDEX], debug_option ) )
				{ puts( "\nError: main - error in project_query_vector!\n" ); return 1; }




				/*
				 * if the window value of the next iteration is not equal to the one in this iteration, it is necessary to clear the memory used for the projection matrices
				 */
				if( current_projection_step == (number_projections_to_make - 1) || window_values[ current_projection_step ] != window_values[ current_projection_step + 1 ] )
				{
					if( debug_option > 1 )
					{ puts( "\nclearing projection matrices memory!\n" ); }



					/* clear the memory used for the projection matrices */
					if( clear_projection_matrices_memory( projection_matrices, number_existing_matrices, window_values[current_projection_step] ) )
					{ puts( "\nError: main - error in clear_projection_matrices_memory (Mark 2)!\n" ); return 1; }
				}
			}



			if( debug_option > 1 )
			{ puts( "\n--- Main - Project Query Vector End ---\n" ); }



			/* read epsilon value */
			if( load_epsilon_value( working_directory, &epsilon_value, debug_option ) )
			{ puts( "\nError: main - error in load_epsilon_value!\n" ); return 1; }



			if( debug_option > 0 )
			{ printf( "\nmain - loaded epsilon value: %f\n", epsilon_value ); }



			for( current_query_step = 0; current_query_step <= number_projections_to_make; current_query_step++ )
			{
				/* calculate the current database dimension */
				if( compute_query_dimension( current_query_step, &current_dimension, vectors_original_lenght, window_values, number_projections_to_make, debug_option ) )
				{ puts( "\nError: main - error in compute_query_dimension!\n" ); return 1; }



				/* check vector distances. in each dimension, check which vectors are within epsilon distance. in the end it should have formed a result list */
				if( execute_query( working_directory, current_dimension, number_existing_vectors, current_query_step, argv[COMPUTATION_MODE_INDEX],
						&query_result_list, epsilon_value, number_projections_to_make, database_path, argv[VECTOR_NUMBER_INDEX], debug_option ) )
				{ puts( "\nError: main - error in execute_query!\n" ); return 1; }


				if( query_result_list == NULL )
				{
					/* the result list is empty, no vector was within epsilon range */
					break;
				}
			}


			/* save the result list to a file  */
			if( save_query_results( working_directory, query_result_list, argv[COMPUTATION_MODE_INDEX], argv[VECTOR_NUMBER_INDEX], debug_option ) )
			{ puts( "\nError: main - error in save_query_results!\n" ); return 1; }
		}
		else
		{
			puts( "\nERROR: Main - Query Vector Number missing!\n" );
		}
	}



	/* mark the end of the process */
	mark_end_of_process( startTime, working_directory, argv[VECTOR_NUMBER_INDEX] );



	/* shutdown */
	shutdown_program( window_values, database_path, working_directory, &query_result_list, number_projections_to_make );



	return EXIT_SUCCESS;
}
