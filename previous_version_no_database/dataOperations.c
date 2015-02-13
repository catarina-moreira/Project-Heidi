/*
 * dataOperations.c
 *
 *  Created on: 29 de Jun de 2012
 *      Author: twi
 */

#include "auxiliaryFunctions.h"
#include "dataStructures.h"
#include "fileHandling.h"

#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_VALUE	1

#define READ_MODE "r"
#define WRITE_MODE "w"

#define DATABASE "database"
#define VECTOR "vector"
#define DIM "dim"
#define QUERY_VECTOR "queryVector"
#define TXT ".txt"

#define MKDIR "mkdir"

#define WRITING_SINGLE_FILE 0
#define WRITING_GLOBAL_FILE 1


/* ----------------------------------------------------------------------------
   calculate_mean_value
---------------------------------------------------------------------------- */
double calculate_mean_value( double new_value, double current_mean, int iteration_step )
{
	/* validate the input */
	if( iteration_step < 1 )
	{ puts( "\nError: calculate_mean_value - invalid input!\n" ); }


	/* calculate the mean value */
	if( iteration_step == 1 )
	{
		/* if it is the first value of the vector, no operation is needed */
		return new_value;
	}

	/* calculate the mean value and return it */
	return ( ( current_mean * (iteration_step - 1) ) + new_value ) / iteration_step;
}


/* ----------------------------------------------------------------------------
   calculate_mean_values
---------------------------------------------------------------------------- */
int calculate_mean_values( char * database_path, char * working_directory, int number_existing_vectors, int vectors_lenght, double * mean_values, int first_projection_step,
		int previous_dimension, int debug_on )
{
	int number_read_vectors = 0;
	int number_read_components = 0;


	/* value of the input data */
	double data_value = 0.0;


	char * filename = NULL;


	FILE * data_file = NULL;



	/* verify the input */
	if( number_existing_vectors < 1 || vectors_lenght < 1 || mean_values == NULL || first_projection_step < 0 )
	{ puts( "\nError: calculate_mean_values - invalid input!\n" ); return 1; }


	if( debug_on > 1 )
	{ puts( "\n--- Mean Values Start ---\n" ); }



	if( first_projection_step )
	{
		/* we are in the first projection step. all the values are read from the original database file */

		if( database_path == NULL )
		{ puts( "\nError: calculate_mean_values - invalid database path!\n" ); return 1; }


		if( open_file( database_path, READ_MODE, &data_file ) )
		{ puts( "\nError: calculate_mean_values - problem in open_file (Mark1)!\n" ); return 1; }


		/* discard first two values (number of existing vectors and original vectors length) */
		fscanf( data_file, "%*i %*i" );
	}



	/*
	 * reserve memory for paths like
	 * .../project/data/dim20/vector2.txt
	 */
	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( DIM ) + count_number_digits( previous_dimension ) + 1 +
			strlen( VECTOR ) + count_number_digits( number_existing_vectors ) + strlen( TXT ) + 1 ) * sizeof( char ) );



	/* read the input file and calculate vector's mean values */
	while( number_read_vectors < number_existing_vectors )
	{
		if( debug_on > 0 )
		{
			if( (number_read_vectors + 1) % 1000 == 0 )
			{
				printf( "\ncalculate_mean_values - vector#%i\n", number_read_vectors + 1 );
			}
		}



		if( !first_projection_step )
		{
			/*
			 * we are not in the first projection step. the input file will be one of the individual vector file
			 * filename is something like: /.../project/data/dim20/vector2.txt
			 * build the path
			 */
			sprintf( filename, "%s%s%i/%s%i%s", working_directory, DIM, previous_dimension, VECTOR, number_read_vectors, TXT );



			if( open_file( filename, READ_MODE, &data_file ) )
			{ puts( "\nError: calculate_mean_values - problem in open_file (Mark2)!\n" ); return 1; }
		}


		while( number_read_components < vectors_lenght )
		{
			fscanf( data_file, "%lf", &data_value );


			if( debug_on > 1 )
			{ printf( " %f ", data_value ); }


			/* calculate the mean value */
			mean_values[number_read_components] = calculate_mean_value( data_value, mean_values[number_read_components], number_read_vectors + 1 );

			number_read_components++;
		}


		if( !first_projection_step )
		{
			close_file( data_file );
		}


		number_read_components = 0;
		number_read_vectors++;


		if( debug_on > 1 )
		{ putchar( '\n' ); }
	}



	/* free the allocated memory */
	free( filename );



	if( first_projection_step )
	{
		close_file( data_file );
	}



	if( debug_on > 1 )
	{ puts( "\n--- Mean Values End ---" ); }



	return 0;
}


/* ----------------------------------------------------------------------------
   calculate_covariance_matrix
---------------------------------------------------------------------------- */
int calculate_covariance_matrix( double * vector, int window_size, double ** covariance_matrix, int number_read_vectors, double * mean_values, int number_existing_vectors )
{
	int i = 0;
	int j = 0;

	/* verify input */
	if( vector == NULL || window_size < 1 || covariance_matrix == NULL || number_read_vectors < 0 || mean_values == NULL || number_existing_vectors < 1 )
	{ puts( "\nError: calculate_covariance_matrix - invalid input!\n" ); return 1; }


	/* go to each position of the covariance matrix and calculate its value */
	for( i = 0; i < window_size; i++ )
	{
		for( j = i; j < window_size; j++ )
		{
			covariance_matrix[i][j] = ( ( covariance_matrix[i][j] * ( number_read_vectors - 1 ) ) + ( ( vector[i] - mean_values[i] ) * ( vector[j] - mean_values[j] ) ) ) / number_read_vectors;


			/* convert from population to sample */
			if( number_read_vectors == number_existing_vectors )
			{
				covariance_matrix[i][j] = ( covariance_matrix[i][j] * number_existing_vectors ) / ( number_existing_vectors - 1 );

				if( i != j )
				{
					covariance_matrix[j][i] = covariance_matrix[i][j];
				}
			}
		}
	}

	return 0;
}


