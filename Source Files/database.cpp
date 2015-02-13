/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* database.cpp
* This file contains the definition of the functions that are used to perform a connection to
* a database in Microsoft SQL Server, using a driver and a server name, and to perform queries
* in the database. If the driver or the server name are not specified correctly, the program opens
* a window so the user can select the desired database.
*
* Author: Catarina Moreira
* Personal Page: http://web.ist.utl.pt/~catarina.p.moreira/index.html
*
* Supervisor: Andreas Wichert
* Personal Page: http://web.ist.utl.pt/~andreas.wichert/
*
* ====================================================================================== */

#include "database.hpp"

/* ======================================================================================
*
* sql_allocate_env: allocates an SQL environment handler. This is required to open a 
*				database connection
*
*		* henv - SQL environment
*
* ======================================================================================
*/
SQLHENV sql_allocate_env(SQLHENV henv)
{
	/* returns return code of SQL executions; Used to detect function failures*/
	SQLRETURN retcode;

	/* Allocate environment handle */
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	
	/* Check if the allocation of the environment handler was successful */
	sql_verify_error( retcode, "sql_allocate_env" );

	return henv;
}

/* ======================================================================================
*
* sql_connect_database: opens an SQL connection with the given arguments. If the connection
*				fails, a window will pop up to ask for the path of the database
*
*		* hdbc - SQL ODBC connection
*		* henv - SQL environment
*		* driver - a string containing the SQL Server ODBC driver
*		* server - a string containing the name of the server
*		* database - a string containing the database name
*
* ======================================================================================
*/
SQLHDBC sql_connect_database(SQLHDBC hdbc, SQLHENV henv, char *driver, char *server, char *database)
{
	/* return code of SQL executions; Used to detect function failures */
	SQLRETURN retcode;	 

	/* Set the ODBC version environment attribute */
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

	/* Check if the setting of the ODBC version was successful */
	sql_verify_error(retcode, "sql_connect_database_1");

	/* Allocate connection handle */
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

	/* Check if the allocation of the connection handler was successful */
	sql_verify_error(retcode, "sql_connect_database_2");

	/* Set login timeout to 1 seconds */
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)1, 0);

	/* Connect to database. If the connection string is wrong, then a window will prompt */
	printf("Attempting to connect to database...\n\n");
	SQLWCHAR *sql_connection = build_connection_str(driver, server, database);
		
	/* connect to database */
	retcode = SQLDriverConnect(hdbc, GetDesktopWindow(), sql_connection, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

	/* Check if connection to database was successful,
	 * if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_connect_database_3");

	printf("Successfully Connected to Database!\n\n");

	/* free memory allocated for connection strings */
	free(sql_connection);

	/* return connecion handler */
	return hdbc;
}

/* ======================================================================================
*
* sql_transfer_data_to_database: performs a bulk insert into the database
*
*	* hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void sql_transfer_data_to_database(SQLHDBC hdbc)
{
	printf("Transfering data to table %s\n\n", DB_TABLE_NAME);
	SQLWCHAR* query = build_query_to_import_data( );

	/* return code of SQL executions; Used to detect function failures */
	SQLRETURN retcode;

	SQLHSTMT hstmt = sql_allocate_stmt(hdbc);
	retcode = sql_make_prepared_query(hdbc, query, hstmt);

	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_transfer_data_to_database");

	/* close SQL connection */
	sql_close_stmt_handler(hstmt);

	/* free memory */
	free(query);
}

/* ======================================================================================
*
* sql_compute_distances: computes the distance between each vector of the database with a 
*						query vector.  The distances can be computed using the L1 norm or L2 norm
*
*		* hdbc - an opened SQL connection
*		* query_matrix - matrix containing the query vector and all of its projections
*		* chunk - index of the current chunk of data being processed
*		* dimensions - number of dimensions of the current projection 
*
* ======================================================================================
*/
long *sql_compute_distances(SQLHDBC hdbc, gsl_matrix *query_matrix, int chunk, int dimensions )
{
	/* build query to compute distances between query and dabase vectors */
	SQLWCHAR* query = build_query_to_compute_distance(query_matrix, chunk, dimensions);

	/* return code of SQL executions; Used to detect function failures */
	SQLRETURN retcode;

	/* perform SQL query */
	SQLHSTMT hstmt = sql_allocate_stmt(hdbc);
	retcode = sql_make_prepared_query(hdbc, query, hstmt);

	/* allocate memory to hold the query output */
	long *result_temp = (long*)malloc(sizeof(long)*1000000);

	/* iterate over all retrieved items from the SQL query */
	int row = 0;
	while (TRUE)
	{
		/* fetch the results of the SQL query one by one */
		retcode = SQLFetch(hstmt);

		/* if there is no more data to fetch, finish */
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			break;
		
		/* temporary variable to hold the featched data */
		float data;
		
		/* Get the data */
		SQLGetData(hstmt, 2, SQL_C_FLOAT, &data, sizeof(float), NULL);
		
		/* store the data */
		result_temp[row] = data;
		row++;
	}

	/* update the global variable,which counts the number of items retrieved by the query */
	NUM_ITEMS = row;

	/* close SQL statement */
	sql_close_stmt_handler(hstmt);

	/* free memory */
	free( query );
	
	return result_temp;
}


