/*
 * fileHandling.h
 *
 *  Created on: 29 de Jun de 2012
 *      Author: twi
 */

#ifndef FILEHANDLING_H_
#define FILEHANDLING_H_


#include <stdio.h>

/* ----------------------------------------------------------------------------
   open_file

   opens the specified file
---------------------------------------------------------------------------- */
int open_file( char * file_path, char * access_mode, FILE ** file );


/* ----------------------------------------------------------------------------
   close_file

   closes the specified file
---------------------------------------------------------------------------- */
int close_file( FILE * filename );


/* ----------------------------------------------------------------------------
   rewind_file

   rewinds the file pointer to the beginning of the document
---------------------------------------------------------------------------- */
FILE * rewind_file( FILE * data_file );


/* ----------------------------------------------------------------------------
   set_up_mean_values_structure

   sets up the structure to hold the mean values
---------------------------------------------------------------------------- */
int set_up_mean_values_structure( int vectors_lenght, double ** mean_values );


/* ----------------------------------------------------------------------------
   save_number_vectors_and_length

   opens the database file and saves the number of existing vectors and their
   length
---------------------------------------------------------------------------- */
int save_number_vectors_and_length( char * database_path, int * number_existing_vectors, int * vectors_lenght, int debug_on );


/* ----------------------------------------------------------------------------
   save_covariance_matrices

   save the information of the covariance matrices into a data file
---------------------------------------------------------------------------- */
int save_covariance_matrices( char * working_directory, double *** covariance_matrices, int number_existing_matrices, int window_size, int current_dimension,
		int debug_on );


/* ----------------------------------------------------------------------------
   save_eigen_vectors

   save the information of the eigen vectors into a data file
---------------------------------------------------------------------------- */
int save_eigen_vectors( char * working_directory, double ** eigen_vectors, int number_existing_vectors, int window_size, int current_dimension,
		int debug_on );


/* ----------------------------------------------------------------------------
   save_projection_matrices

   save the information of the projection matrices into a data file
---------------------------------------------------------------------------- */
int save_projection_matrices( char * working_directory, double *** projection_matrices, int number_existing_matrices, int window_size, int current_dimension,
		int debug_on );


/* ----------------------------------------------------------------------------
   read_projection_matrices

   reads and loads the information about projection matrices
---------------------------------------------------------------------------- */
int read_projection_matrices( FILE * load_file, double **** projection_matrices, int * number_existing_matrices, int window_size, int allocate_memory );


/* ----------------------------------------------------------------------------
   load_projection_matrices

   prepares the system to load projection matrices
---------------------------------------------------------------------------- */
int load_projection_matrices( char * working_directory, double **** projection_matrices, int * window_values, int current_dimension, int current_projection_step,
		int * number_existing_matrices, int debug_on );

/* ----------------------------------------------------------------------------
   save_projection_value

   saves the projection value into the given file
---------------------------------------------------------------------------- */
int save_projection_value( FILE * save_file, double projection_value, int end_of_vector, int writing_into_global_file, int eof );


/* ----------------------------------------------------------------------------
   load_epsilon_value

   reads the value of the epsilon from its file
---------------------------------------------------------------------------- */
int load_epsilon_value( char * working_directory, double * epsilon_value, int debug_on );


/* ----------------------------------------------------------------------------
   save_query_results

   saves the query results in a file
---------------------------------------------------------------------------- */
int save_query_results( char * working_directory, struct queryElement * result_list, char * computation_mode, char * query_vector_number, int debug_on );


/* ----------------------------------------------------------------------------
   save_distance_values

   saves only the distance values in a file
---------------------------------------------------------------------------- */
int save_distance_values( char * working_directory, struct queryElement * result_list, char * query_vector_number, int debug_on );


#endif

/* ----------------------------------------------------------------------------
   fileHandling.h
---------------------------------------------------------------------------- */
