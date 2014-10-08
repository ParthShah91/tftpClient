/*
 * =====================================================================================
 *
 *       Filename:  tftp_client.c
 *
 *    Description:  tftp client application
 *
 *        Version:  0.1
 *        Created:  10/01/2014 06:53:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <stdlib.h>

#include "tftp.h"

#define MAX_DATA_SIZE		300
#define MAX_FILE_NAME_LEN	255
#define IP_ADDRESS_LEN		4
#define TIMEOUT			5   /* time out in seconds */
bool is_read_request = false, is_write_request = false;
char read_file_name[MAX_FILE_NAME_LEN], write_file_name[MAX_FILE_NAME_LEN];
unsigned char server_ip_address[IP_ADDRESS_LEN];

char data_buf[MAX_DATA_SIZE];

int rw_req_packet(unsigned char req_type, char *filename, encoding_type type)
{

}
struct rw_hdr* read_write(int opcode, char* filename, int mode)
{

}

struct ack_hdr* ack(int block)
{

}

struct data_hdr* data(int block, char* data)
{

}

int ack_wait(int sfd, char* file)
{
	fd_set rfds;
	int retval;
	struct timeval timeout;
	int i;
	int ret;

	FD_ZERO(&rfds);
	FD_SET(sfd, &rfds);
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;

	retval = select(1, &rfds, NULL, NULL, &timeout);
	if (retval == -1)
		return -1;
	else if (retval) {
		printf("Data is available now.\n");
		ret = recvfrom(sfd, data_buf, sizeof(data_buf), 0, (struct sockaddr *)&server,(socklen_t *)sizeof(struct sockaddr));
		if(ret < 0) {
			perror("Rcv error\n");
			return -1;
		}
	}
	else {
		printf("No data within five seconds.\n");
		return -2;
	}

	if(data_buf[0] == 0 && data_buf[1] == 5)
	{
		//error_msg(data_buf[3]);
		return -1;
	}

	return 0;

	/* Todo */
}

int send_data(int socket_fd, char* buf, uint16_t block_num, int data_size)
{
	char *data_buf = NULL;
	int status = 0;

	data_buf = (char*)malloc(data_size + 4); //TODO remove hardcoding
	if(!data_buf)
	{
		perror("send_data malloc");
		return -1;
	}

	data_buf[0] = 0;
	data_buf[1] = 3; // TODO remove hardcoding
	memcpy(data_buf + 2, &block_num, sizeof(block_num));
	memcpy(data_buf + 4, buf, data_size);

	status = sendto(socket_fd, data_buf, data_size + 4, 0, (struct sockaddr *)&server, sizeof(struct sockaddr));
	if(status < 0)
	{
		perror("sendto");
		return -1;
	}

	return 0;
}

int tftp_send(char* file, int socketFd)
{
	char buf[512]; //TODO remove hardcoding
	uint16_t block_num = 0;
	int i = 0, ret = 0, num_bytes_read = 0;
	bool file_read_complete = false;
	FILE* fp;

	/* Open file which is requested to send */
	fp = fopen(file,"r");
	if(!fp) {
		perror("Couldn't open file for reading\n");
		return ERROR;
	}

	/* Create write request and send it */
	/* Start timer and wait for ack until time out if error packet get then abort transfer */
	/* If time occurs and ack is not there then retransmit write request */
	/* Get tid from ack if first run then store it otherwise compare */
	rw_req_packet(WRITE, file, NET_ASCII);
	while(i++ < RTCOUNT)
	{
		ret = ack_wait(socketFd, file);
		if (ret == -2)
			rw_req_packet(WRITE, file, NET_ASCII);
		else if(ret == -1)
		{
			fclose(fp);
			return -1;
		}
		else
			break;

	}

	/* Read data from file and create data packet with block number. send data packet */
	/* Start timer and wait for ack until timeout. no ack then retransmit packet */
	while(!file_read_complete)
	{
		num_bytes_read = fread(buf, 1, 512, fp);
		if(!num_bytes_read)
		{
			if(ferror(fp))
			{
				perror("file read");
				return -1;
			}
		}
		send_data(socketFd, buf, block_num, num_bytes_read);
		while(i++ < RTCOUNT)
		{
			ret = ack_wait(socketFd, file);
			if (ret == -2)
			{
				send_data(socketFd, buf, block_num, num_bytes_read);
			}
			else if(ret == -1)
			{
				fclose(fp);
				return -1;
			}
			else
				break;

		}

	}
}

