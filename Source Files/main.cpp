/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* main.cpp
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

#define MAIN_FILE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>

#include "database.hpp"
#include "projection.hpp"
#include "input_manipulation.hpp"

void update_variables(int indx);

/*
 * Implementation of the Hierarchical Linear SubSpace Indexing Model
 * for content-based retrieval.
 *
 */
int main( int argc, char **argv )
{
    /* get data from user's input */
    assign_user_input(argv);

    /* open SQL connection */
	SQLHENV henv = SQL_NULL_HANDLE;
	henv = sql_allocate_env(henv);

	/* initialize SQl connection variable */
	SQLHDBC hdbc = SQL_NULL_HANDLE;
	hdbc = sql_connect_database(hdbc, henv, "SQL Server Native Client 12.0", "CATARINAMORB1C0", "master");

	/* track running time */
	clock_t t1, t2;

	/*									INDEXING PHASE								  */
	/* ****************************************************************************** */
	if( BILLION_DATASET == 0 )
		project_database(hdbc);
	else
	{
		int i;
		for( i = 99; i <= 100; i++ )
		{
			/* if the indexing option is on, then index the dataset */
			project_database(hdbc);

			/* update variables: table name, dataset path and dataset root name */
			update_variables(i);
		}
	}
	/* ****************************************************************************** */



	/*									QUERY PHASE								  */
	/* ****************************************************************************** */

    /* if the query option is on, then perform queries */
	/* variable to hold the time of start of the process */
	int run;
	float *diff = (float*)malloc(sizeof(float)*5);

	double NUM_RUNS = 5;
	float avg_diffs = 0;

	long *IDs;

	/* compute each query 10x */
	for( run = 0; run < NUM_RUNS; run++ )
	{
		/* save starting time */
		t1 = clock();   

		/* find more similar vectors */
		IDs = performQuery(hdbc);
  
		/* save ending time */
		 t2 = clock();

		 /* compute running time for each query */
		 diff[run] = (((float)t2 - (float)t1) / 1000000.0F ) * 1000;   
		 avg_diffs += diff[run];
	}

	/* compute the average query time */
	for( run = 0; run < NUM_RUNS; run++ )
		printf("Run #%d: %f\n", run, diff[run]);

	printf("\nAverage Time for query1 = %f\n", avg_diffs / NUM_RUNS );
	printf("\nNumber of similar vectors returned = %ld\n", NUM_ITEMS);
	
	/* preview the computed vectors */
	int i;
	for( i = 0; i < NUM_ITEMS; i++ )
		if( i >= 10 ) break;
		else
			printf("%d\n", IDs[i]);

	/* free memory */
	free( IDs );

	/* close database connections */
	sql_close_connection(hdbc);
	sql_close_connection_handler(hdbc);
	sql_close_env_handler(henv);

	system("PAUSE");

    return EXIT_SUCCESS;
}


void update_variables(int indx)
{
	int len = length(indx);
	if( indx >= 100 )
		len--;

	/* update DATASET_PATH */
	free(DATASET_PATH);
	DATASET_PATH = (char *)malloc(sizeof(char)*(50 + strlen(DATASET_ROOT_NAME) ));
	sprintf(DATASET_PATH, "%s%d%s_%ld_%d.txt", ROOT_DIR, indx, DATASET_ROOT_NAME+len, TOTAL_VECTORS, TOTAL_DIMENSIONS);

	printf("\n\nUpdating dataset path to %s\n\n", DATASET_PATH);

	/* update DATASET_ROOT_NAME */
	update_dataset_root_name();
	printf("\n\nUpdating root name to %s\n\n", DATASET_ROOT_NAME);

	/* update DB_TABLE_NAME */
	update_table_name(TOTAL_DIMENSIONS);
	printf("\n\nUpdating table name to %s\n\n", DB_TABLE_NAME);
}

