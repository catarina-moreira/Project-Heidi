/*
 * dataStructures.h
 *
 *  Created on: 17 de Out de 2012
 *      Author: twi
 */


#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_


struct queryElement
{
	int vector_number;
	double distance_value;
	struct queryElement * previous_element;
	struct queryElement * next_element;
};


/* ----------------------------------------------------------------------------
   add_element_list_head

   adds a new element to the head of the list
---------------------------------------------------------------------------- */
int add_element_list_head( struct queryElement ** list_head, int vector_number, double distance_value );


/* ----------------------------------------------------------------------------
   add_element_list_sorted

   adds a new element to the list, respecting the increasing distance value
---------------------------------------------------------------------------- */
int add_element_list_sorted( struct queryElement ** list_head, int vector_number, double distance_value );


/* ----------------------------------------------------------------------------
   remove_element

   removes the given element from the list
---------------------------------------------------------------------------- */
int remove_element( struct queryElement ** list_head, struct queryElement * element_to_delete );


/* ----------------------------------------------------------------------------
   delete_list

   deletes the given list
---------------------------------------------------------------------------- */
int delete_list( struct queryElement ** list_head );


/* ----------------------------------------------------------------------------
   print_list

   prints the given list
---------------------------------------------------------------------------- */
int print_list( struct queryElement * list_head );


#endif

/* ----------------------------------------------------------------------------
   dataStructures.h
---------------------------------------------------------------------------- */
