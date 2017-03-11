/*
 *  Logic Creature Script added by Charles Marchant for 
 *  Mordor 3.x
 * 
 */


#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

int load_crt_actions(crt_ptr)
creature    *crt_ptr;
{
  ttag        *act,*a;
  char        crt_name[80];
  char        filename[256];
  char        cmdstr[80];
  char        responsestr[1024];
  int         i,count = 0;
  char        *ptr;
  FILE        *fp;
   

  if(!F_ISSET(crt_ptr,MROBOT))
     return 0;
  if(crt_ptr->first_tlk)
     return 1;
  
  
   
  strcpy(crt_name,crt_ptr->name);
  for(i=0;crt_name[i];i++)
     if(crt_name[i] == ' ')
        crt_name[i] = '_';
   
   
  sprintf(filename,"%s/talk/%s-%d-act",MONPATH,crt_name,crt_ptr->level);
  
  fp = fopen(filename,"r");
  if(!fp)
     return 0;
        
  fgets(cmdstr,80,fp);
  while(!feof(fp))
     {
       count++;
       act = (ttag *)calloc(1,sizeof(ttag));
       act->key = 0;
       act->target = 0;
       act->action = 0;
       act->response = 0;
       act->next_tag = 0;
	
       if(!act)
	  merror("load_crt_actions",FATAL);
       ptr = cmdstr;
       act->type = count;      
       while(*ptr)
       {	     
         switch(*ptr)
	  {
	   case '?': /* tester */
	     ptr++;
	     act->test_for = *ptr++;
	     if(*ptr == ':')
	       act->arg1 = atoi(++ptr);
	     break;
	   case '@': /* unconditional goto cmd */
	     ptr++;
	     act->goto_cmd = atoi(ptr);
	     break;
	   case '!': /* if not last comand success goto cmd */
	     ptr++;
	     act->if_cmd = atoi(ptr);
	     for(;*ptr && *ptr != ':';ptr++);
	     ptr++;
	     act->not_goto_cmd = atoi(ptr);
	     break;
	   case '=': /* if last command succesful goto cmd */
	     ptr++;
	     act->if_cmd = atoi(ptr);
	     for(;*ptr && *ptr != ':';ptr++);
	     ptr++;
	     act->if_goto_cmd = atoi(ptr);
	     break;
	   case '>':
	     ptr++;
	     act->do_act = *ptr++;
	     if(*ptr == ':')
	       act->arg1 = atoi(++ptr);
	     break;
	   default:
	     merror("ERROR - logic command", NONFATAL);
	     logn("Errors", "UNKOWN LOGIC COMMAND [%s] in %s\n",cmdstr,filename);
	     act->do_act = 0;
	     act->goto_cmd = 1;
             for(;*ptr;ptr++);
	     break;
	  }
	 for(;*ptr && *ptr != ' ';ptr++);
	 for(;*ptr && *ptr == ' ';ptr++);
       }/* end of command string parsing */
       fgets(responsestr,1024,fp);
       if(responsestr[0] != '*')
	  {
	    ptr = responsestr;
	    while(*ptr)
	       if(*ptr == '\n' || *ptr == '\r')
	          *ptr = 0;
	       else
	          ptr++;
	     
	    act->response = (char *)calloc(1,strlen(responsestr)+1);
	    if(!act->response)
	       merror("load_crt_action",FATAL);
	    strcpy(act->response,responsestr);
	  }
	else
	  act->response = 0;
       act->next_tag = 0;
       if(!crt_ptr->first_tlk)
	{  
	  crt_ptr->first_tlk = act;
	  act->on_cmd = 1;
	}
       else
	{
	    for(a = crt_ptr->first_tlk;a->next_tag;a = a->next_tag);
	    a->next_tag = act;
	}
       fgets(cmdstr,80,fp);
       if(feof(fp))
	  break;
     } 
  fclose(fp);  
}

