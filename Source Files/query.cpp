/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* query.cpp
* The functions contained in this file are primarily SQLWCHAR representations
* of queries that need to be executed in SQLSever.
*
* Author: Catarina Moreira
* Personal Page: http://web.ist.utl.pt/~catarina.p.moreira/index.html
*
* Supervisor: Andreas Wichert
* Personal Page: http://web.ist.utl.pt/~andreas.wichert/
*
* ====================================================================================== */


#include "query.hpp"

/* ======================================================================================
*
* build_connection_str: createas an SQLWCHAR representation of the Microsoft SQL Server
*					connection string. The string takes the form:
*					Driver={<driver>};Server=<server>;Integrated Security=True;DATABASE=<database>;
*					Trusted_Connection=Yes;
*
*      * driver - string representing the Microsoft SQL Server Driver (example: SQL Server Native Client 12.0)
*	   * server - string containing the name of the installed SQL Server (example: local)
*	   * database - string containing the name of the database to be used (example: heidi)
*
* ======================================================================================
*/
SQLWCHAR *build_connection_str(char *driver, char *server, char *database)
{
	/* build an SQL connection string of the form: Driver={driver};Server=server;Integrated Security=True;DATABASE=database;
	* Trusted_Connection=Yes;*/
	char *connection_str = (char *)malloc(sizeof(char)*(120 + strlen(driver) + strlen(server) + strlen(database)));
	sprintf(connection_str, "Driver={%s};Server=%s;Integrated Security=True;DATABASE=%s;Trusted_Connection=Yes;", driver, server, database);

	/* Create SQLWCHAR representation of the connection string */
	SQLWCHAR *sql_connection = (SQLWCHAR *)malloc(sizeof(SQLWCHAR)*(strlen(connection_str) + 1));
	swprintf(sql_connection, L"%s", connection_str);

	/* print the query for debugging purposes */
	if (DEBUG_OPTION > 1) printf("%ws\n\n", connection_str);

	/* Clear memory asociated to the string representation of the connection string*/
	free(connection_str);

	/* Return the SQLWCHAR format of the connection string */
	return sql_connection;
}

/* ======================================================================================
*
* build_query_to_import_data: performs a bulk insert into the database by creating an
*						SQWCHAR representation of the string:
*						BULK INSERT [dbo]. [TABLE_NAME] FROM 'VECTOR_FILE_PATH' 
*											WITH( FIELDTERINATOR = ' ', ROWTERMINATOR = '0x0a')
*						The '0x0a' character correcponds to the hexadecimal representaion of \r\n
*
*      * hdbc - an  opened SQL connection
*
* ======================================================================================
*/
SQLWCHAR *build_query_to_import_data(  )
{
	/* Allocate memmory for the query */
	int size = 110 + strlen(DB_TABLE_NAME) + strlen(DATASET_PATH) + length(TOTAL_DIMENSIONS);
	SQLWCHAR *query = (SQLWCHAR *)malloc(sizeof(SQLWCHAR)*size);

	/* Create SQL string: BULK INSERT [dbo]. [TABLE_NAME] FROM 'VECTOR_FILE_PATH' 
	 * WITH( FIELDTERINATOR = ' ', ROWTERMINATOR = '0x0a');*/
	swprintf(query, size, L"BULK INSERT %hs FROM '%hs' WITH( FIELDTERMINATOR = ' ', ROWTERMINATOR = '0x0a' );\0", DB_TABLE_NAME, DATASET_PATH );

	/* print the query for debugging purposes */
	if (DEBUG_OPTION > 1) printf("%ws\n\n", query);

	return query;
}

/* ======================================================================================
*
* build_query_to_create_table: ceates an SQLWCHAR representation of the string to create an SQL tale.
*					The string has the form: CREATE TABLE <table_name> ( c_0 FLOAT ... c_dims FLOAT );
*
*      * dims - integer representing the number of columns of the SQL table
*
* ======================================================================================
*/
SQLWCHAR *build_query_to_create_table(int dims)
{
	/* Allocate memmory for the query */
	int size = 20 + 15 * dims + strlen(DB_TABLE_NAME) + length(TOTAL_DIMENSIONS);
	char *query_str = (char *)malloc(sizeof(char)*size);

	/* Build string representation of the query */
	sprintf(query_str, "CREATE TABLE %s ( c_0 FLOAT", DB_TABLE_NAME);
	query_str = concat_query(query_str, dims);

	/* Convert string representation of the query to an SQLWCHAR type */
	SQLWCHAR *query = (SQLWCHAR *)malloc(sizeof(SQLWCHAR)*(strlen(query_str) + 1));
	swprintf(query, L"%hs", query_str);

	/* Clear memory asociated to the string representation of the query*/
	free(query_str);

	/* print the query for debugging purposes */
	if (DEBUG_OPTION > 1) printf("%ws\n\n", query);

	/* return SQLWCHAR representation of the query */
	return query;
}

