#ifndef VASH_INTERNAL_COMMANDS
#define VASH_INTERNAL_COMMANDS

// Checks if a command is internal
// If is internal it gets executed (ret 0), else return -1
int isInternal(const char *command);


// AVAILABLE COMMANDS
// Change directory
void cd();



#endif // VASH_INTERNAL_COMMANDS
