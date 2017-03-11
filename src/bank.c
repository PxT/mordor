/*
 * BANK.C
 * 
 * Banking routines for Mordor6.0
 * 
 * Copyright (C) 2000,2001	Paul Telford & Brooke Paul
 * $Id: bank.c,v 6.23 2001/07/29 16:54:38 develop Exp $
 *
 * $Log: bank.c,v $
 * Revision 6.23  2001/07/29 16:54:38  develop
 * *** empty log message ***
 *
 * Revision 6.23  2001/07/29 16:35:40  develop
 * fixed check for $ in dm setting player balance
 *
 * Revision 6.22  2001/07/26 01:40:07  develop
 * DMs can now set player balances
 *
 * Revision 6.21  2001/07/22 19:03:06  develop
 * DMs can now see players balances
 *
 * Revision 6.20  2001/07/21 23:46:21  develop
 * ibid
 *
 * Revision 6.19  2001/07/21 23:41:47  develop
 * fixing problems with DMs viewing players balances
 *
 * Revision 6.18  2001/07/21 23:38:47  develop
 * *** empty log message ***
 *
 * Revision 6.17  2001/07/21 23:34:52  develop
 * added ability for DMs to view players bank accounts
 *
 * Revision 6.16  2001/07/21 15:22:07  develop
 * no longer allow deposits less than $10
 *
 * Revision 6.15  2001/07/20 19:32:25  develop
 * fixed bug in bank while loop
 *
 * Revision 6.14  2001/07/17 19:25:11  develop
 * Added transfer and modify_account_balance
 * Also net_worth
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

#define NETWORTH

int bank_balance(creature *ply_ptr, cmd *cmnd)
{

	int 		fd, crtloaded=0;
	long		amount;
	creature	*crt_ptr;

	fd = ply_ptr->fd;

	if(!F_ISSET(ply_ptr->parent_rom, RBANKR) && ply_ptr->class < DM) {
		output(fd, "This is not a bank.\n");
		return(0);
	}
	if(ply_ptr->class == DM && cmnd->num > 1) {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
       		crt_ptr = find_who(cmnd->str[1]);

        	if(!crt_ptr) {
               		cmnd->str[1][0] = up(cmnd->str[1][0]);
                	if(load_ply(cmnd->str[1], &crt_ptr) < 0){
                        	output(fd,"Player does not exist.\n");
                        	return (0);
                	}
                crtloaded=1;
        	}
		if(cmnd->num ==3 ) {
			if(cmnd->str[2][0] == '$') {
				amount = atol(&cmnd->str[2][1]); 
				crt_ptr->bank_balance = amount;
			} else
				output(fd, "Change balanace to what?\n");
		}
		sprintf(g_buffer, "\n%s's current bank balance: $%ld\n", crt_ptr->name, crt_ptr->bank_balance);
		output(fd, g_buffer);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
	} else {

	sprintf(g_buffer, "\nCurrent bank balance: $%ld\n", ply_ptr->bank_balance);
	output(fd, g_buffer);
#ifdef NETWORTH
	/* just for fun, undefine if you don't like it */
	sprintf(g_buffer, "Your net worth: $%ld\n", net_worth(ply_ptr));
	output(fd, g_buffer);
#endif
	}
	return(PROMPT);
}

int bank_deposit(creature *ply_ptr, cmd *cmnd)
{

	int fd;
	long amount;

	fd = ply_ptr->fd;

	if(!F_ISSET(ply_ptr->parent_rom, RBANKR)) {
		output(fd, "This is not a bank.\n");
		return(0);
	}

	if(cmnd->num < 2 || cmnd->str[1][0] != '$') {
		output(fd, "Deposit how much?\n");
		return(0);
	}

	amount = atol(&cmnd->str[1][1]);
	if(amount < 1 || amount > ply_ptr->gold) {
		output(fd, "Get real.\n");
		return(0);
	}
	if(amount < 10) {
		output(fd, "The bank only accepts deposits of 10 gold or above.\n");
		return(0);
	}

	ply_ptr->gold -= amount;
	bank_account_modify(ply_ptr, amount);

	sprintf(g_buffer, "You deposit %ld gold pieces.\nThank you for your patronage.", amount);
	output(fd, g_buffer);
	ply_ptr->bank_balance = MAX(0, ply_ptr->bank_balance);
	sprintf(g_buffer, "Your balance is now: $%ld.\n", ply_ptr->bank_balance);
	output(fd, g_buffer);

	return(PROMPT);
}


/* make a withdrawl from your bank account
 */
