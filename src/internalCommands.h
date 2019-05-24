#ifndef VASH_INTERNAL_COMMANDS
#define VASH_INTERNAL_COMMANDS

#include "job_control.h"
#include "vash_globals.h"

// Checks if a command is internal
// If is internal it gets executed (ret 0), else return -1
int isInternal(const char *command[]);

// AVAILABLE COMMANDS
// Change directory
void cd();
// Show list of jobs
void jobs();



#endif // VASH_INTERNAL_COMMANDS
