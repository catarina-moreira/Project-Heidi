/*
 * dataStructures.c
 *
 *  Created on: 17 de Out de 2012
 *      Author: twi
 */


#include "dataStructures.h"

#include <stdio.h>
#include <stdlib.h>


/* ----------------------------------------------------------------------------
   add_element_list_head
---------------------------------------------------------------------------- */
int add_element_list_head( struct queryElement ** list_head, int vector_number, double distance_value )
{
	struct queryElement * new_element = NULL;


	/* validate input */
	if( vector_number < 0 || distance_value < 0 )
	{ puts( "\nERROR: add_element_list_head - invalid input!\n" ); return 1; }



	/* create the structure to the new element */
	new_element = malloc( sizeof( struct queryElement ) );


	/* fill in the data in the new structure */
	new_element->vector_number = vector_number;
	new_element->distance_value = distance_value;
	new_element->previous_element = NULL;
	new_element->next_element = (* list_head);


	if( (* list_head) != NULL )
	{
		/*
		 * the list is not empty
		 * adjust the pointers so that the current first element points to the new structure
		 */

		(* list_head)->previous_element = new_element;
	}


	/* update the head of the list */
	(* list_head) = new_element;


	return 0;
}


/* ----------------------------------------------------------------------------
   add_element_list_sorted
---------------------------------------------------------------------------- */
int add_element_list_sorted( struct queryElement ** list_head, int vector_number, double distance_value )
{
	struct queryElement * new_element = NULL;
	struct queryElement * current_list_element = NULL;


	/* validate input */
	if( vector_number < 0 || distance_value < 0 )
	{ puts( "\nERROR: add_element_list_sorted - invalid input!\n" ); return 1; }



	/* create the structure to the new element */
	new_element = malloc( sizeof( struct queryElement ) );


	/* fill in the datain the new structure */
	new_element->vector_number = vector_number;
	new_element->distance_value = distance_value;


	if( (* list_head) == NULL )
	{
		/*
		 * the list is empty
		 * add element to the head of the list
		 */

		/* update the list head */
		(* list_head) = new_element;


		/* update pointer to the previous element, in the new element */
		new_element->previous_element = NULL;

		/* update pointer to the next element, in the new element */
		new_element->next_element = NULL;
	}
	else
	{
		/*
		 * the list is not empty
		 * find the correct position to insert the new element
		 */

		current_list_element = (* list_head);


		/* while the distance value of the new element is bigger than the distance value of the current list element, we advance */
		while( distance_value > current_list_element->distance_value )
		{
			/* check if exists a next element on the list */
			if( current_list_element->next_element == NULL )
			{ break; }

			current_list_element = current_list_element->next_element;
		}


		if( distance_value <= current_list_element->distance_value )
		{
			/* exited the while loop because correct position in the list was found */

			/*
			 * update pointers in the new element
			 * previous element will be the previous element of the current list element
			 * next element will be the current list element
			 */
			new_element->previous_element = current_list_element->previous_element;
			new_element->next_element = current_list_element;


			/* update pointers in the previous list element */
			if( current_list_element->previous_element != NULL )
			{
				/* we are not in the beginning of the list, the previous list element must point to the new element */
				current_list_element->previous_element->next_element = new_element;
			}
			else
			{
				/* we are in the begining of the list, the new element will be the new first element of the list */
				(* list_head) = new_element;
			}


			/* the current list element, has to point to the new element, since it will take its place */
			current_list_element->previous_element = new_element;
		}
		else
		{
			/* exited the while loop because the end of the list was reached */


			/*
			 * update pointers in the new element
			 * previous element will be the current list element (the last element of the list)
			 * next element will be the end of the list
			 */
			new_element->previous_element = current_list_element;
			new_element->next_element = NULL;


			/* update pointer in the current list element, so that it points to the new element */
			current_list_element->next_element = new_element;
		}
	}


	return 0;
}


/* ----------------------------------------------------------------------------
   remove_element
---------------------------------------------------------------------------- */
int remove_element( struct queryElement ** list_head, struct queryElement * element_to_delete )
{
	/* validate input */
	if( list_head == NULL || element_to_delete == NULL )
	{ puts( "\nERROR: remove_element - invalid input!\n" ); return 1; }



	if( element_to_delete->previous_element == NULL && element_to_delete->next_element == NULL )
	{
		/*
		 * the list only has one element
		 */

		/* update the list head */
		(* list_head) = NULL;
	}
	else
	{
		/*
		 * the list has more than one element
		 */

		if( element_to_delete->previous_element == NULL )
		{
			/*
			 * the element to be deleted is the first of the list
			 */

			/* update the list head */
			(* list_head) = element_to_delete->next_element;


			/* update pointer to the previous element, in the second list element */
			element_to_delete->next_element->previous_element = NULL;
		}
		else
		{
			/*
			 * the element to be deleted is not the first of the list
			 */

			/* update pointer to next element in previous list element */
			element_to_delete->previous_element->next_element = element_to_delete->next_element;


			if( element_to_delete->next_element != NULL )
			{
				/* update pointer to previous element in the next list element */
				element_to_delete->next_element->previous_element = element_to_delete->previous_element;
			}
		}
	}


	/* free the memory */
	free( element_to_delete );


	return 0;
}


/* ----------------------------------------------------------------------------
   delete_list
---------------------------------------------------------------------------- */
int delete_list( struct queryElement ** list_head )
{
	struct queryElement * current_list_element = NULL;
	struct queryElement * element_to_delete = NULL;


	if( (* list_head) == NULL )
	{
		/*
		 * list is empty, no action needed
		 */
	}
	else
	{
		current_list_element = (* list_head);

		(* list_head) = NULL;


		while( current_list_element != NULL )
		{
			element_to_delete = current_list_element;

			current_list_element = current_list_element->next_element;

			free( element_to_delete );
		}
	}


	return 0;
}


/* ----------------------------------------------------------------------------
   print_list
---------------------------------------------------------------------------- */
int print_list( struct queryElement * list_head )
{
	struct queryElement * current_element = NULL;



	if( list_head == NULL )
	{ puts( "\nprint_list: list is empty\n" ); return 0; }



	current_element = list_head;


	puts( "\n" );

	while( current_element != NULL )
	{
		printf( "%i (%f)   ", current_element->vector_number, current_element->distance_value );

		current_element = current_element->next_element;
	}

	puts( "\n" );


	return 0;
}


/* ----------------------------------------------------------------------------
   dataStructures.c
---------------------------------------------------------------------------- */
