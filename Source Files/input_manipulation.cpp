/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* input_manipulation.cpp
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

#include "input_manipulation.hpp"
#include "query.hpp"

/* ======================================================================================
*
* assign_user_input: takes the input arguments and assigns them to each global variable
*                    their respective arguments
*
*      * user_input - string containing all the program's arguments
*
* ======================================================================================
*/
void assign_user_input(char **user_input)
{
	/* set DEBUG_OPTION variable */
	assign_debug_option(user_input[DEBUG_INDX]);

	/* set NORM_TYPE variable */
	assign_norm_type(user_input[COMP_MODE_INDX]);

	/* set DATASET_PATH variable */
	assign_dataset_path(user_input[DATASET_PATH_INDX]);

	/* set QUERY_PATH variable */
	assign_query_path( user_input[QUERY_PATH_INDX] );

	/* compute the number of projections and set NUM_PROJECTIONS variable */
	/* the number of projections is given by the number of dimensions in the WINDOWS variable */
	assign_number_projections(user_input[WINDOW_VALUES_INDX]);

	/* set WINDOWS variable */
	assign_window_values(user_input[WINDOW_VALUES_INDX]);

	/* set DATASET_NAME and DB_TABLE_NAME variables */
	assign_dataset_name();

	/* set TOTAL_VECTORS and TOTAL_DIMENSIONS variables */
	assign_dataset_dimensions();

	/* set DATASET_NAME and DB_TABLE_NAME variables */
	assign_table_name();

	/* set ROOT_DIR variable */
	assign_root_directory( );

	/* set PROJECTION_TYPE variable */
	assign_projection_type(user_input[PROJECTION_INDX]);

	/* set PERFORM_INDEX_PHASE variable */
	assign_index_phase(user_input[EXEC_INDEXING_INDX]);

	/* set PERFORM_QUERY variable */
	assign_query_phase(user_input[EXEC_QUERY_INDX]);

	/* set epsilon threshold */
	assign_epsilon(user_input[EPSILON_INDX]);

	/* display reults if DEBUG_OPTION variable is set */
	if (DEBUG_OPTION > 1) print_input_variables( );
}

/* ======================================================================================
*
* assign_debug_option: assigns the user argument to the DEBUG global variable
*
*      * user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_debug_option(char *user_input)
{
	check_input(user_input, (char *)"assign_debug_option");

	DEBUG_OPTION = atoi(user_input);
}

/* ======================================================================================
*
* check_input:checks if the input of a given function is in the right format
*                    if the input format is incorrect, the program will return an
*					 error and will exit
*
*		* input - string containing the arguments related to the window sizes
*		* function - string containing the name of the function that we are checking
*
* ======================================================================================
*/
void check_input(char *input, char *function)
{
	if (input != NULL)
		return;
	
	/* if the input is not correct, then return error and exit the application */
	printf("\n[%s] Error: invalid input\n", function);
	exit(-1);
}

/* ======================================================================================
*
* assign_dataset_path: assigns the user argument to the DATASET_PATH global variable
*
*      * user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_dataset_path(char *user_input)
{
	check_input(user_input, (char *)"assign_dataset_path");

	/* allocate memory for dataset_path variable */
	DATASET_PATH = (char *)malloc(sizeof(char)*(strlen(user_input) + 1));

	/* copy input string to dataset_path */
	strcpy(DATASET_PATH, user_input);
}

/* ======================================================================================
*
* assign_query_path: assigns the user argument to the QUERY_PATH global variable
*
*      * user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_query_path(char *user_input)
{
	check_input(user_input, (char *)"assign_query_path");

	/* allocate memory for dataset_path variable */
	QUERY_PATH = (char *)malloc(sizeof(char)*(strlen(user_input) + 1));

	/* copy input string to dataset_path */
	strcpy(QUERY_PATH, user_input);
}


/* ======================================================================================
*
* assign_number_projections: computes the number of projections based on the number of
*                            WINDOWS and assigns the computed value to the NUM_PROJECTIONS
*                            variable
*
*      * user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_number_projections(char *user_input)
{
	/* copy input string to token */
	char *temp = (char *)malloc(sizeof(char)*(strlen(user_input) + 1));
	strcpy(temp, user_input);

	/* get the first token */
	char *token = strtok(temp, ",");

	/* if the number of projections is less than one, return error */
	check_input(token, (char *)"assign_number_projections");

	/* walk through other tokens */
	for (NUM_PROJECTIONS = 0; token != NULL; token = strtok(NULL, ","))
		NUM_PROJECTIONS++;

	/* clear memory researved for token */
	free(temp);
}

