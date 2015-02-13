/* ======================================================================================
* This software is in the public domain, furnished "as is", without technical
* support, and with no warranty, express or implied, as to its usefulness for
* any purpose.
*
* projection.hpp
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


#ifndef __Heidi__projection__
#define __Heidi__projection__

#include "constants.hpp"
#include "database.hpp"
#include "input_manipulation.hpp"

#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>

/* 
 *
 */
gsl_matrix *compute_subspace( double *query );
/*
 *
 */
void compute_dimensions(int proj_step, int *prev_dim, int *current_dim);

/*
 *
 */
void project_data(HDBC hdbc, int proj_step, int current_dim);

/*
 *
 */
void project_database(HDBC hdbc);

int flength_ids( long *IDs );

long *perform_query(HDBC hdbc);

/*
 *
 */
void compute_norm(gsl_matrix *projected_data, gsl_matrix **projected_data_full, int row_indx, int window_size, int column_indx);

/*
 *
 */
int compute_num_chunks();

/*
 *
 */
void update_table_name(int dims);

/*
 *
 */
void update_dataset_path(int dims);

/*
 *
 */
void update_dataset_root_name( );

/*
 *
 */
gsl_matrix * orthogonal_projection_matrix(int dims1, int dims2);

/*
 *
 */
void write_projection_data(FILE *file, gsl_matrix *projected_data, long num_rows, long num_columns);

/*
 *
 */
void compute_orthogonal_projection(gsl_matrix *projection_matrix, gsl_matrix *matrix_database, gsl_matrix **projected_data, int dim, int window, int chunk_indx, int number_chunks_to_read, long number_remaining_vectors);

/*
 *
 */
void compute_orthogonal_projection_query(gsl_matrix *projection_matrix, gsl_matrix *query, gsl_matrix **projected_data, int dim, int window );

/*
 *
 */
void print_gsl_matrix(gsl_matrix *matrix, int dim1, int dim2);

/*
 *
 */
gsl_matrix *load_data_chunk(HDBC hdbc, int indx, int total_chunks, long num_vecs, int dims);

/*
 *
 */
int compute_num_vecs_to_load(int current_chunk, int total_chunks);

/*
 *
 */
void print_gsl_vector(gsl_vector *vector, int dim);

/*
 *
 */
FILE * open_file();

#endif /* defined(__Heidi__projection__) */
