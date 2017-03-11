/***************************************************************************
 *  combat.c added by Charles Marchant
 *  Jan 12 1995 to replace old combat
 */


#include "mstruct.h"
#include "mextern.h"

#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

int update_combat(crt_ptr)
creature *crt_ptr;
{
   creature *att_ptr;
   room     *rom_ptr;
   etag     *ep;
   char     *enemy;
   int      n = 0,rtn = 0,p = 0,fd,t = 0;
   


   rom_ptr = crt_ptr->parent_rom;
   ep = crt_ptr->first_enm;
   while(1)
     {
       enemy = ep->enemy;
       if(!enemy)
	  ep = ep->next_tag;
       if(!ep)
	  return 0;
       if(enemy)
	  break;
     }
    
   att_ptr = find_crt(crt_ptr, rom_ptr->first_ply,enemy, 1);
   if(!att_ptr)
      att_ptr = find_crt(crt_ptr, rom_ptr->first_mon,enemy, 1);
   if(!att_ptr)
   {
     if(!find_who(enemy))
       del_enm_crt(enemy, crt_ptr);
     else
       end_enm_crt(enemy, crt_ptr);
     return 0;
   }
   
if(att_ptr != crt_ptr) {
   if(is_charm_crt(crt_ptr->name, att_ptr)&& F_ISSET(crt_ptr, MCHARM))
      p = 1;
   crt_ptr->NUMHITS++;
   n=20;
   if(F_ISSET(crt_ptr, MMAGIO))
      n=crt_ptr->proficiency[0];
	         
   if(F_ISSET(crt_ptr, MMAGIC) && mrand(1, 100) <= n && !p) 
   {
     rtn = crt_spell(crt_ptr, att_ptr);
     if(rtn == 2) 
        return 1; 
     else if(rtn == 1)
        n = 21;
     else 
      	n = crt_ptr->thaco - att_ptr->armor/10;
   }
   else 
   {
     n = crt_ptr->thaco - att_ptr->armor/10;
     n = MAX(n, 1);
   }
   
   
   if(mrand(1,20) >= n && !p) 
    {
      fd = att_ptr->fd;
      ANSI(fd, RED); 
      if(F_ISSET(crt_ptr, MBRETH) && mrand(1,30)<5)
      {
       if (F_ISSET(crt_ptr, MBRWP1) && !F_ISSET(crt_ptr, MBRWP2))
         n = bw_spit_acid(crt_ptr,att_ptr);
       else if (F_ISSET(crt_ptr, MBRWP1) && F_ISSET(crt_ptr, MBRWP2))
         n = bw_poison(crt_ptr,att_ptr);
       else if (!F_ISSET(crt_ptr, MBRWP1) && F_ISSET(crt_ptr, MBRWP2))
         n = bw_cone_frost(crt_ptr,att_ptr);
       else
         n = bw_cone_fire(crt_ptr,att_ptr);
      }
      else if(F_ISSET(crt_ptr, MENEDR) && mrand(1,100)< 25) 
        n = power_energy_drain(crt_ptr,att_ptr);
      else
        n = mdice(crt_ptr);		     
    
        att_ptr->hpcur -= n;
                
	print(fd, "%M hit you for %d damage.\n",crt_ptr, n);
	/* Output only when monster v. monster */
	 if (att_ptr->type == MONSTER && crt_ptr->type == MONSTER) { 
        	broadcast_rom2(fd,crt_ptr->fd,att_ptr->rom_num,"%M hits %m.", 
		       crt_ptr, att_ptr);
		add_enm_crt(crt_ptr->name, att_ptr);
	}
	add_enm_dmg(att_ptr->name, crt_ptr,n);

        if(F_ISSET(crt_ptr, MPOISS) && mrand(1,100) <= 15) 
	{
          print(fd, "%M poisoned you.\n",crt_ptr);
          F_SET(att_ptr, PPOISN);
        }

        if(F_ISSET(crt_ptr, MDISEA) && mrand(1,100) <= 15) 
	{
          print(fd, "%M infects you.\n",crt_ptr);
          F_SET(att_ptr, PDISEA);
        }
		
	if(F_ISSET(crt_ptr, MBLNDR) && mrand(1,100) <= 15) 
	{
          print(fd, "%M  blinds your eyes.\n",crt_ptr);
          F_SET(att_ptr, PBLIND);
	}
  
        if(F_ISSET(crt_ptr, MDISIT) && mrand(1,100) <= 15) 
           dissolve_item(att_ptr,crt_ptr);
                
        ANSI(fd, WHITE);

        n = choose_item(att_ptr);

        if(n) 
	{
          if(--att_ptr->ready[n-1]->shotscur<1) 
	  {
            print(fd,"Your %s fell apart.\n",att_ptr->ready[n-1]->name);
            broadcast_rom(fd,att_ptr->rom_num,"%M's %s fell apart.",
                          att_ptr,att_ptr->ready[n-1]->name);
            add_obj_crt(att_ptr->ready[n-1],att_ptr);
            att_ptr->ready[n-1] = 0;
            compute_ac(att_ptr);
          }
	}
        
        if(att_ptr->type == PLAYER && !F_ISSET(att_ptr, PNOAAT) && !p)
	{      
             rtn = attack_crt(att_ptr, crt_ptr);
             if(rtn) 
		return 1;
	}
	else 
	{      
          if(LT(att_ptr,LT_ATTCK) < t)
	  {				   
	    rtn = attack_crt(att_ptr, crt_ptr);
            att_ptr->lasttime[LT_ATTCK].ltime = t;
	    if(rtn) 
	      return 1;
	  }
	}
                
	if(att_ptr->hpcur < 1) 
	{
           ANSI(fd, MAGENTA);
           print(fd, "%M killed you.\n", crt_ptr);
           ANSI(fd, WHITE);
           if(att_ptr->type == MONSTER && crt_ptr->type == MONSTER) {
		broadcast_rom2(att_ptr->fd,crt_ptr->fd,att_ptr->rom_num,"%M killed %m.",
                       crt_ptr, att_ptr);
	   }
	   die(att_ptr, crt_ptr);
           return 1;
	}
        else if(F_ISSET(att_ptr, PWIMPY)) 
        {
          if(att_ptr->hpcur <= att_ptr->WIMPYVALUE) 
	  {
            flee(att_ptr);
            return 1;
	  }
	}
        else if(F_ISSET(att_ptr, PFEARS)) 
	{
	  int ff;
	  ff = 40 + (1- (att_ptr->hpcur/att_ptr->hpmax))*40 +
	       bonus[att_ptr->constitution]*3 + 
	       (att_ptr->class == PALADIN) ? -10 : 0;
          if(ff < mrand(1,100)) 
	  {
            flee(att_ptr);
                     
	  }
	}
       
   }
   else if(n <= 20 && !p) 
   {
     ANSI(att_ptr->fd, CYAN);
     print(att_ptr->fd, "%M missed you.\n", crt_ptr);
    /* Output only when monster v. monster */
     if (att_ptr->type == MONSTER && crt_ptr->type == MONSTER) { 
	     broadcast_rom2(att_ptr->fd,crt_ptr->fd, 
       		att_ptr->rom_num,"%M misses %m.", crt_ptr, att_ptr);
	     add_enm_crt(crt_ptr->name, att_ptr);
     }
     ANSI(att_ptr->fd, WHITE);
     if(att_ptr->type == PLAYER && !F_ISSET(att_ptr, PNOAAT))
     {      
         rtn = attack_crt(att_ptr, crt_ptr);
         if(rtn) 
	    return 1;
     }
     else 
     {      
       if(LT(att_ptr,LT_ATTCK) < t)
       {			
	 rtn = attack_crt(att_ptr, crt_ptr);
         att_ptr->lasttime[LT_ATTCK].ltime = t;
	 if(rtn) 
	    return 1;
       }
     }
   }
 }
  return 0;
}


