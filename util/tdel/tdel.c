#include <stdio.h>
#include <stdlib.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#define PATH  "/home/mordor/player"
#define SEC_IN_DAY 86400

/* This program lists/ deletes all files that have not been modified *
 * in the last X days, where X's value is passed in as a command     *
 * arguement.  If the file list not not specified (with -F), then    *
 * the default directory (PATH) will be used. */

main(argc, argv)

int  argc;
char *argv[];
{
        DIR     *dirfd;
struct  dirent  *dirp;
struct  stat    f_stat;
struct  tm      *time_s;
        char    name[80], del =0;
        long    cur_t,last_t,tmp_t;
        int     i,n=0,total =0;

    if(argc < 2){
        printf("syntax: tdel <# days> [-d] [-F <files...>]\n");
        return;
    }

    if(!isdigit(argv[1][0])){
        printf("syntax: tdel <# days> [-d] [-F <files...>]\n");
        return;
    }
    else
        last_t = (long) (atof(argv[1])*((double) SEC_IN_DAY));

/* Parse arguement flags */
    for(i=2;i<argc;i++){
        if(argv[i][0] != '-'){
            printf("syntax error.\n");
            return;
        }

        if(argv[i][1] == 'd')
            del = 1;
        else if(argv[i][1] == 'F' && i+1 < argc){
            n = i+1;
            break;
        }
        else{
            printf("syntax error.\n");
            return;
        }
    }
    

    cur_t = time(0);
    printf("Current Date: %s\n\n",ctime(&cur_t));

    tmp_t = cur_t - last_t;

    if (del){
        printf("Delete unmodified files older than: %.24 (%s days).\n",
        ctime(&tmp_t),argv[1]);
        printf("Are you sure? (y/n) ");
        if (getc(stdin) != 'y'){
            printf("\ndeletion aborted!\n");
            return;
        }
        printf("\n");
    } else
        printf("List unmodified files older than: %.24s (%s days).\n\n",    
            ctime(&tmp_t),argv[1]);
            
    /* If arguement flaf '-F' is set, then all the file names that *
     * followed the -F flag will be examined.  If -F is not set,   *
     * then all the files in the default directory (PATH) will be  *
     * examined */

    if(n){
        for(i=n;i<argc;i++){
            if(stat(argv[i],&f_stat)){
                printf("File load stat error: %s\n",name);
                continue;
            }
            /* check if file is a regular file */
            if ((f_stat.st_mode & S_IFMT) != S_IFREG)
                continue;
            
            if ((cur_t - f_stat.st_mtime) < last_t)
                continue;

            if (del)
                unlink(argv[i]);
            else
                printf("%-15.15s   last modified: %-16.24s    size: %ld\n",
                    argv[i],ctime(&(f_stat.st_mtime)),f_stat.st_size);
            
            total++;
        }
    }
    else {
        if((dirfd = opendir(PATH)) == NULL){
            printf("Directory Load Error.\n");
            return;
        }
    
        while ((dirp = readdir(dirfd))){
            sprintf(name,"%s/%s",PATH,dirp->d_name);
            if(stat(name,&f_stat)){
                printf("File load stat error: %s\n",name);
                continue;
            }
            /* check if file is a regular file */
            if ((f_stat.st_mode & S_IFMT) != S_IFREG)
                continue;

            if ((cur_t - f_stat.st_mtime) < last_t)
                continue;
    
            if (del)
                unlink(name);
            else
                printf("%-15.15s   last modified: %-16.24s    size: %ld\n",
                    dirp->d_name,ctime(&(f_stat.st_mtime)),f_stat.st_size);
            
                total++;
        }
    }
    printf("\nTotal files %s: %d\n",(del) ? "deleted" : "listed",total);
    return;
}
