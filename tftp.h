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

#define ERROR		-1
#define READ		1
#define WRITE		2
#define RTCOUNT		5
#define S_PORT		69
#define IP_ADDRESS_LEN	16	

#define DATA_SIZE	512

struct sockaddr_in server;
/* struct hostent *host_info;*/
char server_ip_address[IP_ADDRESS_LEN];
int error_code;

typedef enum _PACKET_TYPE_ { 
	RD_REQ=1, 
	WR_REQ,
	DATA,
	ACK,
	ERR
} PACKET_TYPE;

#if 0
typedef enum _encoding_type_ {
	NET_ASCII,
	OCTET
} encoding_type;
#endif

int rw_req_packet(int socketFd, unsigned char req_type, char *filename, char* type);
int ack_packet(int socketFd, uint16_t block, char *ackbuf);
int data_packet(uint16_t opcode, uint16_t block, char *data);
/* int error_packet(int socketFd, uint16_t error_code, char *errmsg, char *buf);*/
int error_packet(int socketFd, uint16_t error_code, char *errmsg);
int createSocket();


char buf[DATA_SIZE];

#if 0
char err_msg [7][40] = {"Not defined, see error message if any",
	"File not fount",
	"Access Violation",
	"Disk full, or allocation exceeded",
	"Illegal TFTP operation",
	"Unknown transfer ID",
	"File already exists"};
#endif


