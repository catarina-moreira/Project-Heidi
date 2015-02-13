/*
 * auxiliaryFunctions.h
 *
 *  Created on: 29 de Set de 2012
 *      Author: twi
 */

#ifndef AUXILIARYFUNCTIONS_H_
#define AUXILIARYFUNCTIONS_H_

#include "dataStructures.h"

#include <time.h>
#include <Windows.h>


/* ----------------------------------------------------------------------------
	save_database_path

	saves the location of the database file
---------------------------------------------------------------------------- */
int save_database_path( char ** filename, char * file_path );


/* ----------------------------------------------------------------------------
	find_working_directory

	given a path /../.../.../code/file.txt, it will return the working
	directory /../.../.../code/
---------------------------------------------------------------------------- */
int find_working_directory( char * full_path, char ** directory_save_location, int debug_on );


/* ----------------------------------------------------------------------------
	set_number_projections

	analyzes argv and sets the number of projections that will be performed
---------------------------------------------------------------------------- */
int set_number_projections( int * number_projections_to_make, char * window_values, int debug_on );


/* ----------------------------------------------------------------------------
	set_window_values

	analyzes argv and sets the values of the several windows in the structure
---------------------------------------------------------------------------- */
int set_window_values( char * argv, int ** window_values, int number_projections_to_make, int debug_on );


/* ----------------------------------------------------------------------------
	compute_current_previous_dimension

	computes the dimensions values in projection procedures
---------------------------------------------------------------------------- */
int compute_current_previous_dimension( int projection_step, int * previous_dimension, int * current_dimension, int vectors_original_lenght,
		int * window_values, int debug_on );


/* ----------------------------------------------------------------------------
	compute_query_dimension

	computes the dimensions value in query procedures
---------------------------------------------------------------------------- */
int compute_query_dimension( int query_step, int * current_dimension, int vectors_original_lenght,	int * window_values, int number_projections_to_make,
		int debug_on );


/* ----------------------------------------------------------------------------
	print_mean_values

	prints the mean values existing in the structure
---------------------------------------------------------------------------- */
void print_mean_values( double * mean_values, int number_of_values );


/* ----------------------------------------------------------------------------
	print_covariance_matrices

	prints the covariance matrices existing in the structure
---------------------------------------------------------------------------- */
void print_covariance_matrices( double *** covariance_matrices, int number_existing_matrices, int matrix_size );


/* ----------------------------------------------------------------------------
	print_eigen_values_vectors

	prints the eigen values and eigen vectors existing in the structure
---------------------------------------------------------------------------- */
void print_eigen_values_vectors( double ** eigen_values, double ** eigen_vectors, int number_existing_matrices, int window_size );


/* ----------------------------------------------------------------------------
	print_projection_matrices

	prints the projection matrices existing in the structure
---------------------------------------------------------------------------- */
void print_projection_matrices( double *** projection_matrices, int number_existing_matrices, int matrix_size );


/* ----------------------------------------------------------------------------
   compute_vector_length

   computes iteratively the length of the given vector, using L1 or L2 mode
---------------------------------------------------------------------------- */
int compute_vector_length( double * input_vector, int vector_size, char * computation_mode, double * vector_length );


/* ----------------------------------------------------------------------------
   compute_vector_distance

   computes iteratively the distance value between two vectors, using L1 or
   L2 mode
---------------------------------------------------------------------------- */
int compute_vector_distance( double component1, double component2, char * computation_mode, double * result, int vector_end );


/* ----------------------------------------------------------------------------
   absoluteValue

   returns the absolute value of the input value
---------------------------------------------------------------------------- */
double absoluteValue( double value );


/* ----------------------------------------------------------------------------
   count_number_digits

   counts the number of digita of the given int. eg: 10 -> 2
---------------------------------------------------------------------------- */
int count_number_digits( int number );


/* ----------------------------------------------------------------------------
	count_char_in_string

	counts the number of occurrences of a char in a string
---------------------------------------------------------------------------- */
int count_char_in_string( char * string, char * char_to_count );


/* ----------------------------------------------------------------------------
	create_dimension_folder

	in the given directory creates a folder called 'dimensionCURRENT_DIMENSION'
---------------------------------------------------------------------------- */
int create_dimension_folder( char * working_directory, int dimension, int debug_on );


/* ----------------------------------------------------------------------------
	clear_mean_values_info

	clears the data from the mean values
---------------------------------------------------------------------------- */
int clear_mean_values_info( double * mean_values );


/* ----------------------------------------------------------------------------
	clear_covariance_eigen_info

	clears the data from the covariance matrices, eigen values and vectors
---------------------------------------------------------------------------- */
int clear_covariance_eigen_info( double *** covariance_matrices, double ** eigen_values, double ** eigen_vectors, int number_existing_matrices, int window_size );


/* ----------------------------------------------------------------------------
	mark_start_of_process

	regists the time of the begining of the calculation process
---------------------------------------------------------------------------- */
int mark_start_of_process( clock_t * startTime );


/* ----------------------------------------------------------------------------
	mark_end_of_process

	stops the clock and calculates how much time has passed since the
	beginning of the process
---------------------------------------------------------------------------- */
int mark_end_of_process( clock_t startTime, char * working_directory, char * query_vector_number );


/* ----------------------------------------------------------------------------
	shutdown

	performs the necessary operations to a correct shutdown process
---------------------------------------------------------------------------- */
int shutdown_program( int * window_values, char * database_path, char * working_directory, struct queryElement ** result_list, int number_projections_to_make );


#endif

/* ----------------------------------------------------------------------------
   auxiliaryFunctions.h
---------------------------------------------------------------------------- */
