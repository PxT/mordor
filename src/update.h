#define MAX_TEXIT 5

typedef struct {
	int	room;
	char    *name1, *name2;
	int	exit1,exit2;
	char	*mess1, *mess2;
} tx_tag;

static t_toggle =0; 
static tx_tag   time_x[MAX_TEXIT] =
{ {3311,"up","ocean", 951, 3161,
   "The Widow Maker sets sail to uncharted waters.","The Widow Maker has docked."},
  {951,"down","down",3311, 362,NULL,NULL},
  {3160,"down",NULL,3161,0,NULL,NULL},
  {362,NULL,"up",0,951,NULL,NULL},
  {3161,"rope","dock",3160,3311,NULL,NULL}} ;