/* ----------------------------------------------------------------------------
   calculate_covariance_matrices
---------------------------------------------------------------------------- */
int calculate_covariance_matrices( char * database_path, char * working_directory, int number_existing_vectors, int vectors_lenght, int window_size, double * mean_values, double **** covariance_matrices,
		int * number_existing_matrices, int debug_on, int first_projection_step, int previous_dimension )
{
	int i = 0;
	int j = 0;
	int matrix_marker = 0;

	int number_read_vectors = 0;
	int number_read_components = 0;

	/* auxiliary structure to hold vector values to calculate covariance matrix */
	double * vector_values = NULL;

	/* auxiliary structure to hold mean values to calculate covariance matrix */
	double * mean_values_aux = NULL;

	char * filename = NULL;

	FILE * input_file = NULL;





	/* verify input */
	if( number_existing_vectors < 1 || vectors_lenght < 1 || window_size < 1 || mean_values == NULL || covariance_matrices == NULL || previous_dimension < 1 )
	{ puts( "\nError: calculate_covariance_matrices - invalid input!\n" ); return 1; }



	if( debug_on > 1 )
	{ puts( "\n--- Covariance Matrices Start ---" ); }



	/* number of existing covariance matrices, which is given by ceiling the vector's length divided by window_size */
	(* number_existing_matrices ) = 1 + ( (vectors_lenght - 1) / window_size );





	/* create auxiliary structure to hold vector values to calculate covariance matrix  */
	vector_values = ( double * ) malloc( window_size * sizeof( double ) );

	/* create auxiliary structure to hold mean values to calculate covariance matrix  */
	mean_values_aux = ( double * ) malloc( window_size * sizeof( double ) );




	/* create structure to hold the covariance matrices */
	(* covariance_matrices) = ( double *** ) malloc( (* number_existing_matrices ) * sizeof( double ** ) );



	if( debug_on > 0 )
	{ matrix_marker = ( (* number_existing_matrices ) * window_size ) / 10; }



	for( i = 0; i < (* number_existing_matrices ); i++ )
	{
		if( debug_on > 0 )
		{
			if( matrix_marker > 0 )
			{
				if( (i + 1) % matrix_marker == 0 )
				{ printf( "\ncalculate_covariance_matrices - matrix#%i\n", i + 1 );	}
			}
		}


		(* covariance_matrices)[i] = ( double ** ) malloc( window_size * sizeof( double * ) );

		for( j = 0; j < window_size; j++ )
		{
			(* covariance_matrices)[i][j] = ( double * ) malloc( window_size * sizeof( double ) );
		}
	}




	if( first_projection_step )
	{
		/* we are in the first projection step. all the values are read from the original database file */

		if( database_path == NULL )
		{ puts( "\nError: calculate_covariance_matrices - invalid database path!\n" ); return 1; }


		if( open_file( database_path, READ_MODE, &input_file ) )
		{ puts( "\nError: calculate_covariance_matrices - problem in open_file (Mark1)!\n" ); return 1; }


		/* discard first two values (number of existing vectors and original vectors length) */
		fscanf( input_file, "%*i %*i" );
	}



	/*
	 * reserve memory for paths like
	 * .../project/data/dim20/vector2.txt
	 */
	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( DIM ) + count_number_digits( previous_dimension ) + 1 +
			strlen( VECTOR ) + count_number_digits( number_existing_vectors ) + strlen( TXT ) + 1 ) * sizeof( char ) );



	/* read the input file and calculate covariance matrices */
	while( number_read_vectors < number_existing_vectors )
	{
		if( debug_on > 0 )
		{
			if( (number_read_vectors + 1) % 1000 == 0 )
			{
				printf( "\ncalculate_covariance_matrices - vector#%i\n", number_read_vectors + 1 );
			}
		}


		if( !first_projection_step )
		{
			/*
			 * we are not in the first projection step. the input file will be one of the individual vector file
			 * filename is something like: /.../project/data/dim20/vector2.txt
			 * build the path
			 */
			sprintf( filename, "%s%s%i/%s%i%s", working_directory, DIM, previous_dimension, VECTOR, number_read_vectors, TXT );



			if( open_file( filename, READ_MODE, &input_file ) )
			{ puts( "\nError: calculate_covariance_matrices - problem in open_file (Mark2)!\n" ); return 1; }
		}



		while( number_read_components < vectors_lenght )
		{
			/* scan input value and place it in auxiliary structure */
			fscanf( input_file, "%lf", &vector_values[ number_read_components % window_size ] );



			/* load the corresponding mean value */
			mean_values_aux[ number_read_components % window_size ] = mean_values[ number_read_components ];


			number_read_components++;



			/* verify if all the vector's components for this window have been read */
			if( number_read_components % window_size == 0 )
			{
				if( debug_on > 1 )
				{
					printf( "\nWindow #%i\n", number_read_components / window_size );

					printf( "\nVector Values: " );

					for( i = 0; i < window_size; i++ )
					{ printf( "%f ", vector_values[i] ); }
					putchar( '\n' );

					printf( "\nMean Values: " );

					for( i = 0; i < window_size; i++ )
					{ printf( "%f ", mean_values_aux[i] ); }
					putchar( '\n' );
				}


				/* all the vector's components for the window have been read, now calculate covariance matrix */
				calculate_covariance_matrix( vector_values, window_size, (* covariance_matrices)[ ( number_read_components / window_size ) - 1 ],
						number_read_vectors + 1, mean_values_aux, number_existing_vectors );
			}
		}


		if( !first_projection_step )
		{
			close_file( input_file );
		}


		number_read_components = 0;
		number_read_vectors++;
	}



	/* free the allocated memory */
	free( filename );



	if( first_projection_step )
	{
		close_file( input_file );
	}


	free( vector_values );
	free( mean_values_aux );



	if( debug_on > 1 )
	{ puts( "\n--- Covariance Matrices End ---" ); }


	return 0;
}


