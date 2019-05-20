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
#include "signalHandler.h"      // Signal handler for shell (not GUI)
#include "vash_globals.h"       // Manage global variables


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

// List of jobs (found in job_control.h)
job* job_list;

    
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

   
    // Create the job list
    job_list = new_list("ShellJobs");

    // Specify Signal Handler
    signal(SIGCHLD, vash_signalHandler);


    // Program ends in get_command() if ^D signal is received
    while (1){
        // In case still blocked
        // Does not stack so there is no problem to use this if not blocked
        unblock_SIGCHLD();

        // Ignore signals related to terminal
        ignore_terminal_signals();

        // Print shell line, edit in config.h
        PRINTSHELLID; 
        fflush(stdout);
        
        // Gets and processes next command
        // SIGCHLD is blocked due to critical section
        block_SIGCHLD();
        get_command(inputBuffer, MAX_LINE, args, &background);


        if (args[0] == NULL){// Command is empty
            unblock_SIGCHLD();             
            continue;
        }

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
                // Unblock in children, blocked since get_command input
                unblock_SIGCHLD();
                
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
                    //unblock_SIGCHLD(); // End of critical section             
                    set_terminal(pid_fork); // Set the control of terminal to child
                  
                    // We wait child, check man 2 wait for options details
                    pid_wait = waitpid(pid_fork, &status, WUNTRACED);

                    // Retrieve control of terminal
                    set_terminal(getpid()); 

                    // Get status of process
                    status_res = analyze_status(status, &info);

                    // Print info about the process
                    fprintf(stderr, "\nForeground pid: %d, command: %s, %s, info: %d\n",
                            pid_fork, args[0], status_strings[status_res], info);

                    
                    // Print information about the process
                    if (status_res != EXITED) {
                        // Process was exited
                         fprintf(stderr, "\tpid: %d, command: %s was exited.\n",
                            pid_fork, args[0]);
   
                    } else if (status_res == SIGNALED) {
                        // Process was signaled
                        fprintf(stderr, "\tpid: %d, command: %s was signaled.\n",
                            pid_fork, args[0]);

                    } else {
                        // Process was suspended
                        //add_job(job_list, new_job(pid_fork, args[0], STOPPED));
                        
                        fprintf(stderr, "\tpid: %d, command: %s was suspended.\n",
                            pid_fork, args[0]);
   
                    }

                    // End of critical section
                    // Blocked since getting command input 
                    unblock_SIGCHLD();


                } else { // We execute in background
                    // Job control to avoid defunct processes
                    // void add_job(job *list, job*item)
                    // job* new_job(pid_t pid, const char *command, enum job_state state)
                    add_job(job_list, new_job(pid_fork, args[0], BACKGROUND));
                    
                    fprintf(stderr, "Background job running... pid: %d, command: %s\n", 
                            pid_fork, args[0]);
                    
                } // end of bg job
        
        
            } // end of parent code 
        
        } // end of internal command

    } // end while


} // end main