/* ======================================================================================
*
* assign_query: reads and retuens the query in the file QUERY_PATH. The query is an n-dimensional array.
*
* ======================================================================================
*/
double *assign_query( )
{
	/* read query from file */
	double *query = (double *)malloc( sizeof(double)*(TOTAL_DIMENSIONS + 1) );

	/* open the file for reading */
	FILE *file = fopen ( QUERY_PATH , "r");  
	
	/* there is only one query per file */
	char *line = (char *)malloc(sizeof(char)*10000);
	line = fgets(line, 10000, file);

	/* tokenize the line read and put it in a double * array */
	char *tokens = (char *)malloc(sizeof(char)*(TOTAL_DIMENSIONS+1));
	tokens = strtok( line, " ");

	/* fetch each token and save it in an array */
	int i;
	for (i = 0; i < TOTAL_DIMENSIONS; i++, tokens = strtok(NULL, " "))
		query[i] = atof(tokens);

	/* free allocated string */
	free(tokens);
	
	/* close the file prior to exiting the routine */
	fclose(file);  

	return query;
}

/* ======================================================================================
*
* assign_window_values: reads the user input relative to the window sizes, tokenizes this input 
					and saves it in the WINDOWS global variabel
*
*      * input_window_values - string containing the arguments related to the window sizes
*
* ======================================================================================
*/
void assign_window_values(char * input_window_values)
{
	/* check if input is not empty */
	check_input(input_window_values, (char *)"assign_window_values");

	/* allocate memory for structure to save window values */
	WINDOWS = (int *)malloc(sizeof(int)*(NUM_PROJECTIONS));

	/* read and store window values */
	char *token = strtok(input_window_values, ",");

	int i;
	for (i = 0; i < NUM_PROJECTIONS; i++, token = strtok(NULL, ","))
		WINDOWS[i] = atoi(token);
}

/* ======================================================================================
*
* assign_root_directory: extracts the root directory from the DATASET_PATH variable and
*						stores this information in the ROOT_DIR global variable
*
* ======================================================================================
*/
void assign_root_directory( )
{
	/* search for the last occurrence of the character '\' in the DATASET_PATH string */
	char *pointer = strrchr(DATASET_PATH, '\\');

	/* compute the length of the string, starting from its beginning to the place of the last 
	 * occurrence of the '\' character  */
	int length = strlen(DATASET_PATH) - strlen(pointer) + 1;

	/* copy all the characters of the DATASET_PATH string until the '\' character to the 
	 * ROOT_DIR global variable */
	ROOT_DIR = (char *)malloc(sizeof(char)*(length + 1));
	memcpy(ROOT_DIR, DATASET_PATH, length);
	ROOT_DIR[length] = '\0';

}

/* ======================================================================================
*
* assign_dataset_name: tokenize the DATASET_PATH global variable in order to extact the 
*					the dataset name out of it. THis information will be stored in the
*					DATASET_NAME global variable
*
* ======================================================================================
*/
void assign_dataset_name()
{
	/* get pointer to the string after the last slash */
	char *pointer = strrchr(DATASET_PATH, '\\');

	/* copy this string to a temporary string */
	char *temp = (char *)malloc(sizeof(char)*(strlen(pointer+1)+1));
	memcpy(temp, pointer+1, strlen(pointer+1));
	temp[strlen(pointer+1)] = '\0';

	/* get pointer to the string after the last '.' */
	pointer = strrchr(temp, '.');

	/* if the dataset_name does not contain an extension file type */
	int size_str = 0;
	if (pointer == NULL)
		size_str = (int)strlen(temp) + 1;
	/* if the dataset_name contains an extension file type */
	else 
		size_str = (int)strlen(temp) - (int)strlen(pointer);

	/* assign to DATASET_NAME by copying the temporary string */
	DATASET_NAME = (char *)malloc(sizeof(char)*(size_str+3));
	memcpy(DATASET_NAME, temp, size_str);
	DATASET_NAME[size_str] = '\0';
	
	/* free temporary string */
	free(temp);
}

/* ======================================================================================
*
* assign_table_name: assigns the DB_TABLE_NAME variable. 
*					This function copies the DATASET_NAME to the variable DB_TABLE_NAME
*
* ====================================================================================== 
*/
void assign_table_name( )
{
	/* the DB_TABLE_NAME is the same as the DATASET_NAME */
	DB_TABLE_NAME = (char *)malloc(sizeof(char)*(strlen(DATASET_ROOT_NAME) + 50));
	sprintf(DB_TABLE_NAME, "[dbo].[%s_%d_%s]", DATASET_ROOT_NAME, TOTAL_DIMENSIONS, NORM_TYPE);
}

