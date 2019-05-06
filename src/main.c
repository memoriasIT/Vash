/*
                        ##                            
                      #(((((                          
   ................ (((((((((( ,,,,,,,,,,,,,,,,       
  /&&&&&&&&&&&&&&&&&////((////(&&&&&&&&&&&&&&&&(      
    ##&&&&&&&&&&//////////////////&&&&&&&&&&&&        
      &&&&&&&&&&/////////////////&&&&&&&&&&/          
      &&&&&&&&&&///////////////&&&&&&&&&&                                
      &&&&&&&&&&////////////#&&&&&&&&&&/.                        &&&&&&&& 
      &&&&&&&&&&//////////#&&&&&&&&&&/////                        (&&&&&( 
      &&&&&&&&&&////////%&&&&&&&&&&/////////                      (&&&&&  
    //&&&&&&&&&&//////&&&&&&&&&&&/////////////                    &&&&&(  
  ////&&&&&&&&&&////&&&&&&&&&&%////(#%%#////##//    ./#%%%(/.    /&&&&&    (%%( 
  ////%%%%%%%%%&//&&&&&&&&&**&&&&&%//&&&&&&&// #&&&&% .%&&&&     &&&&&( &&&&&&&&& 
    ,,%%%%%%%%%%%%%%%%%%%%/,,&&&&*****&&&&&&  &&&&&      &&&   .&&&&&&&    &&&&&(
      %%%%%%%%%%%%%%%%%%*,,,&&&&&(******&&&&&(  &&&&           &&&&&&/     &&&&& 
      %%%%%%%%%%%%%%%%,,,*,&&&&*******&&&&&   %&&&&&&&,        &&&&&(     #&&&&& 
      %%%%%%%%%%%%%%,,,,***%%%%%*******%%%%%(     #%%%%%%%%   %%%%%%      %%%%%
      ###########%,,,,,**,#%%%%#*****,.%%%%%         %%%%%%%  %%%%%#     /%%%%%
      ##########.,,,,,,**,#####(***, ,######  ..       ##### /#####      #####(
      ########  .,,,,,,,,,(#####,,  ##/####  ###/      ##### ######     ,##### 
      #####(      ,,,,,,,,,#########  ######.######/(#####( *#####.     ######(
      ***           ,,,,,,,,,,#(                 /(##/                         
                      .,,,,,                                                   
                         .    

    A intelligent Shell with Vim-like capabilities.
    ~ @memoriasIT - 2019

*/

#include "job_control.h"        // Get command and job control
#include "config.h"             // Configuration of messages and styles
#include "internalCommands.h"   // Internal shell commands
#include "signalHandler.h"      // TODO: make signal handler

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <string.h>


// Maximum characters per line
#define MAX_LINE 256

// [--------------------------------------------------------------------------]
// [                                 MAIN                                     ]
// [--------------------------------------------------------------------------]

int main(void){
    char inputBuffer[MAX_LINE]; // Command buffer
    int background;             // Equals to 1 if followed by '&'
    char *args[MAX_LINE/2];     // Max allowed arguments
   
    int pid_fork, pid_wait;     // pid for created and waited processes
    int status;                 // Status returned by wait
    enum status status_res;     // Status processed by analyze_status()
    int info;                   // Info processed by analyze_status()

    
    // Program ends in get_command() if ^D signal is received
    while (1){
        // Ignore signals related to terminal
        ignore_terminal_signals();

        // Print shell line, edit in config.h
        PRINTSHELLID; 
        fflush(stdout);
        
        // Gets and processes next command
        get_command(inputBuffer, MAX_LINE, args, &background);

        if (args[0] == NULL) continue; // Command is empty
      
        // Check if command is internal
        // If internal, function called in internalCommands.c and return 0
        // Else return -1 and a fork is needed
        if (isInternal((const char*) args) == -1){
            // Child created
            pid_fork = fork();

            if (pid_fork == -1) { // Check if fork was created correctly
                fprintf(stderr, ERRORSTR" - Error while forking: %s\n", strerror(errno));
            }

            if (pid_fork == 0){
                // [----------------- CHILDREN CODE ONLY! ------------------------]
                // 
                // Child processes are assigned a gid that differs from parent id
                new_process_group(getpid());
 
                // restore signals and add to process group
                restore_terminal_signals();               

                // Execute with given arguments
                execvp(args[0], args);
                
                // If this gets executed execvp failed
                fprintf(stderr, ERRORSTR"- Error in exec: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            
            } else {
                // [------------------ PARENT CODE ONLY! -------------------------]
                // 
                if (background == 0){ // We execute in foreground
                    set_terminal(pid_fork); // Set the control of terminal to child
                  
                    // We wait child, check man 2 wait for options details
                    pid_wait = waitpid(pid_fork, &status, WUNTRACED);

                    // Retrieve control of terminal
                    set_terminal(getpid()); 

                    // Get status of process
                    status_res = analyze_status(status, &info);

                    // Print info about the process
                    fprintf(stderr, "Foreground pid: %d, command: %s, %s, info: %d\n",
                            pid_fork, args[0], status_strings[status_res], info);

                    
                    // Print information about the process
                    if (status_res != EXITED) {
                        // Process was exited
                         fprintf(stderr, "pid: %d, command: %s was exited.\n",
                            pid_fork, args[0]);
   
                    } else if (status_res == SIGNALED) {
                        // Process was signaled
                        fprintf(stderr, "pid: %d, command: %s was signaled.\n",
                            pid_fork, args[0]);

                    } else {
                        // Process was suspended
                        fprintf(stderr, "pid: %d, command: %s was suspended.\n",
                            pid_fork, args[0]);
   
                    }


                } else { // We execute in background
                    fprintf(stderr, "Background job running... pid: %d, command: %s\n", 
                            pid_fork, args[0]);
                    
                } // end of bg job
        
        
            } // end of parent code 
        
        } // end of internal command

    } // end while


} // end main