int tftp_rcv()
{
	/* create file with given name */
	/* send first request */
	/* get tid and change port number as per tid 
	 * if it is first run then store it is some variable otherwise compare with it
	 * and it they are not equal then crate error */
	/* subsequent packet might be data/error packet
	 * start timer and wait for data packet until timeout 
	 * extract data and block number from that
	 * using block number create ack request
	 * send that ack and write data into file
	 * if data packet is less then 512 then it means it is last packet */
}

void run_tftp(int s_fd)
{
	/* if require, as per the request fill respective structure */
#if 0
	if(/* receive */)
		tftp_rcv(/* ptr of receive struct */);
	else if(/* send */ )
		tftp_send(/* ptr of send struct */ );
#endif
}

int creat_socket(char* addr, int port)
{
	/* create socket */
	/* bind with server port*/
	/* on success return socket fd */
}

int valid_ip_address(char* addr)
{
	/* proper ip addr is there or not */
	/* its in same subnet in which we are */
	/* return -ev if something goes wrong otherwise 0 */
}

void print_usage(void)
{
	printf("################### TFTP client ##############################################\n");
	printf("Usage: ./tftpClient [OPTION]\n");
	printf("  -i, --ip	IP ADDDRESS	TFTP server ip address\n");
	printf("  -r, --read	FILE NAME	Name of file to be fetch from server\n");
	printf("  -w, --write	FILE NAME	Name of file to be send to server\n\n");
	printf("Examples:\n");
	printf("1. ./tftpClient -i 192.168.2.62 -r readme.txt\n");
	printf("\tReads 'readme.txt' from TFTP server running on 192.168.2.62\n");
	printf("2. ./tftpClient --ip 192.168.2.240 --write make_in_india.txt\n");
	printf("\tWrites file 'make_in_india.txt' to server running on 192.168.2.240\n");
	printf("##############################################################################\n");
}

int validate_ip_address(char *ip_addr_str)
{
	char ip_address_delim[2] = ".";
	char *token;
	int ip_address_byte = 0;

	token = strtok(ip_addr_str, ip_address_delim);
	while(token)
	{
		if(atoi(token) > 255)
		{
			printf("ERROR: Invalid ip address string. Exiting...\n");
			return -1;
		}
		server_ip_address[ip_address_byte++] = atoi(token);
		//printf("ip byte: %d\n", server_ip_address[ip_address_byte - 1]);
		token = strtok(NULL, ip_address_delim);
	}
	return 0;
}

int parse_args(int argc, char** argv)
{
	int c = 0, opt_index = 0;
	struct option opts[] = {
		{"ip", required_argument, 0, 'i'},
		{"put", required_argument, 0, 'r'},
		{"get", required_argument, 0, 'w'},
	};
	while(1)
	{
		c = getopt_long(argc, argv, "i:r:w:", opts, &opt_index);
		if(c == -1)
			break;
		switch(c)
		{
			case 'i':
				//printf("ip address string\n");
				if(validate_ip_address(optarg) < 0)
					return -1;

				if (!(host_info = gethostbyname (optarg)))
				{   
					perror ("Client could not get host address information");
					exit (2);
				}

				break;
			case 'r':
				//printf("read request\n");
				is_read_request = true;
				strcpy(read_file_name, optarg);
				//printf("file name: %s\n", read_file_name);
				break;
			case 'w':
				//printf("write request\n");
				is_write_request = true;
				strcpy(write_file_name, optarg);
				//printf("file name: %s\n", write_file_name);
				break;
			case '?':
				print_usage();
				break;
			default:
				printf("ERROR: Invalid arguments. Exiting...\n");
				break;
		}
		return 0;
	}
}
int main(int argc, char** argv)
{
	struct sockaddr_in server;
	int socketfd;
	/* argument check */
	parse_args(argc, argv);

	/* create socket and bind with server */
	socketfd = createSocket ();

	run_tftp(socketfd);

	return 0;
}

