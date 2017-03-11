#include <stdio.h>
/*
 * $Id: errhandl.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: errhandl.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

void HandleError(char *pcMsg, int iErr)
{
  
  printf("%s: Err(%d)\n", pcMsg, iErr);

}