/* ----------------------------------------------------------------------------
   calculate_matrix_eigen_values_and_vectors
---------------------------------------------------------------------------- */
int calculate_matrix_eigen_values_and_vectors( double ** covariance_matrix, double * eigen_value, double * eigen_vector, int window_size, int debug_on )
{
	int i = 0;
	int j = 0;

	/* auxiliary variable to perform calculation on vector length */
	double vector_length = 0;

	/* auxiliary structure to hold covariance matrix's values during calculations */
	double * covariance_matrix_aux = NULL;

	gsl_matrix_view cov_matrix;
	gsl_vector * eigen_values_gsl;
	gsl_matrix * eigen_vectors_gsl;
	gsl_eigen_symmv_workspace * workspace;


	/* verify input */
	if( covariance_matrix == NULL || eigen_value == NULL || eigen_vector == NULL || window_size < 1 )
	{ puts( "\nError: calculate_matrix_eigen_values_and_vectors - invalid input!\n" ); return 1; }



	/* create auxiliary structure of covariance matrix for calculations */
	covariance_matrix_aux = ( double * ) malloc( ( window_size * window_size ) * sizeof( double ) );



	/* copy covariance matrix values into auxiliary structure */
	/* copy from a matrix like structure into a flat array structure */
	for( i = 0; i < window_size; i++ )
	{
		for( j = 0; j < window_size; j++ )
		{
			covariance_matrix_aux[ (i * window_size) + j ] = covariance_matrix[i][j];
		}
	}



	/* reshape the above vector into a WINDOW x WINDOW matrix */
	cov_matrix = gsl_matrix_view_array( covariance_matrix_aux, window_size, window_size );


	/* allocate memory for the eigen values and eigen vectors */
	eigen_values_gsl = gsl_vector_alloc( window_size );
	eigen_vectors_gsl = gsl_matrix_alloc( window_size, window_size );


	/* Allocate memory for a workspace. It is in this workspace where the calculation of the eigen values will occur */
	workspace = gsl_eigen_symmv_alloc( window_size );


	/* compute the eigen values and eigen vectors in the defined workspace*/
	gsl_eigen_symmv( &cov_matrix.matrix, eigen_values_gsl, eigen_vectors_gsl, workspace );


	/* don't need the workspace anymore, so free it */
	gsl_eigen_symmv_free( workspace );



	if( debug_on > 1 )
	{
		printf( "\nEigen Values:\n" );
		gsl_vector_fprintf( stdout, eigen_values_gsl, "%g");


		/*
		 * the vectors are along the columns, not lines, in the matrix
		 * v1 v2
		 * v1 v2
		 */
		printf( "\nEigen Vectors:\n" );
		for( i = 0; i < window_size; i++)
		{ for( j = 0; j < window_size; j++)
		{ printf( "%g \t", gsl_matrix_get( eigen_vectors_gsl, j , i ) ); }
		putchar( '\n' );
		}
		putchar( '\n' );
	}



	/* store the highest eigen value and respective eigen vector */
	for( i = 0; i < window_size; i++ )
	{
		if( gsl_vector_get ( eigen_values_gsl , i ) > (* eigen_value) )
		{
			/* the current eigen value is higher than the stored one, update it */
			(* eigen_value ) = gsl_vector_get ( eigen_values_gsl , i );



			/* update eigen vector */

			/*
			 * the vectors are along the columns, not lines, in the matrix
			 * v1 v2
			 * v1 v2
			 */
			for( j = 0; j < window_size; j++ )
			{
				eigen_vector[j] = gsl_matrix_get( eigen_vectors_gsl, j, i );
			}
		}
	}


	if( debug_on > 1 )
	{
		printf( "\nOriginal Eigen Vector: \t\t" );
		for( i = 0; i < window_size; i ++ )
		{ printf( "%f ", eigen_vector[i] );	}
	}


	/*
	 * normalize the eigen vector
	 */

	/* calculate the vector's length */

	for( i = 0; i < window_size; i++ )
	{
		vector_length += pow( eigen_vector[i], 2.0 );
	}

	vector_length = sqrt( vector_length );


	/* normalize each vector component */
	for( i = 0; i < window_size; i++ )
	{
		eigen_vector[i] /= vector_length;
	}


	if( debug_on > 1 )
	{
		printf( "\nNormalized Eigen Vector: \t" );
		for( i = 0; i < window_size; i ++ )
		{ printf( "%f ", eigen_vector[i] );	}

		putchar( '\n' );
		putchar( '\n' );
	}


	gsl_vector_free( eigen_values_gsl );
	gsl_matrix_free( eigen_vectors_gsl );

	free( covariance_matrix_aux );

	return 0;
}


/* ----------------------------------------------------------------------------
   calculate_eigen_values_and_vectors
---------------------------------------------------------------------------- */
int calculate_eigen_values_and_vectors( double *** covariance_matrices, double *** eigen_values, double *** eigen_vectors, int number_existing_matrices, int window_size, int debug_on )
{
	int i = 0;
	int j = 0;

	int matrix_marker = 0;

	/* verify input */
	if( covariance_matrices == NULL || eigen_values == NULL || eigen_vectors == NULL || number_existing_matrices < 1 || window_size < 1 )
	{ puts( "\nError: calculate_eigen_values_and_vectors - invalid input!\n" ); return 1; }



	/* create structure to hold the eigen values */
	(* eigen_values) = ( double ** ) malloc( number_existing_matrices * sizeof( double * ) );

	for( i = 0; i < number_existing_matrices; i++ )
	{
		(* eigen_values)[i] = ( double * ) malloc( sizeof( double ) );

		(* (* eigen_values)[i] ) = 0.0;
	}



	/* create structure to hold the eigen vectors */
	(* eigen_vectors) = ( double ** ) malloc( number_existing_matrices * sizeof( double * ) );

	for( i = 0; i < number_existing_matrices; i++ )
	{
		(* eigen_vectors)[i] = ( double * ) malloc( window_size * sizeof( double ) );

		for( j = 0; j < window_size; j++ )
		{
			(* eigen_vectors)[i][j] = 0.0;
		}
	}


	if( debug_on > 0 )
	{ matrix_marker = ( number_existing_matrices * window_size ) / 10; }



	/* calculate the eigen values and eigen vectors of each existing matrix */
	for( i = 0; i < number_existing_matrices; i++ )
	{
		if( debug_on > 0 )
		{
			if( matrix_marker > 0 )
			{
				if( (i + 1) % matrix_marker == 0 )
				{ printf( "\ncalculate_eigen_values_and_vectors - eigenVector#%i\n", i + 1 ); }
			}
		}


		calculate_matrix_eigen_values_and_vectors( covariance_matrices[i], (* eigen_values)[i], (* eigen_vectors)[i], window_size, debug_on );
	}


	return 0;
}


/* ----------------------------------------------------------------------------
   calculate_projection_matrix
---------------------------------------------------------------------------- */
int calculate_projection_matrix( double * eigen_vector, double ** projection_matrix, int window_size )
{
	int i = 0;
	int j = 0;

	gsl_matrix_view eigen_vector_column;
	gsl_matrix_view eigen_vector_row;
	gsl_matrix_view vector_multiplication_matrix;

	double * vector_multiplication = ( double * ) malloc( (window_size * window_size) * sizeof( double ) );


	/* verify input */
	if( eigen_vector == NULL || projection_matrix == NULL || window_size < 1 )
	{ puts( "\nError: calculate_projection_matrices - invalid input!\n" ); return 1; }



	/* reshape the arrays into matrixes */
	eigen_vector_column = gsl_matrix_view_array( eigen_vector, window_size, 1 );
	eigen_vector_row = gsl_matrix_view_array( eigen_vector, 1, window_size );
	vector_multiplication_matrix = gsl_matrix_view_array( vector_multiplication, window_size, window_size );



	/* perform multiplication */
	gsl_blas_dgemm( CblasNoTrans, CblasNoTrans, 1.0, &eigen_vector_column.matrix, &eigen_vector_row.matrix, 0.0, &vector_multiplication_matrix.matrix );



	/* save the results into the projection matrices structure */
	for( i = 0; i < window_size; i++ )
	{
		for( j = 0; j < window_size; j++ )
		{
			projection_matrix[i][j] = vector_multiplication[ (i * window_size) + j ];
		}
	}


	free( vector_multiplication );


	return 0;
}


