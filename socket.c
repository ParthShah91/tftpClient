
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
	//memcpy (&server.sin_addr, host_info->h_addr, host_info->h_length);	/*set address of server taken from gethostbyname function */

	//printf("IP address: %d.%d.%d.%d\n", server_ip_address[0], server_ip_address[1], server_ip_address[2], server_ip_address[3]);
	printf("server_ip_address: %s\n", server_ip_address);
	if(inet_aton(server_ip_address, &server.sin_addr) == 0)
	{
		perror("<createSocket> inet_aton");
		return -1;
	}
	printf("s_addr: %u\n", server.sin_addr.s_addr);
	server.sin_port = htons (S_PORT);	/*pick a free port */
	
	return sock;
}
