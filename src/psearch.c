#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#ifndef WIN32
#include <dirent.h>
#endif
#include "mstruct.h"
#include "mextern.h"

extern int psearch();
static  int ac, dam, spell = -1; 
long val ; 

main(argc, argv)
int argc;
char *argv[];

{
    object      *obj;
    char        oname[81];
    int         i,n;
    int         item,total;
    DIR     *dirfd;
struct dirent  *dirp;

	oname[0] =0;
	i = 1;
	n = ac = dam = item = total = 0;
	val = 0L;


    if (argc < 2 || (!isdigit(argv[1][0]) && argc == 2)){
        printf("Too few arguements.\n");
        printf("syntax: psearch <[object #] [-adnsv #] [-N <name>]> [-P <players>]\n");
        exit(1);
    }

    
    if(isdigit(argv[1][0])) {
        item = atoi(argv[1]);
        i = 2;
    }

    for(i;i<argc;i++)
        if(argv[i][0] == '-' && ((i+1) < argc)){
            if (argv[i][1] == 'P'){
                n = i+1;
                break;
            }
        
            if (argv[i][1] == 'N'){
                strncpy(oname,argv[i+1],80);
	        i++;
                continue;
            }
            if (isdigit(argv[i+1][0])){
                switch(argv[i][1]){
                    case 'a':
                        ac = atoi(argv[i+1]);
                        break;
                    case 'd':
                        dam = atoi(argv[i+1]);
                        break;
                    case 'v':
                        val = atol(argv[i+1]);
                        break;
                    case 'n':
                        item = atoi(argv[i+1]);
                        break;
                    case 's':
                        spell = atoi(argv[i+1]);
                        break;
                    default:
                        printf("invalid option.\n");
                        exit(1);
                        break;
                }   
	        i++;
            }
            else{
                printf("invalid value.\n");
                exit(1);
            }
        }
        else{
            printf("syntax error.\n");
            exit(1);
        }


    if (item){
        if (load_obj(item,&obj) < 0) {
            printf("object load error.\n");
            exit(1);
        }
        strcpy(oname,obj->name);
        printf("Searching for: %s (%d)\n",oname,item);
        free_obj(obj);
    }
    else if (oname[0])
        printf("Searching for: %s\n",oname);

    if(ac)
        printf("Check for items with ac value >= %d\n",ac);

    if(dam)
        printf("Check for items with max dam. >= %d\n",dam);

    if(val)
        printf("Check for items with value >= %d\n",val);

    printf("\n");
    
    /* use given player names */
    if(n)   
        for(i=n;i< argc;i++){
            if(!isupper(argv[i][0]))
                continue;   
            total += psearch(argv[i],oname);
        }
    else{
        if ((dirfd = opendir(PLAYERPATH)) == NULL){
            printf("Directory could not be opened.\n");
            return;
        }
 
        while((dirp = readdir(dirfd)) != NULL){
            if (dirp->d_name[0] == '.')
                continue; 
 
            if(!isupper(dirp->d_name[0]))
                continue;   
            total += psearch(dirp->d_name,oname);
        }       
 
        closedir(dirfd);    
    }
    printf("\nTotal Players examined: %d\n",total);
    return; 
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define MDAM(x) (((x)->sdice)*((x)->ndice)+((x)->pdice))

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int psearch(pname,oname)
char *pname;
char *oname;
{

    creature    *player;
    object      *obj;
    otag        *obj_lst, *cnt_lst;
    int         item, count, hidden;

        if(load_ply(pname, &player) < 0){
            printf("Player (%s) Load Error.\n",pname); 
            free_crt(player);
            return (0);
         }

	if (S_ISSET(player,spell) && spell != -1)
		printf("%s knows %s\n",player->name,spllist[spell].splstr);

        obj_lst = player->first_obj;
        count = 0;
        hidden = 0;


        while(obj_lst){
            if(!strcmp(obj_lst->obj->name,oname))
                count++;    

            if (ac  && obj_lst->obj->armor >= ac)
                printf("%s has %s with armor value of %d\n",
                  player->name, obj_lst->obj->name, obj_lst->obj->armor);

            if (dam  && MDAM(obj_lst->obj) >= dam)
                printf("%s has %s with max dam of %d\n",
                  player->name, obj_lst->obj->name, MDAM(obj_lst->obj));

            if (val  && obj_lst->obj->value >= val)
                printf("%s has %s with a value of %d\n",
                  player->name, obj_lst->obj->name, obj_lst->obj->value);

           if(F_ISSET(obj_lst->obj,OCONTN)){
                cnt_lst = obj_lst->obj->first_obj;
                while(cnt_lst){
                    if(! strcmp(cnt_lst->obj->name,oname)){
                        count++;
                        hidden++;
                    }

                    if (ac  && cnt_lst->obj->armor >= ac)
                        printf("%s has %s with armor value of %d\n",
                        player->name, cnt_lst->obj->name, cnt_lst->obj->armor);

                    if (dam  && MDAM(cnt_lst->obj) >= dam)
                        printf("%s has %s with max dam of %d\n",
                        player->name, cnt_lst->obj->name, MDAM(cnt_lst->obj));

                    if (val  && cnt_lst->obj->value >= val)
                        printf("%s has %s with a value of %d\n",
                        player->name, cnt_lst->obj->name, cnt_lst->obj->value);

                    cnt_lst = cnt_lst->next_tag;
                }
            }

            obj_lst = obj_lst->next_tag;
        }
    
    if(count)
        printf("%s has %d (%d in containers) %s\n",
        player->name,count,hidden,oname);
    free_crt(player);       
    return (1);
}


