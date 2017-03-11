#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>         
#define TALLYFILE "/home/mordor/log/vote_tally"
#define VOTEPATH "/home/mordor/player/vote/"
#define ISSUEFILE "/home/mordor/post/ISSUE"
 

typedef struct vote {
    char    *issue;
    int     support;
    int     against;
    int     blank;
} vote;
typedef struct dirent DIRS;

/*****************************************************************/
void presult(vote vt[],int number, int total, char *c)
/* the presult function dsiplas the result of all the ballots. */
{
    int i;
    FILE    *fp;

    fp = fopen(TALLYFILE,"w");
    printf("Total Number of Voters: %d\n",total);
    printf("Total Number of Issues:%d\n",number);
    printf("Voting restriction: %s\n\n",(c[0]) ? c : "none");

    fprintf(fp,"Total Number of Voters: %d\n",total);
    fprintf(fp,"Total Number of Issues:%d\n",number);
    fprintf(fp,"Voting restriction: %s\n\n",(c[0]) ? c : "none");
    for(i=0;i<number;i++){
        printf("%s    support:%-8d  against:%-8d blank:%-8d\n",
            vt[i].issue,vt[i].support,vt[i].against,vt[i].blank);
        fprintf(fp,"%s    support:%-8d  against:%-8d blank:%-8d\n",
            vt[i].issue,vt[i].support,vt[i].against,vt[i].blank);
    }
    fclose(fp);
    return;
}
/*****************************************************************/

/*****************************************************************/
vote *elect_setup(char class[], int *number)
/* The elect_setup functions reads in the set up information    *
 * regarding ther election (number of issues, class restriction) *
 * Elect_setup returns a pointer to an array  oftype vote, which *
 * has been initialized (issues are loaded).                     */
{
    FILE    *fp;
    vote    *vt;
    char    tmp[1024];
    int     i, n;

    class[0] =0;
    fp = fopen(ISSUEFILE,"r");

    if(!fp){
        printf("No Issues to.\n");
        exit(0);
    }

    fgets(tmp,1024,fp);
    if (feof(fp))
        return;

    n = sscanf(tmp,"%d %s",number,class);
    if (n == 1)
        class[0] = 0;

    if (!n)
        return;

    if (n == 1)
        class[19] = 0;

    vt = (vote *) malloc(sizeof(vote)*(*number));

    i= 0;
    fgets(tmp,1024,fp);

    while (!feof(fp)){

        tmp[1023] = 0;
        n = strlen(tmp);
        vt[i].issue = (char *)malloc(n+1);
        strcpy(vt[i].issue,tmp);
        vt[i].issue[n] = 0;
        vt[i].support = 0;
        vt[i].against = 0;
        vt[i].blank = 0;
        i++;
        if(i == *number)
            break;

        fgets(tmp,1024,fp);
    } 

    if (feof(fp))
        *number = i;
    fclose(fp);

    return vt;
}
/*****************************************************************/

/*****************************************************************/
int read_ballot(vote *vt,int number,int del)
/* The read_ballot function reads all the voter ballots in the  *
 * voting directory, and tellies the votes.                     */
{
    int     i,t=0;
    char    tmp[1024], str[80];
    DIR     *dirfd;
    DIRS    *dirp;
    FILE    *fp;

    if ((dirfd = opendir(VOTEPATH)) == NULL){
            printf("Directory could not be opened.\n");
            exit(0);
    }
    while((dirp = readdir(dirfd)) != NULL){
        if (dirp->d_name[0] == '.')
            continue; 
 
        if(!isupper(dirp->d_name[0]))
            continue;   

    sprintf(str,"%s/%s",VOTEPATH,dirp->d_name);
        fp = fopen(str,"r");

    if(!fp){
        printf("Can not open ballot:%s.\n",dirp->d_name);
        continue;
    }

        fgets(tmp,1024,fp);
        tmp[number]=0;
        fclose(fp);

		if (del)
			unlink(str);

        if (strlen(tmp) != number){
            continue;
        }

        for(i=0;i<number;i++)
            if(tmp[i] == 'A')
                vt[i].against++;
            else if(tmp[i] == 'S')
                vt[i].support++;
            else
                vt[i].blank++;
     
        t++;
    }                    
    closedir(dirfd);    
    return t;
}
/*****************************************************************/
/*****************************************************************/
main(int argc, char *argv[]) {
    int     number, total, del = 0;
    char    class[20];
    vote    *vt;

    if  (argc ==2)
        if(!strcmp(argv[1],"-d")){
			printf("tallying and deleting ballots.\n");
            del = 1;
		}
    vt = elect_setup(class,&number);
    total = read_ballot(vt,number,del);
    presult(vt,number,total,class);
}
