// ██      ██                   ██     
//░██     ░██                  ░██     
//░██     ░██  ██████    ██████░██     
//░░██    ██  ░░░░░░██  ██░░░░ ░██████ 
// ░░██  ██    ███████ ░░█████ ░██░░░██
//  ░░████    ██░░░░██  ░░░░░██░██  ░██
//   ░░██    ░░████████ ██████ ░██  ░██
//    ░░      ░░░░░░░░ ░░░░░░  ░░   ░░ 
// ~ Memorias de un informatico 2018 ~



#include <curses.h>
#include <panel.h>
#include <string>
#include <string.h>
#include <panel.h>

// Splash Screen Print
void splash(int &row, int &col);
void terminal(int &row, int &col);
void PrintVimStatus(WINDOW *vim, int selected);
void TerminalPrint (WINDOW *terminal, int input, int x, int y);
void VimCommand(WINDOW *vim, int y);


    char *long_modes[] = { 
        "- Normal",
        "- Visual",
        "- Insert",
    };
    char *modes[] = { 
        "N",
        "V",
        "I",
    };

    int n_modes = sizeof(modes) / sizeof(char *);


    int main(){

        int row, col;   
        initscr();
        getmaxyx(stdscr, row, col);

        splash(row, col);
        terminal(row, col);

        endwin();                                                                                  

        return 0;                                                                                  
    }                                                                                           

    // SPLASH SCREEN
    void splash(int &row, int &col){
        char msg[]="V-ash 0.0.0-alpha";
        char msg2[]="by MemoriasIT 2018";
        char msg3[]="Vash is a bash terminal with VIm-like capabilities";
        char msg4[]="The project is open source and distributable";
        char msg5[]="Press any key to start";

        mvprintw(row/2-3, (col-strlen(msg))/2, "%s", msg);
        mvprintw(row/2, (col-strlen(msg2))/2, "%s", msg2);
        mvprintw(row/2+2, (col-strlen(msg3))/2, "%s", msg3);
        mvprintw(row/2+3, (col-strlen(msg4))/2, "%s", msg4);
        mvprintw(row/2+4, (col-strlen(msg5))/2, "%s", msg5);
        attron(A_BLINK);
        mvprintw(row-2,2,"Press any key to start...",row,col);                      
        refresh();                                                                                 
        getch();
    }

    // TERMINAL WINDOW
    void terminal(int &row, int &col){
        PANEL  *panels[2];

        // VIm pannel selection
        int selected = 0;

        // For both vim pannel and window
        int input;
        int input2;
        
        // Coordinates for printing
        int tx = 0;
        int ty = 0;

        clear();
        // WINDOW 0 --> TERMINAL
        // WINDOW 1 --> VIM
        WINDOW *terminal = newwin(row-2, col, 0, 0);
        WINDOW *vim = newwin(1, col, row-1, 0);

        panels[0] = new_panel(terminal); 	/* Push 0, order: stdscr-0 */
        panels[1] = new_panel(vim);      	/* Push 1, order: stdscr-0-1 */

        // Show pannel 1 in front
        update_panels();
        doupdate();

        // PROPERTIES FOR TERMINAL WINDOW
        keypad(terminal, FALSE);          // disable function keys

        // PROPERTIES FOR VIM WINDOW
        keypad(vim, TRUE);                // enable function keys
        noecho();                         // disable writing input
        //curs_set(0);                    // disable cursor so it doesn't blink

        // PROGRAM LOGIC
        PrintVimStatus(vim,selected);     // Start vim bar
        while(1){
            input = getch();
           
            // Detect vim mode change
            if (input == 27){ // ESC key 27
                input2 = wgetch(vim);
                switch(input2){
                    case 105:     // i
                        selected = 2; 
                        break;
                    case 118:     // v
                        selected = 1;
                        break;
                    case 58:      // :
                        VimCommand(vim, row-1);
                    default:
                        selected = 0;
                }
                wrefresh(vim);
                PrintVimStatus(vim, selected);
            }

            // Terminal Print only in insert mode
            if (selected == 2){
                tx++;
                if (tx == col){
                    tx = 0;
                    ty++;
                }
                TerminalPrint (terminal,input,tx,ty);     
            }
        }
    }

void VimCommand(WINDOW *vim, int y){
    int command = 0;
    char *commandstr;
    //while(command!= 10){
        command = getch();
        //if (command > 31 && command < 123){
            commandstr = commandstr + (char) command; 
            mvwprintw(vim, y, 10, "%s", commandstr);
        //}
    //}
//    ParseCommand();

}


void PrintVimStatus(WINDOW *vim, int selected){
    int x, y, i;
    x = 1;
    y = 0;
    for(i = 0; i < n_modes; ++i){       
        if(selected == i){       
            wattron(vim, A_REVERSE); 
            mvwprintw(vim, y, x, "%s", modes[i]);
            mvwprintw(vim, y, 7, "%s", long_modes[i]);
            wattroff(vim, A_REVERSE);
        }
        else
            mvwprintw(vim, y, x, "%s", modes[i]);
        x = x+2;
    }
    wrefresh(vim);

}

void TerminalPrint (WINDOW *terminal, int input, int x, int y){
    mvwprintw(terminal, y, x, "%c", input);
    wrefresh(terminal);
}
