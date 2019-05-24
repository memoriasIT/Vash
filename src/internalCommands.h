#ifndef VASH_INTERNAL_COMMANDS
#define VASH_INTERNAL_COMMANDS

#include "job_control.h"
#include "vash_globals.h"


#include <signal.h>

// Checks if a command is internal
// If is internal it gets executed (ret 0), else return -1
int isInternal(const char *command[]);

// AVAILABLE COMMANDS
// Change directory
void cd();
// Show list of jobs
void jobs();
// Get a job to and from background
void bg(int nargs, const char* args[]);
void fg(int nargs, const char* args[]);

// Map function in struct
typedef struct {
    const char *name;
    void (*func) (int, const char**);
} vash_functionmap;

static vash_functionmap internal_funcs[] = {
    { "cd", cd },
    { "jobs", jobs },
    { "fg", fg },
    { "bg", bg },
    { NULL, NULL }
};




#endif // VASH_INTERNAL_COMMANDS