/* ======================================================================================
*
* assign_dataset_dimensions: tokenize the DATASET_NAME global variable in order to extact the 
*					the DATASET_ROOT_NAME out of it. Extract the number of vectors in the dataset file
*					into the TOTAL_VECTORS global variable and extract the total number of dimensions 
					of the dataset file into the TOTAL_DIMENSIONS global variables
*
* ======================================================================================
*/
void assign_dataset_dimensions()
{
	char *temp = (char *)malloc(sizeof(char)*(strlen(DATASET_NAME) + 1));
	strcpy(temp, DATASET_NAME);

	/* tokenize the DATASET_NAME by '_' to obtain the DATASET_ROOT_NAME,
	 * the number of vectors and the total number of dimensions */
	char *token = strtok(temp, "_");

	/* store the DATASET_ROOT_NAME */
	DATASET_ROOT_NAME = (char *)malloc(sizeof(strlen(token)+1));
	sprintf(DATASET_ROOT_NAME, "%s", token);

	/* if the number of projections is less than one, return error */
	check_input(token, (char *)"assign_dataset_dimensions");

	/* tokenize the DATASET_NAME variable in order to obtain the total number of vectors
	 * and the total number of dimensions */
	int i = 0;
	for (token = strtok(NULL, "_"); token != NULL; token = strtok(NULL, "_"), i++)
		if ( i == 0)
			TOTAL_VECTORS = atoi(token);
		else
			TOTAL_DIMENSIONS = atoi(token);

	/* free memmory */
	free( temp );
}

/* ======================================================================================
*
* assign_norm_type: assigns the user argument to the NORM_TYPE global variable
*
*			* user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_norm_type(char *user_input)
{
	check_input(user_input, (char *)"assign_norm_type");

	/* allocate memory for dataset_path variable */
	NORM_TYPE = (char *)malloc(sizeof(char)*(strlen(user_input) + 1));

	/* copy input string to dataset_path */
	strcpy(NORM_TYPE, user_input);
}

/* ======================================================================================
*
* assign_projection_type: assigns the user argument to the PROJECTION_TYPE global variable
*
*      * user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_projection_type(char *user_input)
{
	check_input(user_input, (char *)"assign_projection_type");

	/* allocate memory for dataset_path variable */
	PROJECTION_TYPE = (char *)malloc(sizeof(char)*(strlen(user_input) + 1));

	/* copy input string to dataset_path */
	strcpy(PROJECTION_TYPE, user_input);
}

/* ======================================================================================
*
* assign_query_phase: assigns the user argument to the PERFORM_QUERY global variable
*
*		* user_input - string containing the arguments of the main program
* 
* ======================================================================================
*/
void assign_query_phase(char *user_input)
{
	check_input(user_input, (char *)"assign_query_phase");

	PERFORM_QUERY = atoi(user_input);
}

/* ======================================================================================
*
* assign_epsilon: assigns the user argument to the EPSILON global variable
*                       if it fails to allocate the memory, the program exits with error
*
*		* user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_epsilon(char *user_input)
{
	check_input(user_input, (char *)"assign_epsilon");

	EPSILON = atoi(user_input);
}

/* ======================================================================================
*
* assign_index_phase: assigns the user argument to the PERFORM_INDEX_PHASE global variable
*
*		* user_input - string containing the arguments of the main program
*
* ======================================================================================
*/
void assign_index_phase(char *user_input)
{
	check_input(user_input, (char *)"assign_index_phase");

	PERFORM_INDEX_PHASE = atoi(user_input);
}

/* ======================================================================================
*
* print_windows: displays the values that are contained in the WINDOWS variable.
*                used for debugging purposes
*
* ======================================================================================
*/
void print_windows()
{
	int i;
	printf("WINDOWS\n");

	for (i = 0; i < NUM_PROJECTIONS; i++)
		printf("%d ", WINDOWS[i]);
	printf("\n");
}

/* ======================================================================================
*
* print_input_variables: displays the values that are contained in the WINDOWS variable.
*                used for debugging purposes
*
* ======================================================================================
*/
void print_input_variables()
{
	printf("\n\n# PRINTING INPUT VARIABLES #\n\n");

	printf("DEBUG_OPTION = %d\n", DEBUG_OPTION);

	printf("DATASET_PATH = %s\n", DATASET_PATH);

	printf("QUERY_PATH = %s\n", QUERY_PATH);

	printf("DATASET_ROOT_NAME = %s\n", DATASET_ROOT_NAME);

	printf("NUM_PROJECTIONS = %d\n", NUM_PROJECTIONS);

	print_windows( );

	printf("ROOT_DIR = %s\n", ROOT_DIR);

	printf("DATASET_NAME = %s\n", DATASET_NAME);

	printf("DB_TABLE_NAME = %s\n", DB_TABLE_NAME);

	printf("TOTAL_DIMENSIONS = %d\n", TOTAL_DIMENSIONS);

	printf("TOTAL_VECTORS = %d\n", TOTAL_VECTORS);

	printf("NORM_TYPE = %s\n", NORM_TYPE);

	printf("PROJECTION_TYPE = %s\n", PROJECTION_TYPE);

	printf("PERFORM_INDEX_PHASE = %d\n", PERFORM_INDEX_PHASE);

	printf("PERFORM_QUERY = %d\n", PERFORM_QUERY);

	printf("EPSILON = %f\n", EPSILON );
}