/* ----------------------------------------------------------------------------
   calculate_projection_matrices
---------------------------------------------------------------------------- */
int calculate_projection_matrices( double ** eigen_vectors, double **** projection_matrices, int number_existing_matrices, int window_size, int debug_on )
{
	int i = 0;
	int j = 0;

	int matrix_marker = 0;


	/* verify input */
	if( eigen_vectors == NULL || projection_matrices == NULL || number_existing_matrices < 1 || window_size < 1 )
	{ puts( "\nError: calculate_projection_matrices - invalid input!\n" ); return 1; }



	/* create structure to hold the projection matrices */
	(* projection_matrices) = ( double *** ) malloc( number_existing_matrices * sizeof( double ** ) );

	for( i = 0; i < number_existing_matrices; i++ )
	{
		(* projection_matrices)[i] = ( double ** ) malloc( window_size * sizeof( double * ) );

		for( j = 0; j < window_size; j++ )
		{
			(* projection_matrices)[i][j] = ( double * ) malloc( window_size * sizeof( double ) );
		}
	}



	if( debug_on > 0 )
	{ matrix_marker = ( number_existing_matrices * window_size ) / 10; }



	/* calculate the projection matrices */
	for( i = 0; i < number_existing_matrices; i++ )
	{
		if( debug_on > 0 )
		{
			if( matrix_marker > 0 )
			{
				if( (i + 1) % matrix_marker == 0 )
				{ printf( "\ncalculate_projection_matrices - eigenVector#%i\n", i + 1 ); }
			}
		}


		calculate_projection_matrix( eigen_vectors[i], (* projection_matrices)[i], window_size );
	}


	return 0;
}


/* ----------------------------------------------------------------------------
   set_up_current_previous_dimension

	using the current projection step and the original length of the vectors
	it analyzes the window values and sets up the current and previous
	dimension values
---------------------------------------------------------------------------- */
int set_up_current_previous_dimension( int vectors_original_length, int * window_values, int current_projection_step, int * current_dimension,
		int * previous_dimension )
{
	int i = 0;

	int current_dimension_aux = 0;

	int previous_dimension_aux = 0;



	/* validate the input */
	if( vectors_original_length < 1 || window_values == NULL || current_projection_step < 0 || current_dimension == NULL || previous_dimension == NULL )
	{ puts( "\nERROR: set_up_current_previous_dimension - invalid input!\n" ); return 1; }



	current_dimension_aux = vectors_original_length;

	do
	{
		previous_dimension_aux = current_dimension_aux;

		current_dimension_aux /= window_values[i];

		i++;
	}
	while( i <= current_projection_step );


	(* current_dimension) = current_dimension_aux;

	(* previous_dimension) = previous_dimension_aux;


	return 0;
}


/* ----------------------------------------------------------------------------
   project_window

   projects the given window using the given projection matrix
---------------------------------------------------------------------------- */
int project_window( double * vector_values, double ** projection_matrix, int window_size, char * computation_mode, double * projection_value )
{
	int row_index = 0;
	int multiplication_index = 0;


	double value = 0.0;

	double * projected_vector = NULL;





	/* verify input */
	if( vector_values == NULL || projection_matrix == NULL ||  window_size < 2 || computation_mode == NULL || projection_value == NULL )
	{ puts( "\nError: project_window - invalid input!\n" ); return 1; }



	/* create structure for the projected vector */
	projected_vector = ( double * ) malloc( window_size * sizeof( double ) );



	/* multiply projection matrix with values vector */
	for( row_index = 0; row_index < window_size; row_index++, value = 0.0 )
	{
		for( multiplication_index = 0; multiplication_index < window_size; multiplication_index++ )
		{
			/* multiply the values, using only 6 decimal digits */
			value += (floor(projection_matrix[row_index][multiplication_index] * 1000000 + 0.5) / 1000000) * (floor(vector_values[multiplication_index] * 1000000 + 0.5) / 1000000);
		}

		projected_vector[row_index] = value;
	}



	if( compute_vector_length( projected_vector, window_size, computation_mode, projection_value ) )
	{ puts( "\nError: project_window - error in compute_vector_length!\n" ); return 1; }



	free( projected_vector );



	return 0;
}


