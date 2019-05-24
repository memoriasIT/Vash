#include "config.h"
#include "internalCommands.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

// Internal functions
void cd(int nargs, const char* args[]);
void jobs(int nargs, const char* args[]);

// Map function in struct
typedef struct {
    const char *name;
    void (*func) (int, const char**);
} vash_functionmap;

static vash_functionmap internal_funcs[] = {
    { "cd", cd },
    { "jobs", jobs },
    { NULL, NULL }
};


// Counts the number of argumments passed as a parameter
int numofargs(const char *command[]){
   int i = 0;
   while (command[i] != NULL){
        i++;
   }

   return i;
}


// Check if command is internal, if internal execute, else return -1 to main.c
int isInternal(const char *command[]) {
    // Use pointer to go through function map
    vash_functionmap * ptr;
    for (ptr = internal_funcs; ptr->name != NULL; ptr++) {
        if (!strcmp(ptr->name, command[0])){ 
          // Command found execute and return 0
          ptr->func(numofargs(command), command);
          
          return 0;
        }
        
    }
    // Command is not internal, return error
    return -1;
}


void cd(int nargs, const char* args[]){
    if (nargs == 1){
        // Only cd command specified, goto home folder
            chdir(getenv("HOME"));
    } else {
        // More data specified
        // chdir returns -1 if error ocurred and erno is set
        if (chdir(args[1]) == -1)
                fprintf(stderr, ERRORSTR" - Error in cd: %s\n", strerror(errno));
    }

}


void jobs(int nargs, const char* args[]) {
    print_job_list(job_list);

}
