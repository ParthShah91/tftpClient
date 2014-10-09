
#include "tftp.h"
 
int createSocket ()
{
	int sock;
	printf("createSocket\n");
	if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf ("Client: Socket could not be created");
		return -1;
	}
	printf("sock: %d\n", sock);

	/*set the address values for the server */
	memset (&server, 0, sizeof (server));	/*Clear the structure */
	server.sin_family = AF_INET;	/*address family for TCP and UDP */

	printf("server_ip_address: %s\n", server_ip_address);
	if(inet_aton(server_ip_address, &server.sin_addr) == 0)
	{
		perror("<createSocket> inet_aton");
		return -1;
	}

	printf("s_addr: %u\n", server.sin_addr.s_addr);
	server.sin_port = htons (S_PORT);
	
	return sock;
}
