#include <stdio.h>
#include <curses.h>
#include <fcntl.h>
#include "dm_help.h"

/* to compile type "gcc dm_help.c -lcurses -ltermcap -o dm_help" */
extern void display_error();
extern void view_file();
extern int print_menu();
extern int menu_opt();


/*=================================================================*/
main(){

    initscr();
    clearok(stdscr,TRUE);
    cbreak();
    idlok(stdscr, TRUE);
    while(menu_opt(main_menu) != -1);
    clear();
    refresh();

endwin();
}

/*=================================================================*/

void display_error(msg)
char *msg;
{
/* This function will disply the given error message (msg) in the *
 * center fo the screen, wait for the user to press a key and     *
 * clear and return to the  calling function */

    int v = 0,h = 0;

    v = LINES / 2;
    h = (COLS/2) - ((strlen(msg)/2)+1);
    h = (h <0 ) ? 0 : h;

    clear();
    mvprintw(v,h,"%s",msg);
    refresh();
    noecho();
    getch();
    echo();
    clear();

    return;
}
/*=================================================================*/

/*=================================================================*/
void view_file(fname)
char *fname;
{
/* This function allows a user to view a scroll-down text file.      *
 * The name of the text file is passed in, the file is open and      *
 * a check of the file is read to memory buffer.  Only disp_m        *
 * lines of the buffer will be displayed at one time.  When the      *
 * screen has been entirely printed a next check of the file is read *
 * This process is repeated until EOF is reached.  The user has      *
 * the options to scroll the page or single line at a time, or quit  */

    char    buf[BUFSIZE+1], fpath[80];
    char    ch;
    int     lb ,max_ch,i;
    int     fd;
    int     disp_m, lines = 0;

    clear();
    noecho();

    disp_m = LINES-3;           /*calculate max txt display */
    buf[BUFSIZE] = 0;

    clear();                   /* clear screen and positon cursor */
    move(0,0);

    sprintf(fpath,"%s/%s",PATH,fname);
    fd = open(fpath,O_RDONLY,0);

    if (fd == -1){
        display_error("Can not load file.\n");
        return;
    }

    /*while not EOF */
    while((max_ch = read(fd,buf,BUFSIZE))>0){
        lb = 0;

        /*Analyize characer in mem and print lines to screen */
        for(i=0;i<max_ch;i++){
            if(buf[i] == '\n'){
                buf[i] = 0;
                printw("%s\n",&buf[lb]);

                lines++;
                lb = i+1;
            }

            /* display txt max is reached */
            if (lines >= disp_m){
                    mvprintw(disp_m+1,0,"Press [<return>, <space>, q]");
                    refresh();
                    ch = getch();

                    switch(ch){
                        case 32:        /* space bar */
                            move(disp_m+1,0);
                            clrtoeol();
                            scrollok(stdscr, TRUE); 
                            scroll(stdscr);
                            scrollok(stdscr, FALSE); 

                            lines = disp_m -1;
                            move(disp_m-1,0);
                        break;
                        case 10:        /* return */
                            clear();
                            refresh();
                            lines = 0;
                            move(0,0);
                        break;
                        case 'q':
                            close (fd);
                            clear();
                            refresh();
                            return;
                        break;
                        case 12:        /*control l*/
                            wclear(curscr);
                            move(disp_m+1,0);
                            clrtoeol();
                    	    mvprintw(disp_m+1,0,"Press [<return>, <space>, q]");
                            refresh();
                        break;    
                    }
            }   
        }  /* for */

        if (max_ch < BUFSIZE) {
            mvprintw(disp_m+1,0,"End Of File (key any key to exit):");
            refresh();
            getch();
            return;
        }
        if (lb  != i){
            printw("%s",&buf[lb]);
        }

    }  /* while */

    close(fd);
    clear();
    echo();
    return;
}

/*=================================================================*/
int print_menu(menu_t menu){
char *menu;


/* this function uses the curses package to print out a menu  *
 * of type menu_t to the stdscr */

    int     n,i=0;
    char    ch;

    clear();
    while(1) {
            while(menu[i].desc){
                mvprintw(MARGT+i,MARGL,"%c) %s",'a'+i,menu[i].desc);
                i++;

                if (i> MAXENTRY)           /*prevent menu overflow */
                    break;
            }

            mvprintw(i+MARGT+1,MARGL,"Menu Selection: ");
            refresh();
            ch = getch();
            n  = (ch == 'B') ? 'B' : (int)(ch - 'a');
            n  = (ch == 'Q') ? -1 : n;

            if ((n >= -1 && n < i) || (n == 'B'))
                break;
            else{
                addch(8);
                delch();
                refresh();
            }
    }
    return (n);
}

/*=================================================================*/
/*=================================================================*/

int menu_opt(menu_t menu)
char *menu;
{
/* The menu_opt function takes a given menu, calls the necessary *
 * functions to display the menu and execute any of the menu's   *
 * options.  If the menu contains a submenu, a recursive call is *
 * made to menu_opt.  The menu_opt will exit when the user       *
 * 'Q' as menu choice */

    int     i, num;

    while(1){
        i = print_menu(menu);

        if (i == -1 || i == 'B' )
            break;
        
        if (!strncmp(menu[i].fname,"SUBMENU",7)){
            if (!isdigit(menu[i].fname[7])){
                display_error("No sub-menu exists.\n");
                continue;
            }

            num = atoi(&(menu[i]).fname[7]);
            if (num <  TSUBMENU)
                i = menu_opt(sub_menu[num]);

            if (i ==-1) 
                break;
        }
        else 
            view_file(menu[i].fname);
    }
    return (i);
}