/* ----------------------------------------------------------------------------
   project_database
---------------------------------------------------------------------------- */
int project_database( char * working_directory, double *** projection_matrices, int * window_values, int vectors_original_length,
		int number_projections_to_make, int current_projection_step, char * computation_mode, int number_existing_vectors, char * database_path, int debug_on )
{
	int i = 0;

	int vector_index = 0;

	int previous_dimension = 0;

	int current_dimension = 0;

	int number_read_components = 0;


	double projection_value = 0.0;

	/* auxiliary structure to hold vector values */
	double * vector_values = NULL;


	char * filename = NULL;


	FILE * input_file = NULL;
	FILE * output_file = NULL;
	FILE * vector_copy_file = NULL;




	/* validate the input */
	if( working_directory == NULL || projection_matrices == NULL || window_values == NULL || vectors_original_length < 0 || number_projections_to_make < 1 ||
			current_projection_step < 0 || computation_mode == NULL || number_existing_vectors < 1 )
	{ puts( "\nERROR: project_database - invalid input!\n" ); return 1; }



	if( debug_on > 1 )
	{ puts( "\n--- Project Database Start --- " ); }



	/* set up current and previous dimensions values */
	set_up_current_previous_dimension( vectors_original_length, window_values, current_projection_step, &current_dimension, &previous_dimension );


	/* create auxiliary structure to hold vector values to perform the projection */
	vector_values = ( double * ) malloc( window_values[ current_projection_step ] * sizeof( double ) );




	/*
	 * this memory reserves are placed in here to dont overload the OS with memory allocation in each vector cycle
	 * to do this it was necessary to change 'project_window' function and add as arguments two variables:
	 * projection_matrix_flat and projected_window_flat
	 */


	/*
	 * reserve memory for paths like
	 * .../project/data/dim20/vector2.txt
	 *
	 * (see which of the paths has the bigger length so that we reserve enough memory to create file paths)
	 */
	if( (strlen( working_directory ) + strlen( DIM ) + count_number_digits( previous_dimension ) + 1 + strlen( VECTOR ) +
			count_number_digits( number_existing_vectors ) + strlen( TXT ) + 1) >= (strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( DATABASE ) +
					1 + strlen( DIM ) + count_number_digits( current_dimension ) + strlen( TXT ) + 1) )
	{
		filename = ( char * ) malloc( strlen( working_directory ) + strlen( DIM ) + count_number_digits( previous_dimension ) + 1 + strlen( VECTOR ) +
				count_number_digits( number_existing_vectors ) + strlen( TXT ) + 1 * sizeof( char ) );
	}
	else
	{
		filename = ( char * ) malloc( strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( DATABASE ) +
				1 + strlen( DIM ) + count_number_digits( current_dimension ) + strlen( TXT ) + 1 * sizeof( char ) );
	}





	/* go through all the existing vectors */
	for( vector_index = 0; vector_index < number_existing_vectors; vector_index++ )
	{
		if( debug_on > 0 )
		{
			if( (vector_index + 1) % 1000 == 0 )
			{
				printf( "\nproject_database - vector#%i\n", vector_index + 1 );
			}


			if( debug_on > 1 )
			{
				printf( "\nProjection Step: %i\n", current_projection_step );
				printf( "Current Dimension: %i\tPrevious Dimension: %i\n", current_dimension, previous_dimension );
				printf( "\nVector #%i\n", vector_index + 1 );
			}
		}




		/* set up input file */

		if( current_projection_step == 0 && vector_index == 0 )
		{
			/* this is the first projection step. The vectors values should be read from original database file */

			if( database_path == NULL )
			{ puts( "\nERROR: project_database - database_path == NULL!\n" ); return 1; }


			if( debug_on > 1 )
			{ printf( "\nInput file: %s\n", database_path ); }


			if( open_file( database_path, READ_MODE, &input_file ) )
			{ puts( "\nError: project_database - problem with opening input file (Mark1)!\n" ); return 1; }


			/* discard first two values (number of existing vectors and original vectors length) */
			fscanf( input_file, "%*i %*i" );
		}


		if( current_projection_step != 0 )
		{
			/*
			 * we are not in the first projection step. open previous dimension's individual projection values file
			 * filename is something like: /working_directory/dimPREVIOUS_DIMENSION/vectorVECTOR_INDEX.txt
			 * build the path
			 */
			sprintf( filename, "%s%s%i/%s%i%s", working_directory, DIM, previous_dimension, VECTOR, vector_index, TXT );


			if( debug_on > 1 )
			{ printf( "\nInput file: %s\n", filename ); }


			if( open_file( filename, READ_MODE, &input_file ) )
			{ puts( "\nError: project_database - problem with opening input file (Mark2)!\n" ); return 1; }
		}


		/* set output file */

		if( current_projection_step == 0 )
		{
			/*
			 * this is the first projection step. create single vector file for all vectors in database file to boost performance.
			 * filename is something like: /working_directory/dimPREVIOUS_DIMENSION/vectorVECTOR_INDEX.txt
			 * build the path
			 */
			sprintf( filename, "%s%s%i/%s%i%s", working_directory, DIM, previous_dimension, VECTOR, vector_index, TXT );


			if( debug_on > 1 )
			{ printf( "\nDatabase Copy: vector file: %s\n", filename ); }


			if( open_file( filename, WRITE_MODE, &vector_copy_file ) )
			{ puts( "\nError: project_database - problem with creating vector file!\n" ); return 1; }
		}



		if( current_projection_step < (number_projections_to_make - 1) )
		{
			/*
			 * we are not in the final projection step. each vector is projected into a individual file with the name:
			 * /working_directory/dimCURRENT_DIMENSION/vectorVECTOR_INDEX.txt
			 * build the path
			 */
			sprintf( filename, "%s%s%i/%s%i%s", working_directory, DIM, current_dimension, VECTOR, vector_index, TXT );


			if( debug_on > 1 )
			{ printf( "\nOutput file: %s\n", filename ); }


			if( open_file( filename, WRITE_MODE, &output_file ) )
			{ puts( "\nError: project_database - problem with creating output file (Mark1)!\n" ); return 1; }
		}



		if( current_projection_step == (number_projections_to_make - 1) && vector_index == 0 )
		{
			/*
			 * this is the final projection step. all the vectors are projected into a global file with the name:
			 * /working_directory/dimCURRENT_DIMENSION/database_dimCURRENT_DIMENSION.txt
			 * build the path
			 */
			sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, DATABASE, DIM, current_dimension, TXT );


			if( debug_on > 1 )
			{ printf( "\nOutput file: %s\n", filename ); }


			if( open_file( filename, WRITE_MODE, &output_file ) )
			{ puts( "\nError: project_database - problem with creating output file (Mark2)!\n" ); return 1; }
		}



		/* read the vector values and calculate the projections */

		number_read_components = 0;


		/*
		 * the vectors that we are going to analyze, it dimensions are equal to the previous dimension value.
		 * eg: if we are going to produce a dimension 20, and we are using a window size of 2, then the vectors that we are analyzing have dimension
		 * (20 * 2) = the previous dimension
		 */
		while( number_read_components < previous_dimension )
		{
			/* scan input file and place the value in the auxiliary structure */
			fscanf( input_file, "%lf", &vector_values[ number_read_components % window_values[ current_projection_step ] ] );



			/*
			 * create copy of original database
			 * each vector will be copied to a single file
			 */
			if( current_projection_step == 0 )
			{
				fprintf( vector_copy_file, "%f", vector_values[ number_read_components % window_values[ current_projection_step ] ] );

				if( number_read_components < (previous_dimension - 1) )
				{ fprintf( vector_copy_file, " " ); }
			}



			/* update the number of read components */
			number_read_components++;



			/* verify if already have been read as many vector values as the window size */
			if( number_read_components % window_values[ current_projection_step ] == 0 )
			{

				/* all the vector's components for the window have been read, now calculate the projection */
				if( project_window( vector_values, projection_matrices[ ( number_read_components / window_values[ current_projection_step ] ) - 1 ],
						window_values[ current_projection_step ], computation_mode, &projection_value ) )
				{ puts( "\nError: project_database - error in project_window!\n" ); return 1; }



				if( debug_on > 1 )
				{
					printf( "\nVector #%i\nWindow #%i\n", vector_index + 1, number_read_components / window_values[ current_projection_step ] );

					puts( "\nOriginal Values: " );
					for( i = 0; i < window_values[ current_projection_step ]; i++ )
					{ printf( "%f ", vector_values[i] ); }	putchar( '\n' );

					printf( "Projection Value: %f\n", projection_value );
				}



				/* save the projection value */
				if( save_projection_value( output_file, projection_value, number_read_components < previous_dimension ? 0 : 1,
						current_projection_step == (number_projections_to_make - 1) ? WRITING_GLOBAL_FILE : WRITING_SINGLE_FILE, vector_index < (number_existing_vectors - 1) ? 0 : 1 ) )
				{ puts( "\nError: project_database - error while saving projection value to file!\n" ); return 1; }
			}
		}



		if( current_projection_step == 0 )
		{
			close_file( vector_copy_file );



			if( number_projections_to_make > 1 )
			{
				if( debug_on > 1 )
				{ puts( "\nClose Output file (Mark 1)\n" ); }


				/* we are in the first projection step. finished projecting a vector, close its output file, but the original database file remains open for next vector */
				close_file( output_file );
			}
		}

		if( current_projection_step != 0 && current_projection_step < (number_projections_to_make - 1) )
		{
			if( debug_on > 1 )
			{ puts( "\nClose Input File (Mark 1)\nClose Output file (Mark 2)\n" ); }


			/*
			 * we are neither on the first or last projection step. finished projecting a vector, the next vector will read from a different input file and write to a
			 * different output file. close its input file and output file
			 */
			close_file( input_file );
			close_file( output_file );
		}

		if( current_projection_step == (number_projections_to_make - 1) && number_projections_to_make > 1 )
		{
			if( debug_on > 1 )
			{ puts( "\nClose Input file (Mark 2)\n" ); }


			/* we are in the last projection step. finished projecting a vector, close its input file, but the global output database file remains open for next vector */
			close_file( input_file );
		}
	}


	if( current_projection_step == 0 )
	{
		if( debug_on > 1 )
		{ puts( "\nClose Input file (Mark 3)\n" ); }


		close_file( input_file );
	}

	if( current_projection_step == (number_projections_to_make - 1) )
	{
		if( debug_on > 1 )
		{ puts( "\nClose Output file (Mark 3)\n" ); }


		close_file( output_file );
	}


	/* free the allocated memory */
	free( vector_values );
	free( filename );


	if( debug_on > 1 )
	{ puts( "\n--- Project Database End --- " ); }


	return 0;
}


