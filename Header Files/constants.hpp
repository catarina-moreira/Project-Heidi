/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* constants.hpp
* This file contains the definition of a set of global variables that will be used throught
* the entire program.
*
* Author: Catarina Moreira
* Personal Page: http://web.ist.utl.pt/~catarina.p.moreira/index.html
*
* Supervisor: Andreas Wichert
* Personal Page: http://web.ist.utl.pt/~andreas.wichert/
*
* ====================================================================================== */

#ifndef __Heidi__constants__
#define __Heidi__constants__

/* information given in input */
#define DEBUG_INDX              1	//
#define DATASET_PATH_INDX       2	//
#define WINDOW_VALUES_INDX      3	//
#define COMP_MODE_INDX          4	//
#define EXEC_INDEXING_INDX      5	//
#define EXEC_QUERY_INDX         6	//
#define EPSILON_INDX            7	//
#define QUERY_PATH_INDX         8	//
#define PROJECTION_INDX         9	//
#define BILLION_DATASET			0

/* size of each chunk of data */
#define CHUNK_SIZE              10000

#define DELIMITER "\\"

#ifdef  MAIN_FILE

/* path where the dataset file is located */
char *DATASET_PATH;

/* path where the query file is located */
char *QUERY_PATH;

/* turns on/off the prints of the application */
int DEBUG_OPTION;

/* find and save how many projections will be performed. Projections need to bigger than zero */
int NUM_PROJECTIONS;

/* get window sizes from input */
int *WINDOWS;

/* dataset file name without the extension */
char *DATASET_NAME;

/* dataset file name without information relative to the vectors */
char *DATASET_ROOT_NAME;

/* name that will be used for the creation of the SQL table */
char* DB_TABLE_NAME;

/* total number of vectors in the dataset */
long TOTAL_VECTORS;

/* total number of dimensions of the dataset */
int TOTAL_DIMENSIONS;

/* norm to be used in the computation of the subspaces */
char *NORM_TYPE;

/* type of projection to be performed. Only allowed: orthogonal and pca */
char *PROJECTION_TYPE;

/* path to the directory where the dataset is contained */
char *ROOT_DIR;

/* integer representing if the user wants to index the dataset: 0 for no, 1 for yes */
int PERFORM_INDEX_PHASE;

/* integer representing if the user wants to index the dataset: 0 for no, 1 for yes */
int PERFORM_QUERY;

/* integer representing the total number of nearest neighbours retrieved by the algorithm  */
int NUM_ITEMS;

/* double value representing the maximum distance acceptable to find nearest neighbours  */
double EPSILON;

#else // ===================================================================================

/* path where the dataset file is located */
extern char *DATASET_PATH;

/* path where the query file is located */
extern char *QUERY_PATH;

/* turns on/off the prints of the application */
extern int DEBUG_OPTION;

/* find and save how many projections will be performed. Projections need to bigger than zero */
extern int NUM_PROJECTIONS;

/* get window sizes from input */
extern int *WINDOWS;

/* dataset file name without the extension */
extern char *DATASET_NAME;

/* dataset file name without information relative to the vectors */
extern char *DATASET_ROOT_NAME;

/* name that will be used for the creation of the SQL table */
extern char* DB_TABLE_NAME;

/* total number of vectors in the dataset */
extern long TOTAL_VECTORS;

/* total number of dimensions of the dataset */
extern int TOTAL_DIMENSIONS;

/* norm to be used in the computation of the subspaces */
extern char *NORM_TYPE;

/* type of projection to be performed. Only allowed: orthogonal and pca */
extern char *PROJECTION_TYPE;

/* path to the directory where the dataset is contained */
extern char *ROOT_DIR;

/* integer representing if the user wants to index the dataset: 0 for no, 1 for yes */
extern int PERFORM_INDEX_PHASE;

/* integer representing if the user wants to index the dataset: 0 for no, 1 for yes */
extern int PERFORM_QUERY;

/* integer representing the total number of nearest neighbours retrieved by the algorithm  */
extern double EPSILON;

/* double value representing the maximum distance acceptable to find nearest neighbours  */
extern int NUM_ITEMS;

#endif /* defined(__Main__file__) */
#endif /* defined(__Heidi__constants__) */