/* ======================================================================================
*
* concat_query: concats a query N times. Auxiliary function for function build_query_to_create_table.
*			Builds a query of the form: c_0 FLOAT, c_1 FLOAT, c_2 FLOAT, ..., c_dims FLOAT
*
*      * query - the query to be repeated
*	   * dims - the number of times the query will be repeated
*
* ====================================================================================== */
char *concat_query( char *query, int dims )
{
	int i;
	for (i = 1; i < dims; i++)
	{
		/* allocate memory for temporary string */
		char *temp = (char *)malloc(sizeof(char)*( 15 + length(i) ));
		sprintf(temp, ", c_%d FLOAT", i);

		/* concat the query */
		strcat(query, temp);

		/* temporary string is no longer needed for this iteration */
		free(temp);
	}
	/* add the terminator caracter to the string */
	strcat(query, ");\0");

	return query;
}

/* ======================================================================================
*
* build_query_to_select_data: creates an SQLWCHAR repreentation of the string:
*							  
*
*      * start_indx - an  opened SQL connection
*	   * end_indx - 
*
* ======================================================================================
*/
SQLWCHAR *build_query_to_select_data(long long start_indx, long long end_indx)
{
	/* length of the string */
	long long size = 200 + strlen(DB_TABLE_NAME) + length(start_indx) + length(end_indx);

	/* SELECT * FROM myData12 WHERE ID <= end_indx AND ID >= start_indx */
	SQLWCHAR *query = (SQLWCHAR*)malloc(sizeof(SQLWCHAR)*(size));
	swprintf(query, L"SELECT * FROM %hs WHERE ID >= %lld AND ID <= %lld", DB_TABLE_NAME, start_indx, end_indx);

	/* print the query for debugging purposes */
	if( DEBUG_OPTION > 0 ) printf("\n\n%ws\n\n", query);

	return query;
}

/* ======================================================================================
*
* build_query_to_index_table: creates an SQLWCHAR repreentation of the string:
*							  ALTER TABLE <table_name> ADD ID BIGINT IDENTITY(1,1)
*
* ====================================================================================== */
SQLWCHAR *build_query_to_index_table()
{
	int size = 50 + strlen(DB_TABLE_NAME);
	SQLWCHAR *query = (SQLWCHAR *)malloc(sizeof(SQLWCHAR)*size);

	swprintf(query, size, L"ALTER TABLE %hs ADD ID BIGINT IDENTITY(1,1);", DB_TABLE_NAME);

	/* print the query for debugging purposes */
	if (DEBUG_OPTION > 1) printf("%ws\n\n", query);

	/* return SQLWCHAR representation of the query that adds an ID column */
	return query;
}

/* ======================================================================================
*
* build_query_to_add_pkey: creates an SQLWCHAR repreentation of the string:
*					       ALTER TABLE <table_name> ADD PRIMARY KEY( ID )
*
* ====================================================================================== */
SQLWCHAR *build_query_to_add_pkey()
{
	int size = 40 + strlen(DB_TABLE_NAME);
	SQLWCHAR *query = (SQLWCHAR *)malloc(sizeof(SQLWCHAR)*size);

	swprintf(query, size, L"ALTER TABLE %hs ADD PRIMARY KEY(ID);", DB_TABLE_NAME);

	/* print the query for debugging purposes */
	if (DEBUG_OPTION > 1) printf("%ws\n\n", query);

	/* return SQLWCHAR representation of the query that adds a primary key */
	return query;
}