/* ----------------------------------------------------------------------------
   project_query_vector
---------------------------------------------------------------------------- */
int project_query_vector( char * working_directory, double *** projection_matrices, int * window_values, int vectors_original_length, int number_projections_to_make,
		int current_projection_step, char * computation_mode, char * query_vector_number, int debug_on )
{
	int i = 0;

	int previous_dimension = 0;

	int current_dimension = 0;

	int number_read_components = 0;


	/* auxiliary structure to hold query vector values */
	double * query_values = NULL;

	double projection_value = 0.0;


	char * filename = NULL;


	FILE * input_file = NULL;
	FILE * output_file = NULL;




	/* validate the input */
	if( working_directory == NULL || projection_matrices == NULL || window_values == NULL || vectors_original_length < 0 || number_projections_to_make < 1 ||
			current_projection_step < 0	|| computation_mode == NULL || query_vector_number == NULL )
	{ puts( "\nERROR: project_query_vector - invalid input!\n" ); return 1; }




	/* set up current and previous dimensions values */
	set_up_current_previous_dimension( vectors_original_length, window_values, current_projection_step, &current_dimension, &previous_dimension );



	if( debug_on > 1 )
	{
		printf( "\n--- Project Query Vector - Dim %i --- \n", current_dimension );
		printf( "\nProjection Step: %i\n", current_projection_step );
	}



	/* create auxiliary structure to hold query vector values to perform the projection */
	query_values = ( double * ) malloc( window_values[ current_projection_step ] * sizeof( double ) );




	/*
	 * this memory reserves are placed in here to dont overload the OS with memory allocation in each vector cycle
	 * to do this it was necessary to change 'project_window' function and add as arguments two variables:
	 * projection_matrix_flat and projected_window_flat
	 */

	/*
	 * reserve memory for paths like
	 * .../project/data/dim20/vectorVector_dim20.txt
	 */
	filename = ( char * ) malloc( ( strlen( working_directory ) + strlen( DIM ) + count_number_digits( previous_dimension ) + 1 +
			strlen( QUERY_VECTOR ) + 1 + strlen( DIM ) + count_number_digits( previous_dimension ) + strlen( TXT ) + 1 ) * sizeof( char ) );



	/* set up input file */

	if( current_projection_step == 0 )
	{
		/* this is the first projection step. The query vector values should be read from original query vector file */

		if( working_directory == NULL )
		{ puts( "\nERROR: project_query_vector - working_directory == NULL!\n" ); return 1; }



		/*
		 * this is the first projection. open original query vector file
		 * filename is something like: /working_directory/queryVector1.txt
		 * build the path
		 */
		sprintf( filename, "%s%s%s%s", working_directory, QUERY_VECTOR, query_vector_number, TXT );



		if( debug_on > 1 )
		{ printf( "\nInput file: %s\n", filename ); }



		if( open_file( filename, READ_MODE, &input_file ) )
		{ puts( "\nError: project_query_vector - problem with opening input file (Mark1)!\n" ); return 1; }
	}


	if( current_projection_step != 0 )
	{
		/*
		 * it is not the first projection. open previous dimension's query vector file
		 * filename is something like: /working_directory/dimPREVIOUS_DIMENSION/queryVector_dimPREVIOUS_DIMENSION.txt
		 * build the path
		 */
		sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, previous_dimension, QUERY_VECTOR, DIM, previous_dimension, TXT );


		if( debug_on > 1 )
		{ printf( "\nInput file: %s\n", filename ); }


		if( open_file( filename, READ_MODE, &input_file ) )
		{ puts( "\nError: project_query_vector - problem with opening input file (Mark2)!\n" ); return 1; }
	}


	/* set output file */

	/*
	 * output filename is something like: /working_directory/dimCURRENT_DIMENSION/queryVector_dimCURRENT_DIMENSION.txt
	 * build the path
	 */
	sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, QUERY_VECTOR, DIM, current_dimension, TXT );


	if( debug_on > 1 )
	{ printf( "\nOutput file: %s\n", filename ); }


	if( open_file( filename, WRITE_MODE, &output_file ) )
	{ puts( "\nError: project_query_vector - problem with creating output file!\n" ); return 1; }


	/* free the allocated memory */
	free( filename );



	/*
	 * read the query vector values and calculate the projections
	 *
	 * the query vector that we are going to analyze, its dimension is equal to the previous dimension value.
	 * eg: if we are going to produce a dimension 20, and we are using a window size of 2, then the query vector that we are analyzing has dimension
	 * (20 * 2) = the previous dimension
	 */
	for( number_read_components = 0; number_read_components < previous_dimension; )
	{
		/* scan input file and place the value in the auxiliary structure */
		fscanf( input_file, "%lf", &query_values[ number_read_components % window_values[ current_projection_step ] ] );


		/* update the number of read components */
		number_read_components++;



		/* verify if already have been read as many query vector values as the window size */
		if( number_read_components % window_values[ current_projection_step ] == 0 )
		{

			/* all the query vector's components for the window have been read, now calculate the projection */
			if( project_window( query_values, projection_matrices[ ( number_read_components / window_values[ current_projection_step ] ) - 1 ],
					window_values[ current_projection_step ], computation_mode, &projection_value ) )
			{ puts( "\nError: project_query_vector - error in project_window!\n" ); return 1; }



			if( debug_on > 1 )
			{
				printf( "\nWindow #%i\n", number_read_components / window_values[ current_projection_step ] );

				puts( "\nOriginal Query Vector Values: " );
				for( i = 0; i < window_values[ current_projection_step ]; i++ )
				{ printf( "%f ", query_values[i] ); }	putchar( '\n' );

				printf( "Projection Value: %f\n", projection_value );
			}



			/* save the projection value */
			if( save_projection_value( output_file, projection_value, number_read_components < previous_dimension ? 0 : 1,
					WRITING_SINGLE_FILE, 1 ) )
			{ puts( "\nError: project_query_vector - error while saving projection value to file!\n" ); return 1; }
		}
	}


	/* close files */
	close_file( input_file );
	close_file( output_file );



	free( query_values );


	if( debug_on > 1 )
	{ puts( "\n--- Project Query Vector End --- " ); }


	return 0;
}


