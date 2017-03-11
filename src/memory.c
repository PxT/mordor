/*
 *  Memory added by Charles Marchant for 
 *  Mordor 3.x
 *  Shows memory status of mordor
 */


#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

extern rsparse	Rom[RMAX];	/* Pointer array declared */
extern csparse	Crt[CMAX];
extern osparse	Obj[OMAX];


int dm_memory(ply_ptr,cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  char buf[80];
  int  i = 0; 
  int  crts    = 0;
  int  rooms   = 0;
  int  objects = 0;
  int  talks   = 0;
  int  actions  = 0;
  int  badtalk  = 0;
  long crt_mem    = 0L;
  long rom_mem    = 0L;
  long obj_mem    = 0L;
  long talk_mem   = 0L;
  long act_mem    = 0L;
  long total_mem  = 0L;
  long bt_mem     = 0L; 
  ctag        *cp;
  otag        *op;
  ttag        *tlk;
   
  for(i=0;i<RMAX;i++)
     {
       if(Rom[i].rom)
	  {
	    rooms++;
	    rom_mem += sizeof(room);
	    for(cp = Rom[i].rom->first_mon;cp;cp = cp->next_tag)
	       {
	         if(cp->crt)
		 {
		      crts++;
		      crt_mem += sizeof(creature);
		      /* add object counting on crts */
		      /* and object wear on crts */
		    
		  if(cp->crt->first_tlk)
		    {
		      tlk = cp->crt->first_tlk;
		      if(F_ISSET(cp->crt,MTALKS))
		      {		    
		       for(;tlk;tlk = tlk->next_tag)
			 {
			  talks++;
			  talk_mem += sizeof(ttag);
			  if(tlk->key) talk_mem += strlen(tlk->key);
			  if(tlk->response) talk_mem += strlen(tlk->response);
			  if(tlk->action) talk_mem += strlen(tlk->action);
			  if(tlk->target) talk_mem += strlen(tlk->target);
			 }
		      }
		      else if(F_ISSET(cp->crt,MROBOT))
		      {
		       for(;tlk;tlk = tlk->next_tag)
			 {
			  actions++;
			  act_mem += sizeof(ttag);
			  if(tlk->response) act_mem += strlen(tlk->response);
			  if(tlk->action) act_mem += strlen(tlk->action);
			  if(tlk->target) act_mem += strlen(tlk->target);
			 }	      
		      }
		      else
		      {
		        sprintf(buf,"%s has a talk and should not.",
				cp->crt->name);
			loge(buf);
			for(;tlk;tlk = tlk->next_tag)
			 {
			  badtalk++;
			  bt_mem += sizeof(ttag);
			  if(tlk->key) bt_mem += strlen(tlk->key);
			  if(tlk->response) bt_mem += strlen(tlk->response);
			  if(tlk->action) bt_mem += strlen(tlk->action);
			  if(tlk->target) bt_mem += strlen(tlk->target);
			 }
		      }
		   }
		 }
	       }
	    for(op = Rom[i].rom->first_obj;op;op = op->next_tag)
	       {
	         if(op->obj)
		    {
		      objects++;
		      obj_mem += sizeof(object);
		      /* and contents counting */
		    }
	       }
	  }
     }
     
   print(ply_ptr->fd,"Memory Status:\n");
   sprintf(buf,"Total Rooms  :   %-5d",rooms);
   print(ply_ptr->fd,buf);
   sprintf(buf,"  %ld -> Total memory\n",rom_mem);
   print(ply_ptr->fd,buf);
   sprintf(buf,"Total Objects:   %-5d",objects);
   print(ply_ptr->fd,buf);
   sprintf(buf,"  %ld -> Total memory\n",obj_mem);
   print(ply_ptr->fd,buf);
   sprintf(buf,"Total Creatures: %-5d",crts);
   print(ply_ptr->fd,buf);
   sprintf(buf,"  %ld -> Total memory\n",crt_mem);
   print(ply_ptr->fd,buf);
   sprintf(buf,"Total Actions:   %-5d",actions);
   print(ply_ptr->fd,buf);
   sprintf(buf,"  %ld -> Total memory\n",act_mem);
   print(ply_ptr->fd,buf);
   sprintf(buf,"Total Bad Talks: %-5d",badtalk);
   print(ply_ptr->fd,buf);
   sprintf(buf,"  %ld -> Total memory\n",bt_mem);
   print(ply_ptr->fd,buf);
   sprintf(buf,"Total Talks:     %-5d",talks);
   print(ply_ptr->fd,buf);
   sprintf(buf,"  %ld -> Total memory\n",talk_mem);
   print(ply_ptr->fd,buf);
   sprintf(buf,"Total Memory:    %ld\n\n",bt_mem + rom_mem + obj_mem + 
	   crt_mem + act_mem + talk_mem);
   print(ply_ptr->fd,buf);
  
   return 0;
}
