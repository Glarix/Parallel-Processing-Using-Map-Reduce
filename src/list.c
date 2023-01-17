#include "list.h"

/**
 * @brief Initiates the list that holds all the lists for mappers (also initializing those lists)
 */
List** create_mappers_lists(int nr_of_mappers, int nr_of_reducers) {
    List** list_for_mappers = (List**)malloc(sizeof(List*) * nr_of_mappers);

    for (int i = 0; i < nr_of_mappers; i++) {
        list_for_mappers[i] = create_list_array(nr_of_reducers);
    }
    
    return list_for_mappers;
}

/**
 * @brief Initiates a list
 */
List* create_list_array(int nr_of_lists) {
    List* list = (List*)calloc(nr_of_lists, sizeof(List));

    for (int i = 0; i < nr_of_lists; i++) {
        list[i].current_size = 0;
    }

    return list;
}

void add_element_for_list_of_lists(List* list, int pos, int elem) {
    // list is in fact a list of list, with pos, I take the list that I need to add the element to
    int size = list[pos - 2].current_size;

    // if list is empty, I calloc it, otherwise I realloc with +1 space
    if (size == 0) {
        list[pos - 2].the_list = (int*)calloc(1, sizeof(int));
    } else {
        list[pos - 2].the_list = (int*)realloc(list[pos - 2].the_list, (size + 1) * sizeof(int));
    }
    list[pos - 2].the_list[size] = elem;
    list[pos - 2].current_size = size + 1;
}


void free_list(List** list, int size) {
    List* aux = *(list);
    for (int i = 0; i < size; i++) {
        if (aux[i].current_size != 0) {
            free(aux[i].the_list);
        }
    }
    free(*(list));
    *list = NULL;
}


void free_mappers_list(List*** list, int nr_mapper, int nr_reducer) {
    List** aux = *(list);
    for (int i = 0; i < nr_mapper; i++) {
        free_list(&(aux[i]), nr_reducer);
    }

    free(*list);
    *list = NULL;
}