/* ----------------------------------------------------------------------------
   execute_query
---------------------------------------------------------------------------- */
int execute_query( char * working_directory, int current_dimension, int number_existing_vectors, int current_query_step, char * computation_mode,
		struct queryElement ** query_result_list, double epsilon_value, int number_projections_to_make, char * database_path, char * query_vector_number, int debug_on )
{
	int vector_index = 0;

	int number_read_components = 0;


	double vector_component = 0.0;

	double distance_value = 0.0;

	double * query_vector_values = NULL;


	char * filename = NULL;


	FILE * vector_file = NULL;
	FILE * query_file = NULL;


	struct queryElement * current_list_element = NULL;

	struct queryElement * final_result_list = NULL;

	struct queryElement * element_to_delete = NULL;



	/* verify input */
	if( working_directory == NULL || current_dimension < 2 || number_existing_vectors < 1 || current_query_step < 0 || computation_mode == NULL
			|| query_result_list == NULL || epsilon_value < 0.0 || number_projections_to_make < 0 || query_vector_number == NULL )
	{ puts( "\nError: execute_query - invalid input!\n" ); return 1; }




	if( debug_on > 1 )
	{ puts( "\n--- Execute Query Start --- " ); }



	/* create auxiliary structure to hold query vector values to perform the calculations */
	query_vector_values = ( double * ) malloc( current_dimension * sizeof( double ) );



	/*
	 * reserve memory for paths like
	 * .../project/data/dim20/vector2.txt
	 *
	 * (see which of the paths has the bigger length so that we reserve enough memory to create file paths)
	 */
	if( (strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( QUERY_VECTOR ) +
			1 + strlen( DIM ) + count_number_digits( current_dimension ) + strlen( TXT ) + 1) >= (strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( VECTOR ) +
					count_number_digits( number_existing_vectors ) + strlen( TXT ) + 1) )
	{
		filename = ( char * ) malloc( strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( QUERY_VECTOR ) +
				1 + strlen( DIM ) + count_number_digits( current_dimension ) + strlen( TXT ) + 1 * sizeof( char ) );
	}
	else
	{
		filename = ( char * ) malloc( strlen( working_directory ) + strlen( DIM ) + count_number_digits( current_dimension ) + 1 + strlen( VECTOR ) +
				count_number_digits( number_existing_vectors ) + strlen( TXT ) + 1 * sizeof( char ) );
	}



	if( current_query_step == 0 )
	{
		/* go through all the existing vectors */
		for( vector_index = 0; vector_index < number_existing_vectors; vector_index++ )
		{
			if( debug_on > 0 )
			{
				if( (vector_index + 1) % 1000 == 0 )
				{
					printf( "\nQuery Step: %i\n", current_query_step );
					printf( "Current Dimension: %i\n", current_dimension );
					printf( "\nanalyzed vectors #%i\n", vector_index + 1 );
				}
			}




			/* set up input files */

			if( number_projections_to_make == 0 && vector_index == 0 )
			{
				if( database_path == NULL )
				{ puts( "\nERROR: execute_query - database_path == NULL!\n" ); return 1; }


				if( debug_on > 1 )
				{ printf( "\nInput file: %s\n", database_path ); }


				if( open_file( database_path, READ_MODE, &vector_file ) )
				{ puts( "\nError: execute_query - problem with opening database file!\n" ); return 1; }


				/* discard first two values (number of existing vectors and original vectors length) */
				fscanf( vector_file, "%*i %*i" );
			}



			if( vector_index == 0 )
			{
				if( number_projections_to_make > 0 )
				{
					/* open database file */

					/*
					 * this is the first query step. The vectors values should be read from global low dimension database file
					 *
					 * the file path is something like: /working_directory/dimCURRENT_DIMENSION/database_dimCURRENT_DIMENSION.txt
					 *
					 * build the path
					 */
					sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, DATABASE, DIM, current_dimension, TXT );



					if( debug_on > 1 )
					{ printf( "\nVector file: %s\n", filename ); }


					if( open_file( filename, READ_MODE, &vector_file ) )
					{ puts( "\nError: execute_query - problem with opening vector file (Mark1)!\n" ); return 1; }
				}



				/* open query vector file */

				if( number_projections_to_make > 0 )
				{
					/*
					 * the file path is something like: /working_directory/dimCURRENT_DIMENSION/queryVector_dimCURRENT_DIMENSION.txt
					 *
					 * build the path
					 */
					sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, QUERY_VECTOR, DIM, current_dimension, TXT );
				}
				else
				{
					/*
					 * the file path is something like: /working_directory/queryVector1.txt
					 *
					 * build the path
					 */
					sprintf( filename, "%s%s%s%s", working_directory, QUERY_VECTOR, query_vector_number, TXT );
				}


				if( debug_on > 1 )
				{ printf( "\nQuery file: %s\n", filename ); }


				if( open_file( filename, READ_MODE, &query_file ) )
				{ puts( "\nError: execute_query - problem with opening query file (Mark1)!\n" ); return 1; }
			}



			/* calculate the distance value, between the current vector and the query vector */
			for( number_read_components = 0, distance_value = 0.0; number_read_components < current_dimension; number_read_components++ )
			{
				/* read the component value from each file */
				fscanf( vector_file, "%lf", &vector_component );

				if( vector_index == 0 )
				{
					fscanf( query_file, "%lf", &query_vector_values[number_read_components] );
				}



				/* compute the distance value */
				if( compute_vector_distance( vector_component, query_vector_values[number_read_components], computation_mode, &distance_value,
						number_read_components == (current_dimension - 1) ? 1 : 0 ) )
				{ puts( "\nError: execute_query - problem in compute_vector_distance (Mark 1)!\n" ); return 1; }
			}



			if( debug_on > 1 )
			{ printf( "\nVector #%i - distance value: %f\n", vector_index + 1, distance_value ); }



