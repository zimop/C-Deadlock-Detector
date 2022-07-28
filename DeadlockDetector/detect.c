#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

#define TERMINATE 2


typedef struct {
int pid;
//an integer which keeps track of which round the process was traversed in.
int traversed;
int lock;
int request;
} Example;



int get_execution_time(int *files_requested, int size);
void insertionSort(int array[], int n);
int* detect_deadlock(Example *examples, int size, int* numToTerminate);



int main(int argc, char* argv[]) {
// Open file
    char* file;
    char letter;
    for(int i = 1; i < argc; i++){
        if(argv[i][1] != 'f'){
            if(argv[i][1] == 'c'){
                letter = 'c';
            }else{
                letter = 'e';
            }
        }
        else{
            i++;
            file = argv[i];
        }
    }
    FILE* fp = fopen(file, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
// Read into memory

    size_t examples_len = 0, examples_size = 2;
    Example* examples = malloc(sizeof(Example) * examples_size);
    if (examples == NULL) {
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }
    int pid, lock , request;
    

    //Array to keep the aggregate of files that are needed from processes
    int* files_needed = malloc(sizeof(int)* examples_size * 2);
    //Array to keep the aggregate of files that are needed from processes
    int* files_requested = malloc(sizeof(int)* examples_size );
    
    //variable to keep track of all the unique files needed in total
    int total = 0;

    //number of processes which are currently under execution.
    int size = 0;

    while (fscanf(fp, "%d %d %d", &pid, &lock, &request) == 3) {
        if (examples_len == examples_size) {
            examples_size *= 2;
            examples = realloc(examples, sizeof(Example) * examples_size);
            files_needed = realloc(files_needed, sizeof(int)* examples_size*2);
            if(letter == 'e'){
                files_requested = realloc(files_requested, sizeof(int)* examples_size);
                if (files_requested == NULL) {
                    fprintf(stderr, "Realloc failure\n");
                    exit(EXIT_FAILURE);
                }
            }
            if (examples == NULL || files_needed == NULL) {
                fprintf(stderr, "Realloc failure\n");
                exit(EXIT_FAILURE);
            }
        }
        if(letter == 'e'){
            files_requested[size] = request;
        }
        size++;

/*looking for distinct files from all files requested or locked*/

// both flags are used to determine whether that file has already been accounted for.
        int lock_flag = 0;
        int request_flag = 0;
        for(int i = 0; i < total; i++){
            if(lock_flag == 1 || files_needed[i] == lock){
                lock_flag = 1;
            }
            if(request_flag == 1 || files_needed[i] == request){
                request_flag = 1;
            }
        }
        if(lock_flag == 0){
            files_needed[total] = lock;
            total++;
        }
        if(request_flag == 0){
            files_needed[total] = request;
            total++;
        }
        examples[examples_len].pid = pid;
        examples[examples_len].lock = lock;
        examples[examples_len].traversed = 0;
        examples[examples_len++].request = request;
    }

    int* numToTerminate = malloc(sizeof(int));
    int* terminates  = detect_deadlock(examples, size, numToTerminate);
    insertionSort(terminates, *numToTerminate);
    printf("Processes %d \n", size);
    printf("Files %d \n", total);
    if(letter == 'e'){
        int exec_time = get_execution_time(files_requested, size);
        printf("Execution time %d \n", exec_time);
    }else{
        int count = *numToTerminate;
        if(count > 0){
           printf("Deadlock detected\n");
           printf("Terminate ");
           for(int i = 0; i < count; i++){
               printf("%d ",terminates[i]);
           }
        }else{
            printf("No deadlocks");
        }
    }
    printf("\n");

    free(examples);
    fclose(fp);
    return 0;
}

/*This function works from the interpretation that during the first time unit, all locks are being unlocked in parallel, and 
the rest of the time units, the requested files are being worked through, so essentially all you need to do is count what the
highest frequency is among all file appearances.*/

int get_execution_time(int* files_requested, int size){
    int lock_execution = 1;
    int max_count = 1;
    //sorts the files in order, so all the same values are right next to each other
    insertionSort(files_requested, size);
    int count = 1;
    for(int j = 0; j < size-1; j++){
        if(files_requested[j] == files_requested[j+1]){
            count++;
            
        }else{
            if(count > max_count){
                max_count = count;
            }
            count = 1;
        }
    }
    if(count > max_count){
        max_count = count;
    }
    return lock_execution + max_count;
}


void insertionSort(int array[], int n) 
{ 
    int i, element, j; 
    for (i = 1; i < n; i++) { 
        element = array[i]; j = i - 1;
        while (j >= 0 && array[j] > element) { 
            array[j + 1] = array[j]; 
            j = j - 1; 
        } 
        array[j + 1] = element; 
    } 
}


/*this function takes in the list of process, how many processes there are, and finally a pointer to the number of 
processes to be terminated (which is 0, at first, but changes once this function runs)
If there is a deadlock, then it will return an array of the processes that need to be terminated*/

int* detect_deadlock(Example *examples, int size, int *numToTerminate){
    int num_terminate = TERMINATE;

    //all the nodes that need to be terminated if a deadlock does occur.
    int *nodes_to_terminate = malloc(sizeof(int)*10);
    int num_deadlocks = 0;

    /*Round is a variable that keeps count of which round of iteration we are in during the outermost for loop execution,
    used to detect how many process diagrams there are with their dependencies. */
    int round = 1;

    /*if there is a deadlock then this will hold the starting point of the cycle in my list*/
    int startingPoint;
    
    
    for(int i = 0; i<size ; i++){
        
        /* this list is a list which will hold all the processes linked next to the 
        process which has locked a file that the first process requested */
        struct list* process_list = newlist(examples[i].pid);

        if(num_deadlocks == num_terminate){
            num_terminate *= TERMINATE;
            nodes_to_terminate = realloc(nodes_to_terminate, sizeof(int)*num_terminate);
        }

        //adds a process to the linked list at the beginning
        process_list = addToBeginning(process_list, examples[i].pid);
        
        Example current = examples[i];

        /*if there is a match(a process is accessing a file that another process has locked), then no_match = 0, and the 
        loop will keep running to see if there are more matches with the next process*/
        int no_match = 0;
        int deadlock = 0;

        
        if(round == 1 || examples[i].traversed == 0){
            
            //indicates that this file has been traversed in this round
            examples[i].traversed = round;


            while(no_match != 1){
                for(int j = 0; j<size; j++){

                    /*if there is a matching file that the current process is requesting, and it is locked
                    then there is a potential process list to be formed*/
                    if(current.request == examples[j].lock){

                        /*if the process has already been traversed, or if the process had been traversed in a previous round, 
                        whether it is connected to a process which ended up in a deadlock or did not end up in a deadlock
                        , it will not need to be terminated because terminating this node will not stop the deadlock, 
                        so there is no match*/
                        if(examples[j].traversed != 0 && examples[j].traversed < round){
                            examples[j].traversed = round;
                            no_match = 1;
                            break;
                        }

                        /*if the process has already been traversed in the current round, then a deadlock definitely exists*/
                        if(examples[j].traversed == round){
                            deadlock = 1;
                            startingPoint = examples[j].pid;
                            num_deadlocks++;
                            no_match = 1;
                            break;
                        }

                        /*else add the process to the process list, and note that it has been traversed in this current round*/
                        process_list = addToBeginning(process_list, examples[j].pid);
                        examples[j].traversed = round;
                        current = examples[j];
                        no_match = 0;
                        break;
                    }

                    /*if it goes through every process in the list and there is no process which locks the file, 
                    then there is no match for a process list*/
                    else{
                        no_match = 1;
                    }
                }
            }
        }
        if(deadlock == 1){
            nodes_to_terminate[num_deadlocks-1] = findTerminateNode(process_list, startingPoint);
        }
        round++;
        freeList(process_list);
    }
    *numToTerminate = num_deadlocks;
    return nodes_to_terminate;
}