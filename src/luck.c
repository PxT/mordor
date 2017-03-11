#include <mstruct.h>
#include <mextern.h>
#include <mtype.h>

/****************************************************************/
/*			luck					*/
/****************************************************************/

/* This sets the luck value for a given player			*/

int luck(ply_ptr)
creature	*ply_ptr;
{
	int	fd, num;

	
	if(!ply_ptr)
		return(0);
	
	fd = ply_ptr->fd;

	num = ((6*ply_ptr->intelligence+2*ply_ptr->constitution)/abs(ply_ptr->alignment)+1)-(ply_ptr->gold/50,000);
	
	if(num>100)
		num=100;
	if(num<1)
		num=1;
	
	Ply[fd].extr->luck = num;

	return(num);
}
