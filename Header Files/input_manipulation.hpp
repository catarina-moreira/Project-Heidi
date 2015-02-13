/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* input_manipulation.hpp
* This file contains the definition of the functions that are used to map the user's input
* argument into the global variables that are required to run this program.
*
* Author: Catarina Moreira
* Personal Page: http://web.ist.utl.pt/~catarina.p.moreira/index.html
*
* Supervisor: Andreas Wichert
* Personal Page: http://web.ist.utl.pt/~andreas.wichert/
*
* ====================================================================================== */

#ifndef __Heidi__input_manipulation__
#define __Heidi__input_manipulation__

#include "constants.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
* assign_user_input: takes the input arguments and assigns them to each global variable
*                    their respective arguments
*
*      * user_input - string containing all the program's arguments
*/
void assign_user_input( char **user_input );

/* 
* assign_debug_option: assigns the user argument to the DEBUG global variable
*
*      * user_input - string containing the arguments of the main program
*/
void assign_debug_option( char *user_input );

/* 
* check_input:checks if the input of a given function is in the right format
*                    if the input format is ncorrect, the program will return an
*					 error and will exit
*
*		* input - string containing the arguments related to the window sizes
*		* function - string containing the name of the function that we are checking
*/
void check_input( char *input, char *function );

/* 
* assign_dataset_path: assigns the user argument to the DATASET_PATH global variable
*
*      * user_input - string containing the arguments of the main program
*/
void assign_dataset_path( char *user_input );

/*
* assign_query_path: assigns the user argument to the QUERY_PATH global variable
*
*      * user_input - string containing the arguments of the main program
*/
void assign_query_path(char *user_input);

/*
* assign_number_projections: computes the number of projections based on the number of
*                            WINDOWS and assigns the computed value to the NUM_PROJECTIONS
*                            variable
*
*      * user_input - string containing the arguments of the main program
*/
void assign_number_projections( char *user_input );

/* 
* assign_query: reads and retuens the query in the file QUERY_PATH. The query is an n-dimensional array.
*/
double *assign_query( );

/* 
* assign_window_values: reads the user input relative to the window sizes, tokenizes this input 
					and saves it in the WINDOWS global variabel
*
*      * input_window_values - string containing the arguments related to the window sizes
*/
void assign_window_values( char * input_window_values );

/* 
* assign_root_directory: extracts the root directory from the DATASET_PATH variable and
*						stores this information in the ROOT_DIR global variable
*/
void assign_root_directory( );

/* 
* assign_dataset_name: tokenize the DATASET_PATH global variable in order to extact the 
*					the dataset name out of it. THis information will be stored in the
*					DATASET_NAME global variable
*/
void assign_dataset_name( );

/* 
* assign_table_name: assigns the DB_TABLE_NAME variable. 
*					This function copies the DATASET_NAME to the variable DB_TABLE_NAME
*/
void assign_table_name();

/* 
* assign_dataset_dimensions: tokenize the DATASET_NAME global variable in order to extact the 
*					the DATASET_ROOT_NAME out of it. Extract the number of vectors in the dataset file
*					into the TOTAL_VECTORS global variable and extract the total number of dimensions 
					of the dataset file into the TOTAL_DIMENSIONS global variables
*/
void assign_dataset_dimensions( );

/*
* assign_norm_type: assigns the user argument to the NORM_TYPE global variable
*
*			* user_input - string containing the arguments of the main program
*/
void assign_norm_type( char *user_input );

/* 
* assign_projection_type: assigns the user argument to the PROJECTION_TYPE global variable
*
*      * user_input - string containing the arguments of the main program
*/
void assign_projection_type( char *user_input );

/* 
* assign_query_phase: assigns the user argument to the PERFORM_QUERY global variable
*
*		* user_input - string containing the arguments of the main program
*/
void assign_query_phase(char *user_input);

/* 
* assign_epsilon: assigns the user argument to the EPSILON global variable
*                       if it fails to allocate the memory, the program exits with error
*
*		* user_input - string containing the arguments of the main program
*/
void assign_epsilon( char *user_input);

/* 
* assign_index_phase: assigns the user argument to the PERFORM_INDEX_PHASE global variable
*
*		* user_input - string containing the arguments of the main program
*/
void assign_index_phase( char *user_input );

/*
* print_windows: displays the values that are contained in the WINDOWS variable.
*                used for debugging purposes
*/
void print_input_variables();

/* 
* print_input_variables: displays the values that are contained in the WINDOWS variable.
*                used for debugging purposes
*/
void print_windows( );

#endif /* defined(__Heidi__input_manipulation__) */