/* ======================================================================================
*
* build_query_to_add_pkey: creates an SQLWCHAR repreentation of the string:
*					       ALTER TABLE <table_name> ADD PRIMARY KEY( ID )
*
* ====================================================================================== */
SQLWCHAR *build_query_to_compute_distance( gsl_matrix *query, int chunk, int dimensions )
{
	char *query_str = (char *)malloc( sizeof(char)*( 1000000 ) );

	/* compute constant_c */
	float constant_c;
	if( strcmp(NORM_TYPE, "L1") == 0 )
		constant_c = 2;

	/* get lowest dimension */
	int w; 
	int current_dim = TOTAL_DIMENSIONS;
	for( w = 0; w < NUM_PROJECTIONS; w++ )
		current_dim /= WINDOWS[w];

	/* select qugery vector from matrix */
	int proj_step, q;
	for( proj_step = NUM_PROJECTIONS; proj_step >= 0; proj_step-- )
	{  
		/* update db_table_name for current dimension */
		DB_TABLE_NAME = (char *)malloc(sizeof(char)*(40));
		
		if( BILLION_DATASET == 1 )
			sprintf(DB_TABLE_NAME, "[dbo].[%dbillion_%d_%s]", chunk, current_dim, NORM_TYPE);
		else
			sprintf(DB_TABLE_NAME, "[dbo].[%s_%d_%s]", DATASET_ROOT_NAME, current_dim, NORM_TYPE);

		/* get current query */
		double *query_vec = (double *)malloc(sizeof( double )*(current_dim + 1));
		for( q = 0; q < current_dim; q++ )
			query_vec[q] = gsl_matrix_get( query, proj_step, q );

		/* start building query here. The final query will be a concatenation of several sql queries */
		if(strcmp( NORM_TYPE, "L1" ) == 0)
			query_str = build_query_to_compute_L1_distance( query_str, query_vec, current_dim, proj_step, constant_c);

		free( DB_TABLE_NAME );
		free( query_vec );

		current_dim *= WINDOWS[proj_step-1];
	}

	SQLWCHAR *sql_query = (SQLWCHAR *)malloc(sizeof(SQLWCHAR)*(strlen(query_str)+1));
	swprintf(sql_query, L"%hs", query_str );

	if( DEBUG_OPTION >= 1 )
		printf( "\n%ws\n", sql_query );
	//system("PAUSE");

	return sql_query;
}

/* ======================================================================================
*
* build_query_to_compute_L1_distance: creates an SQLWCHAR repreentation of the string:
*					       SELECT *
*						   FROM (   
*						          SELECT ABS( c_033 - q_033 )  + ABS( c_1 - q_1 ) + ABS( c_2 - q_2 ) + ABS( c_3 - q_3 ) as dist, ID
*								  FROM [dbo].[14billion_4_L1] 
*								  WHERE ID = 142803 OR ID = 142835 OR ID = 695
*								) AS temp
*						   WHERE temp.dist < 1000
*
* ====================================================================================== */
char *build_query_to_compute_L1_distance( char *previous_query, double *query_vec, int dimensions, int proj_step, double constant_c  )
{
	/* build string of the form 
	 * ABS( c_0 - 0.5 ) + ABS( c_1 - 0.5 ) + ABS( c_2 - 0.5 ) + ABS( c_3 - 0.5 ) 
	 */
	char *distance_str = concat_L1_norm( query_vec, dimensions );

	/* Allocate space */
	char *new_query = (char *)malloc(sizeof(char)*(1000000));

	if( proj_step == 0 )
		proj_step = 11;

	if( BILLION_DATASET == 0 && strlen( previous_query ) == 0 )
		constant_c = 4.0/5.0 - 0.1;

	/* build string */
	if( strlen( previous_query ) == 0 )
		sprintf( new_query, "SELECT * FROM ( SELECT ( %s )*%.4f AS DIST, ID FROM %s ) AS t%d WHERE DIST <= %.2f ", distance_str, constant_c, DB_TABLE_NAME, proj_step, EPSILON ); 
	else
		sprintf( new_query, "SELECT * FROM ( SELECT ( %s )*%.4f AS DIST, u%d.ID FROM %s AS u%d, (%s) AS u%d WHERE u%d.ID = u%d.ID ) as temp%d WHERE temp%d.DIST <= %.2f", 
			distance_str, constant_c, proj_step, DB_TABLE_NAME, proj_step, previous_query, proj_step-1, proj_step, proj_step-1, proj_step, proj_step, EPSILON ); 

	if( DEBUG_OPTION >= 1 )
		printf( "\n%s\n", new_query );

	free( previous_query );
	free( distance_str );

	return new_query;
}



/* ======================================================================================
*
* concat_query: concats a query N times. Auxiliary function for function build_query_to_create_table.
*			Builds a query of the form: c_0 FLOAT, c_1 FLOAT, c_2 FLOAT, ..., c_dims FLOAT
*
*      * query - the query to be repeated
*	   * dims - the number of times the query will be repeated
*
* ====================================================================================== */
char *concat_L1_norm( double *query_vec, int dims )
{
	char *query_str = (char*)malloc(sizeof(char)*(30*dims));
	sprintf( query_str, "ABS(c_0-%.2f)", abs(query_vec[0]) );

	int i;
	for (i = 1; i < dims; i++)
	{
		/* allocate memory for temporary string */
		char *temp = (char *)malloc(sizeof(char)*( dims*30 ));
		sprintf(temp, "+ABS(c_%d-%.2f )", i, abs(query_vec[i]));

		/* concat the query */
		strcat(query_str, temp);

		/* temporary string is no longer needed for this iteration */
		free(temp);
	}
	
	return query_str;
}