/* ======================================================================================
*
* sql_fill_database: performs a bulk insert into the database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void sql_fill_database(HDBC hdbc, int dims)
{
	/* create sql table */
	sql_create_table(hdbc, dims);

	/* import data to tabble */
	sql_transfer_data_to_database(hdbc, dims);

	/* create an ID column to index the table */
	sql_index_table(hdbc);

	/* create a primary key */
	sql_add_primary_key(hdbc);
}

/* ======================================================================================
*
* sql_create_table: creates a SQL table query of the form: 
*					CREATE TABLE <table_name> ( c_0 FLOAT ... c_dims FLOAT );
*
*       * hdbc - an  opened SQL connection
*		* dims - number of columns for the table
*
* ======================================================================================
*/
void sql_create_table(SQLHDBC hdbc, int dims)
{
	printf("\n\nCreating Table %s\n\n", DB_TABLE_NAME);

	/* return code of SQL executions; Used to detect function failures */
	SQLRETURN retcode;

	/* Creates query of the form: CREATE TABLE <table_name> ( c_0 FLOAT ... c_dims FLOAT ); */
	SQLWCHAR *query = build_query_to_create_table(dims);
	HSTMT hstmt = sql_allocate_stmt(hdbc);
	
	/* perform SQL query */
	retcode = sql_make_prepared_query(hdbc, query, hstmt);
	
	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_create_table");
	
	/* close SQL statement */
	sql_close_stmt_handler(hstmt);
	
	/* free memory */
	free(query);
}

/* ======================================================================================
*
* sql_allocate_stmt: allocates an SQL statement handler to enable the computation of SQL queries
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
SQLHSTMT sql_allocate_stmt(SQLHDBC hdbc)
{
	/* return code of SQL executions; Used to detect function failures */
	SQLRETURN retcode;		

	/* SQL statement handler variable */
	SQLHSTMT hstmt;			

	/* Allocate an SQL statement handler */
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_allocate_stmt");

	return hstmt;
}

/* ======================================================================================
*
* sql_make_prepared_query: performs an SQL query
*
*		* hdbc - an  opened SQL connection
*		* query - the query to be performed in the SQLWCHAR * type
*		* hstmt - sql statement handler to excute the query
*
* ======================================================================================
*/
SQLSMALLINT sql_make_prepared_query(SQLHDBC hdbc, SQLWCHAR *query, SQLHSTMT hstmt)
{
	/* return code of SQL executions; Used to detect function failures */
	SQLSMALLINT retcode; 

	/* Prepare the SQL statement */
	retcode = SQLPrepare(hstmt, query, SQL_NTS);

	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_make_prepared_query");

	/* Execute the SQL statement */
	retcode = SQLExecute(hstmt);
	
	if (DEBUG_OPTION > 1) printf("%ws\n", query);
	
	return retcode;
}

/* ======================================================================================
*
* sql_get_database: returns a chunk of data from an SQL table
*
*		* hdbc - an  opened SQL connection
*		* number_remaining_vectors - total vectors to fetch in the SQL query
*		* num_dims - dimension of the current projection
*		* indx - index of the current chunk of data
*
* ======================================================================================
*/
gsl_matrix *sql_get_database(SQLHDBC hdbc, long number_remaining_vectors, int num_dims, int indx)
{
	printf("\n\nvecs = %d\tdims = %d\tindx = %d\n\n", number_remaining_vectors,  num_dims,  indx);

	/* Allocate Memory in a Matrix to hold the database */
	gsl_matrix *database_matrix = gsl_matrix_alloc(number_remaining_vectors, num_dims);	
	
	/* compute indexes to fetch data from SQL table */
	long long start_indx = indx*CHUNK_SIZE + 1;
	long long end_indx = indx*CHUNK_SIZE + CHUNK_SIZE;

	/* builds an SQLWCHAR representation of the query: 
	 * SELECT * FROM <table> WHERE ID >= <x> AND ID <= <y> */
	SQLWCHAR *query = build_query_to_select_data(start_indx, end_indx);

	/* execute query */
	SQLHSTMT hstmt = sql_allocate_stmt(hdbc);
	SQLSMALLINT retcode = sql_make_prepared_query(hdbc, query, hstmt);

	/* Check if the query was successfull otherwise ,
	* the function returns an error and exit the program */
	sql_verify_error(retcode, "sql_get_database");

	/* retrieve data and save it in the database_matrix structure */
	sql_retrieve_database_data( hstmt, &database_matrix, num_dims);

	/* close database connection */
	sql_close_stmt_handler(hstmt);

	return database_matrix;
}

