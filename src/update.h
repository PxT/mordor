#define MAX_TEXIT 5

typedef struct {
	int	room;
	char    *name1, *name2;
	int	exit1,exit2;
	char	*mess1, *mess2;
} tx_tag;

static t_toggle =0; 
static tx_tag   time_x[MAX_TEXIT] =

 { {540,NULL,"dock", 0, 542,tx_mesg1, tx_mesg2 },
   {541,"steamboat",NULL,534, 0,NULL,NULL},
   {542,NULL,"steamboat",0,540,NULL,NULL},
   {534,"dock",NULL,541,0,NULL,NULL}} ;
