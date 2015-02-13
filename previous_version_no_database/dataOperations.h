/*
 * dataOperations.h
 *
 *  Created on: 29 de Jun de 2012
 *      Author: twi
 */

#ifndef DATAOPERATIONS_H_
#define DATAOPERATIONS_H_

#include <stdio.h>


/* ----------------------------------------------------------------------------
   calculate_mean_value

   calculates the mean value based on the input values
---------------------------------------------------------------------------- */
double calculate_mean_value( double new_value, double current_mean, int iteration_step );


/* ----------------------------------------------------------------------------
   calculate_mean_values

   calculates the multiple mean values of the current dimension's database
---------------------------------------------------------------------------- */
int calculate_mean_values( char * database_path, char * working_directory, int number_existing_vectors, int vectors_lenght, double * mean_values, int first_projection_step,
		int previous_dimension, int debug_on );


/* ----------------------------------------------------------------------------
   calculate_covariance_matrix

   calculates the covariance matrix
---------------------------------------------------------------------------- */
int calculate_covariance_matrix( double * vector, int window_size, double ** covariance_matrix, int number_read_vectors, double * mean_values, int number_existing_vectors );


/* ----------------------------------------------------------------------------
   calculate_covariance_matrices

   calculates the multiple covariance matrices
---------------------------------------------------------------------------- */
int calculate_covariance_matrices( char * database_path, char * working_directory, int number_existing_vectors, int vectors_lenght, int window_size, double * mean_values, double **** covariance_matrices,
		int * number_existing_matrices, int debug_on, int first_projection_step, int previous_dimension );


/* ----------------------------------------------------------------------------
   calculate_matrix_eigen_values_and_vectors

   calculates the eigen values and eigen vectors of the given matrix
---------------------------------------------------------------------------- */
int calculate_matrix_eigen_values_and_vectors( double ** covariance_matrix, double * eigen_value, double * eigen_vector, int window_size, int debug_on );


/* ----------------------------------------------------------------------------
   calculate_eigen_values_and_vectors

   calculates the eigen values and eigen vectors of each existing matrix
---------------------------------------------------------------------------- */
int calculate_eigen_values_and_vectors( double *** covariance_matrices, double *** eigen_values, double *** eigen_vectors, int number_existing_matrices, int window_size, int debug_on );


/* ----------------------------------------------------------------------------
   calculate_projection_matrix

   calculates the projection matrix of the given vector
---------------------------------------------------------------------------- */
int calculate_projection_matrix( double * eigen_vector, double ** projection_matrix, int window_size );


/* ----------------------------------------------------------------------------
   calculate_projection_matrices

   calculates all the projection matrices
---------------------------------------------------------------------------- */
int calculate_projection_matrices( double ** eigen_vectors, double **** projection_matrices, int number_existing_matrices, int window_size, int debug_on );


/* ----------------------------------------------------------------------------
   project_database

   projects all the points in the given database
---------------------------------------------------------------------------- */
int project_database( char * working_directory, double *** projection_matrices, int * window_values, int vectors_original_length,
		int number_projections_to_make, int current_projection_step, char * computation_mode, int number_existing_vectors, char * database_path, int debug_on );


/* ----------------------------------------------------------------------------
   project_query_vector

   projects the given query vector
---------------------------------------------------------------------------- */
int project_query_vector( char * working_directory, double *** projection_matrices, int * window_values, int vectors_original_length, int number_projections_to_make,
		int current_projection_step, char * computation_mode, char * query_vector_number, int debug_on );


/* ----------------------------------------------------------------------------
   execute_query

   executes the query with the query vector and epsilon value
---------------------------------------------------------------------------- */
int execute_query( char * working_directory, int current_dimension, int number_existing_vectors, int current_query_step, char * computation_mode,
		struct queryElement ** query_result_list, double epsilon_value, int number_projections_to_make, char * database_path, char * query_vector_number, int debug_on );


/* ----------------------------------------------------------------------------
   clear_projection_matrices_memory

   clears the memory used to store the projection matrices
---------------------------------------------------------------------------- */
int clear_projection_matrices_memory( double *** projection_matrices, int number_existing_matrices, int window_size );


#endif

/* ----------------------------------------------------------------------------
   dataOperations.h
---------------------------------------------------------------------------- */