/***************************************************************************
 *  breath weapons 
 */

int bw_spit_acid(crt_ptr,vic_ptr)
creature *crt_ptr;
creature *vic_ptr;
{
  int fd;
  int n;
  fd = vic_ptr->fd;
  print(fd, "%M spits acid on you!\n", crt_ptr);
  broadcast_rom(fd,vic_ptr->rom_num,"%M spits acid on %m!",crt_ptr,vic_ptr);
  n = dice(crt_ptr->level, 3, 0);
  return n;
}

int bw_poison(crt_ptr,vic_ptr)
creature *crt_ptr;
creature *vic_ptr;
{
   int fd;
   int n;
   fd = vic_ptr->fd;
   print(fd, "%M breathes poisonous gas on you!\n", crt_ptr);
   broadcast_rom(fd, vic_ptr->rom_num,
                 "%M breathes poisonous gas on %m!", crt_ptr, vic_ptr);
   n = dice(crt_ptr->level, 2, 1);
   print(fd, "%M poisoned you.\n", crt_ptr);
   F_SET(vic_ptr, PPOISN);
   return n;
}

int bw_cone_frost(crt_ptr,vic_ptr)
creature *crt_ptr;
creature *vic_ptr;
{
   int fd;
   int n;
   fd = vic_ptr->fd;
   print(fd, "%M breathes frost on you!\n", crt_ptr);
   broadcast_rom(fd,vic_ptr->rom_num,"%M breathes frost on %m!",
		 crt_ptr,vic_ptr);
   if(F_ISSET(vic_ptr, PRCOLD))
     n = dice(crt_ptr->level, 2, 0);
   else
     n = dice(crt_ptr->level, 4, 0);
   return n;
}

int bw_cone_fire(crt_ptr,vic_ptr)
creature *crt_ptr;
creature *vic_ptr;
{
   int fd;
   int n;
   fd = vic_ptr->fd;
   print(fd, "%M breathes fire on you!\n", crt_ptr);
   broadcast_rom(fd, vic_ptr->rom_num,"%M breathes fire on %m!", 
		 crt_ptr, vic_ptr);
   if(F_ISSET(vic_ptr, PRFIRE))
      n = dice(crt_ptr->level, 2, 0);
   else
      n = dice(crt_ptr->level, 4, 0);
   return n;
}
   
int power_energy_drain(crt_ptr,vic_ptr)
creature *crt_ptr;
creature *vic_ptr;
{
  int n;
  int fd;
  fd = vic_ptr->fd;
  n = dice(crt_ptr->level, 5, (crt_ptr->level)*5);
  n = MAX(0,MIN(n,vic_ptr->experience));
  print(fd, "%M drains your experience!\n", crt_ptr);
  broadcast_rom2(crt_ptr->fd,vic_ptr->fd, vic_ptr->rom_num,
		 "%M energy drains %m!", crt_ptr, vic_ptr);
  vic_ptr->experience -= n;
  lower_prof(vic_ptr,n);
  print(fd, "%M drains you for %d experience.\n",crt_ptr,n);
  return 0;
}
