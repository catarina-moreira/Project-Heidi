/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* database.hpp
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

#ifndef __Heidi__database__
#define __Heidi__database__

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#include <gsl/gsl_matrix.h>

#include "query.hpp"

/* 
* sql_allocate_env: allocates an SQL environment handler. This is required to open a 
*				database connection
*
*		* henv - SQL environment 
*/
SQLHENV sql_allocate_env(SQLHENV henv);

/* 
* sql_connect_database: opens an SQL connection with the given arguments. If the connection
*				fails, a window will pop up to ask for the path of the database
*
*		* hdbc - SQL ODBC connection
*		* henv - SQL environment
*		* driver - a string containing the SQL Server ODBC driver
*		* server - a string containing the name of the server
*		* database - a string containing the database name
*/
SQLHDBC sql_connect_database(SQLHDBC hdbc, SQLHENV henv, char *driver, char *server, char *database);

/*
* sql_transfer_data_to_database: performs a bulk insert into the database
*
*		* hdbc - an  opened SQL connection
*/
void sql_transfer_data_to_database(SQLHDBC hdbc);

/*
 * sql_compute_distances: computes the distance between each vector of the database with a 
 *						query vector. The distances can be computed using the L1 norm or L2 norm
 *
 *		* hdbc - an opened SQL connection
 *		* query_matrix - matrix containing the query vector and all of its projections
 *		* chunk - index of the current chunk of data being processed
 *		* dimensions - number of dimensions of the current projection
*/
long *sql_compute_distances(SQLHDBC hdbc, gsl_matrix *query_matrix, int chunk, int dimensions );

/* 
* sql_fill_database: performs a bulk insert into the database
*
*		* hdbc - an  opened SQL connection
*/
void sql_fill_database(HDBC hdbc, int dims);

/*
* sql_create_table: creates a SQL table query of the form: 
*					CREATE TABLE <table_name> ( c_0 FLOAT ... c_dims FLOAT );
*
*       * hdbc - an  opened SQL connection
*		* dims - number of columns for the table
*/
void sql_create_table(SQLHDBC hdbc, int dims);

/*
* sql_allocate_stmt: allocates an SQL statement handler to enable the computation of SQL queries
*
*      * hdbc - an  opened SQL connection
*/
SQLHSTMT sql_allocate_stmt(SQLHDBC hdbc);

/* 
* sql_make_prepared_query: performs an SQL query
*
*		* hdbc - an  opened SQL connection
*		* query - the query to be performed in the SQLWCHAR * type
*		* hstmt - sql statement handler to excute the query
*/
SQLSMALLINT sql_make_prepared_query(SQLHDBC hdbc, SQLWCHAR *query, SQLHSTMT hstmt);

/* 
* sql_get_database: returns a chunk of data from an SQL table
*
*		* hdbc - an  opened SQL connection
*		* number_remaining_vectors - total vectors to fetch in the SQL query
*		* num_dims - dimension of the current projection
*		* indx - index of the current chunk of data
*/
gsl_matrix *sql_get_database(SQLHDBC hdbc, long number_remaining_vectors, int num_dims, int indx);

/* 
* sql_retrieve_database_data: reads the data returned by an SQL query and stores the results 
*						in a gsl_matrix structure
*
*		* hstmt -  an opened SQL statement handler
*		* database_matrix - structure that will hold the data retrieved by the SQL query
*		* num_dims - dimension of  the current projection step
*/
void sql_retrieve_database_data(HSTMT hstmt, gsl_matrix **database_matrix, int num_dims);

/* 
* sql_index_table: creates a new column for an existing SQL table that will be used as a 
*				primary key to index the table. The column will generate IDs for each row of
*				an existing SQL table automatically
*
*      * hdbc - an  opened SQL connection
*/
void sql_index_table(SQLHDBC hdbc);

/* 
* sql_add_primary_key: creates a primary key over the column ID
*
*		* hdbc - an  opened SQL connection
*/
void sql_add_primary_key(SQLHDBC hdbc);

/*
* sql_close_connection: Closes an opened connection to a database
*
*      * hdbc - an  opened SQL connection
*/
void sql_close_connection(SQLHDBC hdbc);

/* 
* sql_close_connection_handler: Deallocates the SQL Connection handler
*
*      * hdbc - an  opened SQL connection
*/
void sql_close_connection_handler(SQLHDBC hdbc);

/* 
* sql_close_stmt_handler: Deallocates the SQL Statement handler
*
*      * hstmt - an opened SQL statement handler
*/
void sql_close_stmt_handler(SQLHSTMT hstmt);

/* 
* sql_close_env_handler:  Deallocates the SQL Environment handler
*
*      * retcode - an  integer resulting from an operation to the database
*/
void sql_close_env_handler(SQLHENV henv);

/*
* sql_verify_error: checks if an operation to the database failed to execute.
*				if the operation does not finish successfully, the application
*			    terminates with error code -100
*
*      * retcode - an  integer resulting from an operation to the database
*/
void sql_verify_error(SQLSMALLINT retcode, char *function);

#endif
