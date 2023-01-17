#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


#ifndef _LIST_
#define _LIST_

typedef struct _list
{
    int current_size;
    int* the_list;
} List;

/**
 * @brief Create a mappers lists object
 */
List** create_mappers_lists(int nr_of_mappers, int nr_of_reducers);

/**
 * @brief Create a list array object
 */
List* create_list_array(int nr_of_lists);

/**
 * @brief adds an element into a list of lists
 * 
 * @param list the list of lists
 * @param pos the position of the list the element needs to be inserted into
 * @param elem the element to be inserted
 */
void add_element_for_list_of_lists(List* list, int pos, int elem);

/**
 * @brief frees memory allocated for a list
 */
void free_list(List** list, int size);

/**
 * @brief frees memory allocated for mappers list
 */
void free_mappers_list(List*** list, int nr_mapper, int nr_reducer);

#endif