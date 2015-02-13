/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* query.hpp
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

#ifndef __Heidi__query__
#define __Heidi__query__

#include <string.h>
#include <gsl\gsl_matrix.h>


#include "constants.hpp"
#include "database.hpp"
#include "projection.hpp"



SQLWCHAR *build_connection_str(char *driver, char *server, char *database);

SQLWCHAR *build_query_to_import_data();

SQLWCHAR *build_query_to_create_table(int dims);

SQLWCHAR *build_query_to_select_data(long long start_indx, long long end_indx);

SQLWCHAR *build_query_to_index_table();

SQLWCHAR *build_query_to_add_pkey();

SQLWCHAR *build_query_to_compute_distance( gsl_matrix *query, int chunk, int dimensions );

char *build_query_to_compute_L1_distance( char *previous_query, double *query_vec, int dimensions, int proj_step, double constant_c  );

SQLWCHAR *build_query_to_compute_L2_distance(double *query_vec, int dimensions, long *IDs, int length_ids);

char *concat_L1_norm( double *query_vec, int dims );

char *concat_L2_norm( char *query_str, double *query_vec, int dims );

char *concat_IDs( char *query_str, long *IDs, int length_ids );

char *concat_query(char *query, int dims);

int length( long number );

#endif /* defined(__query__) */
