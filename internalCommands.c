#include <stdio.h>
#include <string.h>

// Internal functions
void cd();


// Map function in struct
const static struct {
    const char *name;
    void (*func) (void);
} function_map[] = {
    { "cd", cd },
};


// Check if command is internal, if internal execute, else return -1 to main.c
// https://stackoverflow.com/questions/1118705/call-a-function-named-in-a-string-variable-in-c
int isInternal(const char *command) {
    unsigned long i;
    for (i = 0; i < (sizeof(function_map) / sizeof(function_map[0])); i++){
        if (!strcmp(function_map[i].name, command) && function_map[i].func){
            function_map[i].func();
            return 0;
        }
    }

    // Command is not internal, return error
    return -1;
}


void cd(){
   printf("testcd"); 
}
