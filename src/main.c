#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "utils.h"
#include "list.h"

/**
 * @brief Function that is executed by every thread and decides if to run mapper or reducer behaviuor
 */
void *f(void *arguments) {
    int go = 1;
    char* line = NULL;
    size_t len = 0;
  	Arg* args = (Arg*)arguments;

    if (args->thread_type == MAPPER_THREAD){
        while (go) {
            // lock is necessary so that two mappers can't take same file to read from
            pthread_mutex_lock(args->lock);
            if (getline(&line, &len, args->test_file) == -1) {
                go = 0;
            }
            pthread_mutex_unlock(args->lock);

            // if mapper got a file to read from, execute mapper behaviour
            if (go == 1) {
                mapper_work(line, args->mappers_list[args->thread_num], args->thread_num, args->reducers_num);
            }
        }
        // after all files were read, hit the barrier for reducers to start working
        pthread_barrier_wait(args->barrier);
    } else {
        // barier is necessary so mappers can finish their work befor reducers starting theirs
        pthread_barrier_wait(args->barrier);
        reducer_work(args->mappers_list, args->reducer_list, args->thread_num, args->mappers_num);
    }
    // free the line used by getline()
    if (line) {
        free(line);
    }
  	pthread_exit(NULL);
}

void get_program_params(int* nr_mappers, int* nr_reducer, int* total_threads, char** test_file_name, char** argv) {
    *nr_mappers = atoi(argv[1]);
    *nr_reducer = atoi(argv[2]);
    *total_threads = *nr_mappers + *nr_reducer;
    *test_file_name = argv[3];
}

int main(int argc, char *argv[])
{

    pthread_barrier_t reducer_barrier;
    pthread_mutex_t lock;
    int error = pthread_mutex_init(&lock, NULL);
	if (error) {
    	printf("Mutex init has failed\n");
    	exit(-1);
	}

    // getting arguments
    int nr_mappers, nr_reducers, total_threads;
    char* test_file_name;
    get_program_params(&nr_mappers, &nr_reducers, &total_threads, &test_file_name, argv);

    error = pthread_barrier_init(&reducer_barrier, NULL, total_threads);
    if (error) {
    	printf("Barrier init has failed\n");
        pthread_mutex_destroy(&lock);
    	exit(-1);
	}

    FILE* test_file = fopen(test_file_name, "r");
    if (test_file == NULL) {
        printf("Opening test file has failed\n");
        pthread_mutex_destroy(&lock);
        pthread_barrier_destroy(&reducer_barrier);
    }

    char* line = NULL;
    size_t len = 0;
    // I do not need the number of files (logic explained in README)
    getline(&line, &len, test_file);
    if (line)
        free(line);
    

    pthread_t threads[total_threads];
    Arg arguments[total_threads];

    // List containing every mapper list
    List** list_for_mappers = create_mappers_lists(nr_mappers, nr_reducers);

    int r, i; 

    // Creating threads
    for ( i = 0; i < total_threads; i++)
    {
        arguments[i].thread_num = i;
        if(i < nr_mappers) {
            arguments[i].thread_type = MAPPER_THREAD;
            arguments[i].reducer_list = NULL;
        } else {
            arguments[i].thread_type = REDUCER_THREAD;
            arguments[i].reducer_list = create_list_array(1);
        }
        
        arguments[i].mappers_num = nr_mappers;
        arguments[i].reducers_num = nr_reducers;
        arguments[i].test_file = test_file;
        arguments[i].lock = &lock;
        arguments[i].barrier = &reducer_barrier;
        arguments[i].mappers_list = list_for_mappers;

        r = pthread_create(&threads[i], NULL, f, &arguments[i]);

        if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
    }

    // joining threads
    for (i = 0; i < total_threads; i++) 
    {
		r = pthread_join(threads[i], NULL);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

    // free all memory
    free_mappers_list(&list_for_mappers, nr_mappers, nr_reducers);
    for (int j = nr_mappers; j < total_threads; j++) {
        free_list(&(arguments[j].reducer_list), 1);
    }
    
    fclose(test_file);
    pthread_mutex_destroy(&lock);
    pthread_barrier_destroy(&reducer_barrier);
    return 0;
}
