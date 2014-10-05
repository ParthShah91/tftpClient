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

#define MAX_FILE_NAME_LEN	255
#define IP_ADDRESS_LEN		4

bool is_read_request = false, is_write_request = false;
char read_file_name[MAX_FILE_NAME_LEN], write_file_name[MAX_FILE_NAME_LEN];
unsigned char server_ip_address[IP_ADDRESS_LEN];

struct rw_hdr* read_write(int opcode, char* filename, int mode)
{

}

struct ack_hdr* ack(int block)
{

}

struct data_hdr* data(int block, char* data)
{

}

int tftp_send()
{
	/* Open file which is requested to send */
	/* If such file is not exist then send error packet with error number */
	/* Create write request and send it */
	/* Start timer and wait for ack until time out if error packet get then abort transfer */
	/* If time occurs and ack is not there then retransmit write request */
	/* Get tid from ack if first run then store it otherwise compare */
	/* Read data from file and create data packet with block number. send data packet */
	/* Start timer and wait for ack until timeout. no ack then retransmit packet */

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
		{"read", required_argument, 0, 'r'},
		{"write", required_argument, 0, 'w'},
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
	}
	return 0;
}

int main(int argc, char** argv)
{
	/* argument check */
	parse_args(argc, argv);

	/* create socket and bind with server */
	//sockfd = creat_socket(char* addr,int port);

	//run_tftp(socketfd);

	return 0;
}