/* ======================================================================================
*
* sql_retrieve_database_data: reads the data returned by an SQL query and stores the results 
*						in a gsl_matrix structure
*
*		* hstmt - an opened SQL statement handler
*		* database_matrix - structure that will hold the data retrieved by the SQL query
*		* num_dims - dimension of  the current projection step
*
* ======================================================================================
*/
void sql_retrieve_database_data(HSTMT hstmt, gsl_matrix **database_matrix, int num_dims)
{
	/* return code of SQL executions; Used to detect function failures */
	SQLSMALLINT retcode = 0;

	/* index of the rows. For each line of data returned by the SQL query, 
	 * the row index increments */
	int row = 0;
	while (TRUE)
	{
		/* fetch each row of data retrieved by the SQL query */
		retcode = SQLFetch(hstmt);

		/* if there is no more data to be retrieved, end the function */
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
			break;

		/* A table containing data is returned by the SQL query. 
		 * we need to go through all columns of data (index c) in order
		 * to store the data in the temporary variable data  */
		int c; float data;
		for (c = 1; c <= num_dims; c++)
		{
			/* retrieve data */
			SQLGetData(hstmt, c, SQL_C_FLOAT, &data, sizeof(float), NULL);
			
			/* store data in gsl_matrix structure */
			gsl_matrix_set( (*database_matrix), row, c - 1, data);
		}
		row++;
	}
}

/* ======================================================================================
*
* sql_index_table: creates a new column for an existing SQL table that will be used as a 
*				primary key to index the table. The column will generate IDs for each row of
*				an existing SQL table automatically
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void sql_index_table(SQLHDBC hdbc)
{
	/* build a query to index an SQL table. The query has the form: 
	 * ALTER TABLE <table_name> ADD ID BIGINT IDENTITY(1,1) */
	SQLWCHAR *query = build_query_to_index_table();

	/* allocate an SQL statement and perform the SQL query */
	HSTMT hstmt = sql_allocate_stmt(hdbc);
	sql_make_prepared_query(hdbc, query, hstmt);

	/* query is finished. So, close the SQL statement handler */
	sql_close_stmt_handler(hstmt);
	
	/* free memory */
	free(query);
}

/* ======================================================================================
*
* sql_add_primary_key: creates a primary key over the column ID
*
*		* hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void sql_add_primary_key(SQLHDBC hdbc)
{
	/* build a query to index an SQL table. The query has the form: 
	 * ALTER TABLE <table_name> ADD PRIMARY KEY( ID ) */
	SQLWCHAR *query = build_query_to_add_pkey( );
	
	/* allocate an SQL statement and perform the SQL query */
	HSTMT hstmt = sql_allocate_stmt(hdbc);
	sql_make_prepared_query(hdbc, query, hstmt);

	/* query is finished. So, close the SQL statement handler */
	sql_close_stmt_handler(hstmt);
	
	/* free memory */
	free(query);
}

/* ======================================================================================
*
* sql_close_connection: Closes an opened connection to a database
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void sql_close_connection(SQLHDBC hdbc)
{
	/* Disconnect database */
	SQLSMALLINT retcode = SQLDisconnect(hdbc);

	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_close_connection");
}

/* ======================================================================================
*
* sql_close_stmt_handler: Deallocates the SQL Statement handler
*
*      * hstmt - an opened SQL statement handler
*
* ======================================================================================
*/
void sql_close_stmt_handler(SQLHSTMT hstmt)
{
	/* Deallocate statement handle */
	SQLSMALLINT retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_close_stmt_handler");
}

/* ======================================================================================
*
* sql_close_connection_handler: Deallocates the SQL Connection handler
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
void sql_close_connection_handler(SQLHDBC hdbc)
{
	/* Deallocate connection handler */
	printf("Deallocating SQL connection handler...\n");
	SQLSMALLINT retcode = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);

	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_close_connection_handler");
}

/* ======================================================================================
*
* sql_close_env_handler:  Deallocates the SQL Environment handler
*
*      * retcode - an  integer resulting from an operation to the database
*
* ======================================================================================
*/
void sql_close_env_handler(SQLHENV henv)
{
	/* Deallocate SQL environment handler */
	printf("Deallocating SQL environment handler...\n");
	SQLSMALLINT retcode = SQLFreeHandle(SQL_HANDLE_ENV, henv);

	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	sql_verify_error(retcode, "sql_close_env_handler");
}

/* ======================================================================================
*
* sql_verify_error: checks if an operation to the database failed to execute.
*				if the operation does not finish successfully, the application
*			    terminates with error code -100
*
*      * retcode - an  integer resulting from an operation to the database
*
* ======================================================================================
*/
void sql_verify_error( SQLSMALLINT retcode, char *function )
{
	/* Check if connection to database was successful,
	* if connection fails, return an error and exit the program */
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		return;

	printf("\n\nFailed to execute query in function %s with retcode %d\n\n",  function, retcode);
	system("PAUSE");
	exit(-10);
}