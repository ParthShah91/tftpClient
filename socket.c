
#include "tftp.h"
 
int createSocket ()
{
	int sock;

	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("Client: Socket could not be created");
		return -1;
	}

	/*set the address values for the server */
	memset (&server, 0, sizeof (server));	/*Clear the structure */
	server.sin_family = AF_INET;	/*address family for TCP and UDP */
	memcpy (&server.sin_addr, host_info->h_addr, host_info->h_length);	/*set address of server taken from gethostbyname function */
	server.sin_port = htons (S_PORT);	/*pick a free port */
	
	return sock;
}