/* ======================================================================================
*
* concat_query: concats a query N times. Auxiliary function for function build_query_to_create_table.
*			Builds a query of the form: c_0 FLOAT, c_1 FLOAT, c_2 FLOAT, ..., c_dims FLOAT
*
*      * query - the query to be repeated
*	   * dims - the number of times the query will be repeated
*
* ====================================================================================== */
char *concat_L2_norm( char *query_str, double *query_vec, int dims )
{
	char *query_str = (char*)malloc(sizeof(char)*(50*dims));
	sprintf( query_str, "POWER(c_%d-%f,2)", abs(query_vec[0]) );

	int i;
	for (i = 1; i < dims; i++)
	{
		/* allocate memory for temporary string */
		char *temp = (char *)malloc(sizeof(char)*( dims*50 ));
		sprintf(temp, " +POWER(c_%d-%f,2)", i, abs(query_vec[i]));

		/* concat the query */
		strcat(query_str, temp);

		/* temporary string is no longer needed for this iteration */
		free(temp);
	}

	return query_str;
}

/* ======================================================================================
*
* concat_query: concats a query N times. Auxiliary function for function build_query_to_create_table.
*			Builds a query of the form: c_0 FLOAT, c_1 FLOAT, c_2 FLOAT, ..., c_dims FLOAT
*
*      * query - the query to be repeated
*	   * dims - the number of times the query will be repeated
*
* ====================================================================================== */
char *concat_IDs( char *query_str, long *IDs, int length_ids )
{
	int i;
	for (i = 1; i < length_ids; i++)
	{
		/* allocate memory for temporary string */
		char *temp = (char *)malloc(sizeof(char)*( 1000 + length( IDs[i] )));

		sprintf(temp, " OR ID = %ld", (long)IDs[i]);

		/* concat the query */
		strcat(query_str, temp);

		/* temporary string is no longer needed for this iteration */
		free(temp);
	}
	
	return query_str;
}

/* ======================================================================================
*
* build_query_to_compute_L2_distance: creates an SQLWCHAR repreentation of the string:
*					       ALTER TABLE <table_name> ADD PRIMARY KEY( ID )
*
* ====================================================================================== */
SQLWCHAR *build_query_to_compute_L2_distance( double *query_vec, int dimensions, long *IDs, int length_ids )
{
	/* allocate space for string */
	char *query_str = (char *)malloc(sizeof(char)*( 200 + strlen(DB_TABLE_NAME) + dimensions*25 + 30*length_ids ));

	/* Build string representation of the query */
	char *init_str = (char *)malloc(sizeof(char)*100);
	sprintf(init_str, "SELECT * FROM ( SELECT SQRT( POWER( c_0 - %f, 2 ) + ", query_vec[0]);

	/* add ABS( c_%d - %f ) for each dimension of the table */
	init_str = concat_L2_norm(init_str, query_vec, dimensions);

	/* add database table name */
	char *db_name_str;
	if( length_ids != 0 )
	{
		db_name_str = (char *)malloc(sizeof(char)*( 25 + strlen( DB_TABLE_NAME ) + length(IDs[0])) );
		sprintf(db_name_str, " FROM %s WHERE ID = %ld OR ", DB_TABLE_NAME, IDs[0]);

		db_name_str = concat_IDs( db_name_str, IDs, length_ids );
	}
	else
	{
		db_name_str = (char *)malloc(sizeof(char)*( 25 + strlen( DB_TABLE_NAME ) ) );
		sprintf(db_name_str, " FROM %s ", DB_TABLE_NAME);
	}

	/* build complete query */
	sprintf(query_str, "%s%s) As temp WHERE temp.dist <= %f", init_str, db_name_str, EPSILON );
	
	/* convert to SQLWCHAR */
	SQLWCHAR *query = (SQLWCHAR *)malloc(sizeof(SQLWCHAR)*strlen(query_str)+1);
	swprintf(query, strlen(query_str)+1, L"%hs;", query_str);

//	free( init_str );
//	free( db_name_str );
//	free( query_str );

	/* print the query for debugging purposes */
	if (DEBUG_OPTION > 1) printf("%ws\n\n", query);

	return query;

}


/* ======================================================================================
*
* length: auxiliary function that computes the number of digits of an integer
*
*      * number - an  integer
*
* ====================================================================================== */
int length( long number )
{
	int size = 0;

	/* keep performing the integer division of the input number by 10 until it this 
	 * division reaches zero. The length of the number will be given by the number
	 * of times this operation is executed */
	while(TRUE)

		if( number == 0 )
			return size;
		else
		{
			number /= 10;
			size++;
		}

	/* return the length of the input integer */
	return size;
}