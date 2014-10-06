
#include <tftp.h>
 
int createSocket (struct sockaddr_in *server);
{
	int sock;

	if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf ("Client: Socket could not be created");
		return -1;
	}

	/*set the address values for the server */
	memset (server, 0, sizeof (*server));	/*Clear the structure */
	*server.sin_family = AF_INET;	/*address family for TCP and UDP */
	memcpy (server.sin_addr, host->h_addr, host->h_length);	/*set address of server taken from gethostbyname function */
	*server.sin_port = htons (port);	/*pick a free port */
	
	return sock;
}