			/* if distance value is lower or equal to epsilon, the vector is added to the result_list */
			if( distance_value <= epsilon_value )
			{
				/* add vector to result list */
				if( number_projections_to_make > 0 )
				{
					if( add_element_list_head( query_result_list, vector_index, distance_value ) )
					{ puts( "\nError: execute_query - problem in add_element_list_head!\n" ); return 1; }
				}
				else
				{
					if( add_element_list_sorted( query_result_list, vector_index, distance_value ) )
					{ puts( "\nError: execute_query - problem in add_element_list_sorted (Mark 1)!\n" ); return 1; }
				}
			}



			/* we read all the query vector components, we can close its file */
			if( vector_index == 0 )
			{ close_file( query_file ); }
		}


		close_file( vector_file );
	}



	if( current_query_step != 0 )
	{
		current_list_element = (* query_result_list);


		/* go through all the vectors in the result list */
		for( vector_index = 0; current_list_element != NULL; vector_index++ )
		{
			switch( debug_on )
			{
			case 0:
			{
				break;
			}

			case 1:
			{
				if( (vector_index + 1) % 100 == 0 )
				{
					printf( "\nQuery Step: %i\n", current_query_step );
					printf( "\nanalyzed vectors #%i\n", vector_index + 1 );
				}

				break;
			}

			case 2:
			{
				printf( "\nQuery Step: %i\n", current_query_step );
				printf( "Current Dimension: %i\n", current_dimension );
				printf( "\nVector #%i\n", current_list_element->vector_number + 1 );
				printf( "\nanalyzed vectors #%i\n", vector_index + 1 );

				break;
			}
			}




			/* set up input files */


			/*
			 * open vector file. it is not the first query step.
			 * filename is something like: /working_directory/dimCURRENT_DIMENSION/vectorVECTOR_INDEX.txt
			 *
			 * build the path
			 */
			sprintf( filename, "%s%s%i/%s%i%s", working_directory, DIM, current_dimension, VECTOR, current_list_element->vector_number, TXT );


			if( debug_on > 1 )
			{ printf( "\nVector file: %s\n", filename ); }


			if( open_file( filename, READ_MODE, &vector_file ) )
			{ puts( "\nError: execute_query - problem with opening vector file (Mark2)!\n" ); return 1; }



			/* only want to open the query vector to load query values while dealing with the first element of the list */
			if( vector_index == 0 )
			{
				/* open query vector file */

				if( current_query_step < number_projections_to_make )
				{

					/*
					 * the file path is something like: /working_directory/dimCURRENT_DIMENSION/queryVector_dimCURRENT_DIMENSION.txt
					 *
					 * build the path
					 */
					sprintf( filename, "%s%s%i/%s_%s%i%s", working_directory, DIM, current_dimension, QUERY_VECTOR, DIM, current_dimension, TXT );
				}
				else
				{
					/*
					 * the file path is something like: /working_directory/queryVector.txt
					 *
					 * build the path
					 */
					sprintf( filename, "%s%s%s%s", working_directory, QUERY_VECTOR, query_vector_number, TXT );
				}


				if( debug_on > 1 )
				{ printf( "\nQuery file: %s\n", filename ); }


				if( open_file( filename, READ_MODE, &query_file ) )
				{ puts( "\nError: execute_query - problem with opening query file (Mark2)!\n" ); return 1; }
			}



			/* calculate the distance value, between the current vector and the query vector */
			for( number_read_components = 0, distance_value = 0.0; number_read_components < current_dimension; number_read_components++ )
			{
				/* read the component value from each file */
				fscanf( vector_file, "%lf", &vector_component );


				if( vector_index == 0 )
				{
					fscanf( query_file, "%lf", &query_vector_values[number_read_components] );
				}



				/* compute the distance value */
				if( compute_vector_distance( vector_component, query_vector_values[number_read_components], computation_mode, &distance_value,
						number_read_components == (current_dimension - 1) ? 1 : 0 ) )
				{ puts( "\nError: execute_query - problem in compute_vector_distance (Mark 2)!\n" ); return 1; }
			}



			if( debug_on > 1 )
			{ printf( "\nVector #%i - distance value: %f\n", current_list_element->vector_number + 1, distance_value ); }



			/* if distance value is bigger than epsilon, the vector is removed from the result_list */
			if( distance_value > epsilon_value )
			{
				element_to_delete = current_list_element;

				current_list_element = current_list_element->next_element;


				/* remove vector from result list */
				if( remove_element( query_result_list, element_to_delete ) )
				{ puts( "\nError: execute_query - problem in remove_element!\n" ); return 1; }
			}
			else
			{
				if( current_query_step == number_projections_to_make )
				{
					/*
					 * we are in the final query step and this vector is within epsilon range.
					 * add it to the final result list
					 */
					if( add_element_list_sorted( &final_result_list, current_list_element->vector_number, distance_value ) )
					{ puts( "\nError: execute_query - problem in add_element_list_sorted (Mark 2)!\n" ); return 1; }
				}
			}



			/* we read all the query vector components, we can close its file */
			if( vector_index == 0 )
			{ close_file( query_file ); }



			/*
			 * we need to close the vector input file, since each vector is in a individual file.
			 */
			close_file( vector_file );



			if( distance_value <= epsilon_value )
			{
				/*
				 * advance to next list element
				 * (if the distance value is bigger than epsilon, then the analyzed element was already removed from list
				 * and the list pointer was already advanced to the next list element)
				 */
				current_list_element = current_list_element->next_element;
			}
		}


		if( current_query_step == number_projections_to_make )
		{
			/*
			 * free the memory used for the query result list
			 */
			delete_list( query_result_list );


			(* query_result_list) = final_result_list;
		}
	}



	/* free the allocated memory */
	free( filename );
	free( query_vector_values );



	if( debug_on > 1 )
	{ puts( "\n--- Execute Query End --- " ); }


	return 0;
}


/* ----------------------------------------------------------------------------
   clear_projection_matrices_memory
---------------------------------------------------------------------------- */
int clear_projection_matrices_memory( double *** projection_matrices, int number_existing_matrices, int window_size )
{
	int i = 0;
	int j = 0;


	/* verify input */
	if( projection_matrices == NULL || number_existing_matrices < 1 || window_size < 2 )
	{ puts( "\nError: clear_projection_matrices_memory - invalid input!\n" ); return 1; }


	/* free the memory used for the projection matrices */
	for( i = 0; i < number_existing_matrices; i++ )
	{
		for( j = 0; j < window_size; j++ )
		{
			free( projection_matrices[i][j] );
		}

		free( projection_matrices[i] );
	}

	free( projection_matrices );


	return 0;
}


/* ----------------------------------------------------------------------------
   dataOperations.c
---------------------------------------------------------------------------- */