int bank_withdrawl(creature *ply_ptr, cmd *cmnd)
{

	int fd;
	long amount;

	fd = ply_ptr->fd;

	if(!F_ISSET(ply_ptr->parent_rom, RBANKR)) {
		output(fd, "This is not a bank.\n");
		return(0);
	}

	if(cmnd->num < 2 || cmnd->str[1][0] != '$') {
		output(fd, "Withdraw how much?\n");
		return(0);
	}

	amount = atol(&cmnd->str[1][1]);
	if(amount < 1 || amount > ply_ptr->bank_balance) {
		output(fd, "Get real.\n");
		return(0);
	}

	bank_account_modify(ply_ptr, -amount);

	ply_ptr->gold += amount;

	sprintf(g_buffer, "You withdraw %ld gold pieces.  You now have %ld gold and %ld gold in the bank.\n",
			amount, ply_ptr->gold,ply_ptr->bank_balance);
	output(fd, g_buffer);

	return(PROMPT);

}

/* transfer_balance
 * transfer gold to a player with overdraft protection 
 */
int transfer_balance(creature *ply_ptr, cmd *cmnd)
{

	long amount,temp,temp2;
	creature *rcv_ptr;
	int fd,crtloaded=0;

	fd = ply_ptr->fd;

	if(!F_ISSET(ply_ptr->parent_rom, RBANKR)) {
		output(fd, "This is not a bank.\n");
		return(0);
	}

	if(cmnd->num < 3 || cmnd->str[1][0] != '$') {
		output(fd, "Syntax: transfer $amount player\n");
		return(0);
	}

	amount = atol(&cmnd->str[1][1]);
	if(amount < 1 ) {
		output(fd, "Get real.\n");
		return(0);
	}

	/* if there's not enough in the bank, we do an automatic deposit to cover it if possible */
	if(amount > ply_ptr->bank_balance) {
		temp = amount - ply_ptr->bank_balance;
		temp2 = ply_ptr->gold;
		if(temp2 > temp) {
			sprintf(g_buffer, "\nTaking %ld gold from inventory to cover the transfer.\n",temp);
			output(fd, g_buffer);
			bank_account_modify(ply_ptr, temp);
			ply_ptr->gold -= temp;
		}
		else {
			output(fd, "You don't have that much gold!\n");
			return(0);
		}
	}

	lowercize(cmnd->str[2],1);
	rcv_ptr = find_who(cmnd->str[2]);
	if(!rcv_ptr) {
		if(load_ply(cmnd->str[2], &rcv_ptr) < 0){
	                output(fd,"Player does not exist.\n");
	                return (0);
		}
		crtloaded = 1;
	}

	if(rcv_ptr->class >= IMMORTAL) {
		output(fd, "You can't transfer to that person.\n");
		if(crtloaded)
			free_crt(rcv_ptr);
		return(0);
	}
	sprintf(g_buffer, "You transfer %ld gold to %s.",
			amount, rcv_ptr->name);

	bank_account_modify(rcv_ptr, amount);
	bank_account_modify(ply_ptr, -amount);

	output(fd, g_buffer);
	if(crtloaded) {
		save_ply(rcv_ptr);
		free_crt(rcv_ptr);
	}

	return(0);
}


/* bank_account_modify
 * 
 * Make account balance changes to an arbitrary player
 * Returns -1 if the balance would be reduced beyond 0, 0 otherwise
 */
int bank_account_modify(creature *ply_ptr, long amount)
{


	if(amount == 0)
		return(0);

	if( (ply_ptr->bank_balance + amount) < 0)
		return(-1);

	ply_ptr->bank_balance += (amount);

	ply_ptr->bank_balance = MAX(0, ply_ptr->bank_balance);

	return(0);
}

/* net_worth
 * returns the total value of the given player's:
 * inventory (curshots > 1)
 * worn items (curshots > 1)
 * gold
 * gold in bank 
 */
long net_worth(creature *ply_ptr)
{

	long worth;
	int i;
	otag *op,*cop;

	worth = ply_ptr->bank_balance;
	worth += ply_ptr->gold;
	
	for(i=0;i<MAXWEAR;i++) {
		if(ply_ptr->ready[i] && ply_ptr->ready[i]->shotscur > 0)
			worth += ply_ptr->ready[i]->value;
	}
		
	op = ply_ptr->first_obj;
	while(op) {
		if(op->obj->type == CONTAINER) {
			cop=op->obj->first_obj;
			while(cop) {
				if(cop->obj->shotscur > 0)
					worth += cop->obj->value;
				cop = cop->next_tag;
			}
		
		}

		if(op->obj->shotscur > 0) 
			worth += op->obj->value;
			
		op = op->next_tag;
	}
	
	return(worth);
}
