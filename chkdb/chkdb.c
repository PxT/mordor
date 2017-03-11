/**************************************************************************
*  Find and fix bad perms in databases.  
*  Version 1.0:  (C) 1996, 1997 Brooke Paul
*  Dedicated to Styx (roy@tesla.tech.cerritos.edu)
* 
*  $Id: chkdb.c,v 1.2 2001/03/08 19:36:50 develop Exp develop $
*  $Log: chkdb.c,v $
*  Revision 1.2  2001/03/08 19:36:50  develop
*  added a spinner to default compile
*
*  Revision 1.1  2001/03/08 19:32:51  develop
*  Initial revision
*
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


#include "../include/port.h"
#include "../include/morutil.h"
#include "../include/mordb.h"

#include "chkdb.h"


int fd=1, Rlow=1, Rhi=10000, n=0, dbm, dbo, ff, changed=0, rnum;

/* OPTIONS:  Set and unset these before compilation to configure */

int verbose=1;  /* indicates verbose output -- WARNING: can be VERY verbose */ 
int cleanup=0;  /* will flush clean _all_ rooms after check */
int writeok=0;  /* permission to alter database, 0=readonly */ 
int spinner=1;  /* little spinner and message window toggle */



main(int argc, char *argv[])
{
	int  found, y, l, slot=0, dots=0;
	char str[80];
	room    *rom;
	ctag	*cp, *ctemp;
	otag	*op, *otemp;
	char	filename[256];

	signal(SIGSEGV, end);
	clearscreen();
	if(argc > 1)
	{
		if(!isdigit(argv[1][0]))
		{
			sprintf(str, "Syntax: %s <room#> \n", argv[0]);
			write(fd, str, strlen(str));
			exit(0);
		}
		Rlow = atoi(argv[1]);
		if(argv[2])
			Rhi = atoi(argv[2]);
		else
			Rhi = Rlow +1;
    }
	sprintf(str, "", 0);
	posit(0,0); 
	printf("Mordor Database Utility v1.1 (c) 1996-1998\n\n");
	if(cleanup)
		strcat(str, "All rooms will be cleaned.\n");
	if(writeok)
		strcat(str, "All changes will be saved.\n");
	else
		strcat(str, "Readonly mode. Changes not saved.\n");
	write(fd, str, strlen(str));
	posit(8,0); printf("Working ");	
	for(rnum=Rlow; rnum<Rhi; rnum++) 
	{
		n=load_rom_from_file(rnum, &rom);
		if(n<0) 
		{
			/* check for file existance and report error if it does */
			get_room_filename( rnum, filename );
			if ( file_exists(filename) )
			{
				sprintf(str, "Room %d exists but cannot be fully read.\nIt may or may not be corrupted.\n",
					rnum);
				write_message(str);
			}
			continue;
		}
		if(!rom)
		{
			/* load_rom_from_file() worked and yet we have a bad */
			/* rom pointer??? This should be reported too */
			sprintf(str, "Room %d was loaded but the room pointer retuned is NULL\n",
				rnum);
			write_message(str);
			continue;
		}
		if(spinner) 
		{
			dots++;
			if(dots > 4 )
				dots=1;
			spin(dots);
		}
		cp=rom->first_mon;
		while(cp) 
		{
			if(F_ISSET(cp->crt, MPERMT)) 
			{
				if(verbose) 
				{
				 	sprintf(str, "Room %d has a perm %s.", rnum,cp->crt);
					write_message( str );
				}
				F_CLR(cp->crt, MPERMT);
			}
			found=0;
			n=find_crt_num(cp->crt);	
			for(y=0; y<=9; y++) {
				if(rom->perm_mon[y].misc==n && n)
				{
					found=1;
					break;
				}	
			}		
			if(!found) 
			{
				if(verbose) 
				{
					sprintf(str, "Room %d has a bad perm creature: %s.", rnum, cp->crt);
					write_message( str );
				}
				/* add monster to DB */
				if(!n && writeok) 
				{
					slot=0;
					dbm=find_crt_slot();
					op=cp->crt->first_obj;
        			while(op) 
					{
                		l=find_obj_num(op->obj);
                		if(!l) {
                        	sprintf(str, "### Unique object in inventory not saved.");
							write_message( str );
                			op=op->next_tag;   
                        	continue;
                		}	
                		cp->crt->carry[slot]=(short)l;
                		slot++;
                		if(slot>9)
						{
                       		sprintf(str, "### Only first 10 objects in inventory saved.");
							write_message( str );
				      		break;
                		}
       				 	op=op->next_tag;
        			}
					
        			op=cp->crt->first_obj;
        			cp->crt->first_obj=0;
        			while(op) 
					{
                		otemp=op->next_tag;
                		free_obj(op->obj); 
                		free(op);
                		op=otemp;
        			}

					sprintf(str, "%s/m%02d", get_monster_path(), dbm/MFILESIZE);
    				ff = open(str, O_WRONLY | O_CREAT | O_BINARY, ACC);
    				if(ff == -1) 
					{
                		close(ff);
						merror("File open error", FATAL);
                		return(0);
        			}
        			l = lseek(ff, (long)((dbm%MFILESIZE)*sizeof(creature)), 0);
				    l = write(ff, cp->crt, sizeof(creature));
        			if(l < sizeof(creature))
                		merror("write_crt", FATAL);
				    close(ff);
					sprintf(str, "### Creature %d updated for room %d.", dbm, rnum);
					write_message( str );

				} /* writeok */
				/* properly perm monster */
				l=0;
				while(rom->perm_mon[l].misc) 
				{
					l++;        
				}
				if(l>9) 
				{
					sprintf(str, "### Room %d is already full of perm creatures.", rom->rom_num);
					write_message( str );
					break;
				}
				changed=1;
				if(!n)
					rom->perm_mon[l].misc=dbm;
				else				
					rom->perm_mon[l].misc=n;

				rom->perm_mon[l].interval=7200;
			} /* !found */

		cp=cp->next_tag;
			
		}

		l=0;
		while(rom->perm_mon[l].misc && l<10) 
		{
			if(rom->perm_mon[l].interval < 5) 
			{
				sprintf(str, "### Interval fixed on monster %d in room %d.", rom->perm_mon[l].misc, rnum);
				write_message( str );
			}
			rom->perm_mon[l].interval=3600;
			l++;
		}								
		op=rom->first_obj;
		while(op) 
		{
			if(F_ISSET(op->obj, OPERMT)) 
			{
				if(verbose)
				{
					sprintf(str, "Room %d has a perm %s.", rnum,op->obj);
					write_message( str );
				}
				F_CLR(op->obj,OPERMT);
			}
            found=0; 
            n=find_obj_num(op->obj);
            for(y=0; y<=9; y++) 
			{
				if(rom->perm_obj[y].misc==n && n)
				{
					found=1;
                    break;
                }
            }

            if(!found) 
			{
				if(verbose)
				{
                    sprintf(str, "Room %d has a bad perm object: %s.", rnum, op->obj);
					write_message( str );
				}
				if(!n && writeok) 
				{
					dbo=find_obj_slot();
					sprintf(str, "%s/o%02d", get_object_path(), dbo/OFILESIZE);
       				ff = open(str, O_WRONLY | O_CREAT | O_BINARY, ACC);
       				if(ff == -1) 
					{
                		close(ff);
						merror("File open error", FATAL);
                		return(0);
        			}
        			l = lseek(ff, (long)((dbo%OFILESIZE)*sizeof(object)), 0);
					op->obj->parent_rom = 0;
	       			op->obj->parent_obj = 0;
       				op->obj->first_obj = 0;
       				op->obj->parent_crt = 0;

		        	l = write(ff, op->obj, sizeof(object));
        			if(l < sizeof(object))
		                	merror("write_obj", FATAL);

       				close(ff);
					sprintf(str, "### Object %d updated for room %d.", dbo, rnum);
					write_message( str );
				}
		/* properly perm object */
				l=0;
				while(rom->perm_obj[l].misc) 
				{
					l++;        
				}
				if(l>9) 
				{
					sprintf(str, "### Room %d is already full of perm objects.", rom->rom_num);
					write_message( str );
					break;
				}
				changed=1;
				if(!n) 
					rom->perm_obj[l].misc=dbo;
				else
					rom->perm_obj[l].misc=n;

				rom->perm_obj[l].interval=(long)7200;
            } /* !found */

			op=op->next_tag;
		} 

		l=0;
		while(rom->perm_obj[l].misc && l<10) 
		{
			if(rom->perm_obj[l].interval < 5) 
			{
				sprintf(str, "### Interval fixed on object %d in room %d.", rom->perm_obj[l].misc, rnum);
				write_message( str );
            }

			rom->perm_obj[l].interval=3600;
			l++;
		}
		if(changed || cleanup) 
		{
			/* flush the room */
			cp=rom->first_mon;
			rom->first_mon=0;
			while(cp) 
			{
				ctemp = cp->next_tag;
				free_crt(cp->crt);
               	free(cp);

            	cp = ctemp;
        	}
			op = rom->first_obj;
		    rom->first_obj = 0;
        	while(op) 
			{
	            otemp = op->next_tag;
	            free_obj(op->obj);
        	    free(op);
                            
		        op = otemp;
        	}
			if(writeok) 
			{
				if(verbose) 
				{
					sprintf(str, "Saving Room %d.", rnum); 
					write_message( str );
				}
				save_rom_to_file(rnum, rom);
			}		
		}
		if(rom)
		   free_rom(rom);  
		changed=0;
	}
	posit(0,20);
	sprintf(str, "Processed %d rooms.\nDone.", Rhi-Rlow);
	write(fd, str, strlen(str));
	exit(0);
}

        


int find_obj_slot()
{
	int i, fd=1;
	char  str[80];
	object *obj;

	for(i=2; i<=OMAX; i++) 
	{
		if(load_obj_from_file(i, &obj) < 0)
			break;
		else
			free_obj(obj);
	}
	if(verbose) 
	{
		sprintf(str, "Object slot open at %d.", i);
		message(str);
	}
	return(i);
}

int find_crt_slot()
{
	int i;
	char  str[80];
	creature  *crt;

	for(i=2; i<=CMAX; i++) {
		if(load_crt_from_file(i, &crt) < 0)
			break;
		else
			free_crt( crt );
	}
	if(verbose) 
	{
		sprintf(str, "Creature slot open at %d.", i);
		message(str);
    }
	return(i);
}


void end(int code)
{
	char str[80];

	sprintf(str, "\n\nCheck DB has encountered an error.\nPlease restart at room %d.\n", rnum);
	write(fd, str, strlen(str));
}


void write_message(char *str)
{
	if(!spinner)
		write(fd, str, strlen(str)); 
	else
		message(str);
	
	return;
}
