int dm_alias (creature, ply_ptr)
creature	*ply_ptr;
char		creature;
{
creature	*crt_ptr;
int		fd, cfd;

	fd = ply_ptr->fd;

	crt_ptr = find_crt(
