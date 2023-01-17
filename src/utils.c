#include "utils.h"

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

char* create_file_name(char* first_part, int reducer_power, char* extension) {
    int power_num_of_chars = (int)(ceil(log10(reducer_power)) * sizeof(char));
    char str[power_num_of_chars + 1];
    sprintf(str, "%d", reducer_power);

    int first_part_len = strlen(first_part);
    int extension_len = strlen(extension);

    int file_name_length = power_num_of_chars + first_part_len + extension_len;

    char *file_name = calloc(file_name_length + 1, sizeof(char));

    // concatenating all parts (maybe it would of been wiser to use strcat :) forgot about it)
    strncpy(file_name, first_part, first_part_len);
    strncpy(file_name + first_part_len, str, power_num_of_chars);
    strncpy(file_name + first_part_len + power_num_of_chars, extension, extension_len);
    file_name[file_name_length] = '\0';
    return file_name;
}

double compute_power(double num, int power) {
    double result = 1.0;

    for (int i = 1; i <= power; i++) {
        result = num * result;
    }
    return result;
}

double compute_nth_root_binary_search(int n_root, int num) {
    double start = 1, end = num;
    double admited_error = 1e-8; 
    double middle = 0;

    // binary search until admited error is reached
    while ((end - start) > admited_error) {
        middle = (end + start) / 2.0;
        if (compute_power(middle, n_root) < num) {
            start = middle; 
        } else {
            end = middle;
        } 
    }
    return start;
}

int is_perfect_power(int power, int num) {
    if (compute_power(ceil(compute_nth_root_binary_search(power, num)), power) == num) {
        return 1;
    }
    return 0;
}

int count_distinct_elements(List* reducer_list) {
    int size = reducer_list->current_size;
    if (size == 0) {
        return 0;
    }

    if (reducer_list->current_size == 1 ) {
        return 1;
    }
    int count = 1;

    // check between two adjacent numbers, if they == then duplicate, else one more distict number
    for (int i = 1; i < size; i++) {
        if (reducer_list->the_list[i-1] != reducer_list->the_list[i]) {
            count++;
        }
    }
    return count;
}

/************ THREAD BEHAVIOUR FUNCTIONS ************/

void mapper_work(char* file_name, List* list, int thread_id, int nr_of_reducers) {
    // loose the '\n' or '\r' char at the end of the file name if it exists
    if (file_name[strlen(file_name) - 1] == '\n' || file_name[strlen(file_name) - 1] == '\r') {
        file_name[strlen(file_name) - 1] = '\0';
    }

    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("BANG\n");
        exit(-1);
    }
    int nums = 0;
    int num_container = 0;

    // minimal power
    int power = 2;

    fscanf(file, "%d", &nums);
    for (int i = 0; i < nums; i++) {
        power = 2;
        fscanf(file, "%d", &num_container);
        for (int j = 0; j < nr_of_reducers; j++) {
            // if number is perfect power for current power, add it to it's specific listS
            if (is_perfect_power(power, num_container)) {
                add_element_for_list_of_lists(list, power, num_container);
            }
            power++;
        }
    }
    
    fclose(file);
}

void reducer_work(List** mappers_list, List* reducer_list, int reducer_id, int total_mappers) {
    // the power that reducer counts elements for
    int power = reducer_id - total_mappers + 2;

    // go through all mappers list and take all elements from the correct list from each mapper
    for (int i = 0; i < total_mappers; i++) {
        List* mapper_list = mappers_list[i];
        // position of list in the list of lists
        int list_index = power - 2;
        int list_size = mapper_list[list_index].current_size;
        
        // add element to reducer list
        for (int j = 0; j < list_size; j++) {
            add_element_for_list_of_lists(reducer_list, 2, mapper_list[list_index].the_list[j]);
        }
    }

    // i use qsort for my way of counting only distinct elements (explained in README)
    qsort(reducer_list->the_list, reducer_list->current_size, sizeof(int), cmpfunc);

    char* file_name = create_file_name("out", power, ".txt");
    FILE* file = fopen(file_name, "w");
    if (file == NULL) {
        printf("BANG\n");
        exit(-1);
    }

    fprintf(file, "%d", count_distinct_elements(reducer_list));

    fclose(file);
    free(file_name);
}

/************ THREAD BEHAVIOUR FUNCTIONS ************/