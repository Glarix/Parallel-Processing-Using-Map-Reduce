#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "list.h"

#ifndef _UTILS_
#define _UTILS_

#define MAPPER_THREAD 0
#define REDUCER_THREAD 1

typedef struct _arguments
{
    int thread_num;
    int thread_type;
    int reducers_num;
    int mappers_num;
    FILE* test_file;
    pthread_mutex_t* lock;
    pthread_barrier_t* barrier;

    List* reducer_list;

    List** mappers_list; // List with all lists of mappers (Used by reducers to get elements)
} Arg;

/**
 * @brief Function that creates the file name string for the out files
 */
char* create_file_name(char* first_part, int reducer_power, char* extension); 

/**
 * @brief Function that computes num^power
 */
double compute_power(double num, int power);

/**
 * @brief Function that computes nth root of a number
 */
double compute_nth_root_binary_search(int n_root, int num);

/**
 * @brief Function that checks if number num is a perfect power
 */
int is_perfect_power(int power, int num);

/**
 * @brief  Function tht counts distinct elements in a list
 */
int count_distinct_elements(List* reducer_list);

/**
 * @brief Function that determines mapper behaviour
 */
void mapper_work(char* file_name, List* list, int thread_id, int nr_of_reducers);

/**
 * @brief  Function that determines reducer behaviour
 */
void reducer_work(List** mappers_list, List* reducer_list, int reducer_id, int total_mappers);


#endif