#include <stdio.h>              /*for input and output */      
#include <string.h>             /*for string functions */      
#include <stdlib.h>             /**/
#include <unistd.h>             /**/
#include <sys/types.h>          /*used for the socket function */
#include <sys/socket.h>         /*for socket functions */      
#include <netdb.h>              /*for address information and conversions, also holds the hostent structure */
#include <netinet/in.h>         /*for internet addresses */    
#include <arpa/inet.h>          /**/
#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>

typedef enum PACKET_TYPE { 
	RD_REQ, 
	WR_REQ,
	DATA,
	ACK,
	ERR
}

int rw_req_packet(uint16_t opcode, char *filename, char *mode);
int ack_packet(uint16_t opcode, uint16_t block);
int data_packet(uint16_t opcode, uint16_t block, char *data);
int error_packet(uint16_t opcode, uint16_t error_code, char *errmsg);


