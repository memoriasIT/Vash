#include "config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

// Internal functions
void cd(int nargs, char* args[]);

// Map function in struct
const static struct {
    const char *name;
    void (*func) (int, const char**);
} function_map[] = {
    { "cd", cd },
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
// https://stackoverflow.com/questions/1118705/call-a-function-named-in-a-string-variable-in-c
int isInternal(const char *command[]) {
    unsigned long i;
    for (i = 0; i < (sizeof(function_map) / sizeof(function_map[0])); i++){
        if (!strcmp(function_map[i].name, command[0]) && function_map[i].func){
            function_map[i].func(numofargs(command), command); //
            return 0;
        }
    }

    // Command is not internal, return error
    return -1;
}


void cd(int nargs, char* args[]){
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
