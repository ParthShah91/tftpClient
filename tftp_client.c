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
#define TIMEOUT			500   /* time out in seconds */
bool is_read_request = false, is_write_request = false;
char read_file_name[MAX_FILE_NAME_LEN], write_file_name[MAX_FILE_NAME_LEN];

char data_buf[MAX_DATA_SIZE];

int rw_req_packet(int socketFd, unsigned char req_type, char *filename, char *type)
{
	int len;

	len = sprintf (buf, "%c%c%s%c%s%c", 0x00, req_type, filename, 0x00, type, 0x00);
	if (len == 0)
	{   
		printf ("Error in creating the request packet\n");
		return -1;
	}   
	//TODO modularize
	if(sendto(socketFd, buf, len, 0, (struct sockaddr *)&server, sizeof(server)) != len);
	//TODO 512 should not be sent
	{
		perror("proper sending is not happen\n");
		perror("<sendData> sendto");
		return -1;
	}

	return len;
}

int ack_packet(int socketFd, uint16_t block, char *ackbuf)
{
	int len;

	len = sprintf (ackbuf, "%c%c%c%c", 0x00, 0x04, 0x00, 0x00);
	ackbuf[2] = (block & 0xFF00) >> 8;
	ackbuf[3] = (block & 0x00FF);
	if (sendto(socketFd, buf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)
	{
		printf ("Mismatch in number of sent bytes\n");
		return -1;
	}

	return 0;
}

int error_packet(int socketFd, uint16_t error_code, char *errmsg)
{
	int len;

	memset (buf, 0, sizeof (buf)); 
	len = sprintf (buf, "%c%c%c%c%s%c", 0x00, ERR, 0x00, error_code, errmsg, 0x00);
	if (len == 0)
	{
		printf ("Error in creating the ACK packet\n");    /*could not print to the client buffer */
		return -1;
	}
	if (sendto (socketFd, buf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)
	{
		printf("Mismatch in number of sent bytes while trying to send mode error packet\n");
		return -1;
	}
	return 0;
}

int ack_wait(int sfd, char* file)
{
	fd_set rfds;
	int retval;
	struct timeval timeout;
	int i, ret, server_len, n, tid;
	struct sockaddr_in data;

	printf("Inside ack_wait\n");
	server_len = sizeof(data);
	errno = EAGAIN;
	n = -1;
	for (i = 0; errno == EAGAIN && i <= 1000 && n < 0; i++)
	{
		n = recvfrom (sfd, data_buf, 4,
				MSG_DONTWAIT, (struct sockaddr *) &data,
				(socklen_t *) & server_len);
		//printf("n: %d\n", n);
		usleep(1000);
	}

	if (!tid)
	{
		tid = ntohs (data.sin_port);
		server.sin_port = htons (tid);
	}

	printf("data_buf[0]: %d\n", data_buf[0]);
	printf("data_buf[1]: %d\n", data_buf[1]);
	printf("data_buf[2]: %d\n", data_buf[2]);
	printf("data_buf[3]: %d\n", data_buf[3]);
	if(data_buf[0] == 0 && data_buf[1] == 4)
	{
		//error_msg(data_buf[3]);
		return 0;
	}

	return -1;

	/* Todo */
}

int send_data(int socket_fd, char* buf, uint16_t block_num, int data_size)
{
	//char *data_buf = NULL;
	char data_buf[550];
	int status = 0, len = 0;

	/*data_buf = (char*)malloc(data_size + 4); //TODO remove hardcoding
	if(!data_buf)
	{
		perror("send_data malloc");
		return -1;
	}
	
	memset(data_buf, 0 , data_size + 4);
	*/
	memset(data_buf, 0 , 550);
	
	//data_buf[0] = 0;
	//data_buf[1] = 3; // TODO remove hardcoding
	//memcpy(data_buf + 2, &block_num, sizeof(block_num));
	len = sprintf (data_buf, "%c%c%c%c%s", 0x00, 0x03, 0x00, 0x00, buf);
        if (len == 0)
        {
                printf ("Error in creating the ACK packet\n");    /*could not print to the client buffer */
                return -1;
        }
	data_buf[2] = (block_num & 0xFF00) >> 8;
	data_buf[3] = (block_num & 0x00FF);

	//memcpy(data_buf + 4, buf, data_size);

	printf("socket_fd: %d\n", socket_fd);
	printf("data_size: %d\n", data_size);
	len = sizeof(server);
	status = sendto(socket_fd, data_buf, data_size + 4, 0, (struct sockaddr*)&server, len);
	//data_buf = NULL;
	if(status < 0)
	{
		perror("<sendData> sendto");
		//free(data_buf);
		return -1;
	}
	else if( status < 512)
	{
		return 1;
	}

	//free(data_buf);
	return 0;
}

int tftp_send(char* file, int socketFd)
{
	char buf[512]; //TODO remove hardcoding
	uint16_t block_num = 0;
	int i = 0, ret = 0, num_bytes_read = 0, status;
	bool file_read_complete = false;
	FILE* fp;

	printf("tftp_send\n");
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
	rw_req_packet(socketFd, WRITE, file, "netascii");
	while(i++ < RTCOUNT)
	{
		ret = ack_wait(socketFd, file);
		if (ret == -2) {
			rw_req_packet(socketFd, WRITE, file, "netascii");
		}
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
		printf("++ blockNum: %d\n", block_num);
		status = send_data(socketFd, buf, block_num, num_bytes_read);
		if(status < 0)
		{
			fclose(fp);
			break;	
		}
		else if (status > 0)
		{
			//TODO ack_wait(socketFd, file);
			ret = ack_wait(socketFd, file);
			fclose(fp);
			printf("sent successfully");
			break;	
			
		}
		i = 0;
		while(i++ < RTCOUNT)
		{
			ret = ack_wait(socketFd, file);
			if (ret == -1)
			{
				printf("Sending data again for block_num %d..\n", block_num);
				send_data(socketFd, buf, block_num, num_bytes_read);
			}
			/*else if(ret == -1) //TODO
			{
				fclose(fp);
				return -1;
			}*/
			else
			{
				//fclose(fp);
				break;
			}

		}
		block_num++;
		printf("\n\n");
		//file_read_complete = true; //TODO update this condition
	}
	return 0;
}

int tftp_rcv(char* file, int socketFd)
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

	/* local variables */
	int server_len, opcode, tid = 0;
	int i, j, n;
	unsigned short int count = 0, rcount = 0;
	unsigned char filebuf[DATA_SIZE + 1];
	unsigned char packetbuf[DATA_SIZE + 12];
	extern int errno;
	char filename[128], mode[12];
	char *bufptr;
	struct sockaddr_in data;
	FILE *fp;			/* pointer to the file we will be getting */

	strcpy (mode, "netascii");
	fp = fopen (read_file_name, "w");	/* open the file for writing */
	if (fp == NULL)
	{				//if the pointer is null then the file can't be opened - Bad perms 
		printf ("Client requested bad file: cannot open for writing (%s)\n",
					filename);
		return -1;
	}
	
	printf ("Getting file... (destination: %s) \n", filename);
	rw_req_packet(socketFd, READ, read_file_name, "netascii");
	
	memset (filebuf, 0, sizeof (filebuf));
	n = DATA_SIZE + 4;
	
	do
	{
		memset (packetbuf, 0, sizeof (packetbuf));
		
		if (n != (DATA_SIZE + 4))
		{
			printf("Last chunk detected (file chunk size: %d). exiting while loop\n",n - 4);
			printf("Last packet recieved");
			ack_packet(socketFd, count, buf);
			//fclose (fp);
			printf ("The Client has sent an ACK for packet\n");
			goto done;
		}

		count++;

		for (j = 0; j < RTCOUNT; j++)
		{
			server_len = sizeof (data);
			errno = EAGAIN;
			n = -1;
			for (i = 0; errno == EAGAIN && i <= TIMEOUT && n < 0; i++)
			{
				n = recvfrom (socketFd, packetbuf, sizeof (packetbuf) - 1,
							MSG_DONTWAIT, (struct sockaddr *) &data,
							(socklen_t *) & server_len);
				usleep (1000);
			}

			if (!tid)
			{
				tid = ntohs (data.sin_port);
				server.sin_port = htons (tid);
			}

			if (n < 0 && errno == EAGAIN)	/* If timeout occurs */
			{
				printf("Client packet got lost");
				printf ("Timeout waiting for data");
			}
			else
			{
				
				bufptr = (char *) packetbuf;	/*start our pointer going*/
				if (bufptr++[0] != 0x00)
					printf ("bad first nullbyte!\n");
				opcode = *bufptr++;
				rcount = *bufptr++ << 8;
				rcount &= 0xff00;
				rcount += (*bufptr++ & 0x00ff);
				memcpy ((char *) filebuf, bufptr, n - 4);
				if (opcode != 3)
				{
					printf("Badly ordered/invalid data packet (Got OP: %d Block: %d) (Wanted Op: 3 Block: %d)\n",
							 opcode, rcount, count);
					if (opcode > 5)
					{
						error_code = 0x04;
						//error_packet(socketFd, error_code, err_msg[error_code]);
					}
				}
				else
				{
					ack_packet(socketFd, count, buf);
					break;
				}		//end of else
			}
		}
		if (j == RTCOUNT)
		{
			printf ("Data recieve Timeout. Aborting transfer\n");
			fclose (fp);
			return -1;
		}
	}
	while (fwrite (filebuf, 1, n - 4, fp) == n - 4);

done:

	fclose (fp);
	sync ();
	printf ("fclose and sync successful. File received successfully\n");
	return 0;

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
	return 0;
}

int valid_ip_address(char* addr)
{
	/* proper ip addr is there or not */
	/* its in same subnet in which we are */
	/* return -ev if something goes wrong otherwise 0 */
	return 0;
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

	memcpy(server_ip_address, ip_addr_str, IP_ADDRESS_LEN);
	token = strtok(ip_addr_str, ip_address_delim);
	while(token)
	{
		if(atoi(token) > 255)
		{
			printf("ERROR: Invalid ip address string. Exiting...\n");
			memset(server_ip_address, 0, IP_ADDRESS_LEN);
			return -1;
		}
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
	printf("parse_args\n");
	while(1)
	{
		c = getopt_long(argc, argv, "i:r:w:", opts, &opt_index);
		if(c == -1)
			break;
		printf("c: %c\n", c);
		switch(c)
		{
			case 'i':
				printf("ip address string\n");
				if(validate_ip_address(optarg) < 0)
					return -1;

				if (!(host_info = gethostbyname (optarg)))
				{   
					perror ("Client could not get host address information");
					exit (2);
				}
				printf("All good for ip address string\n");
				break;
			case 'r':
				printf("read request\n");
				is_read_request = true;
				strcpy(read_file_name, optarg);
				printf("file name: %s\n", read_file_name);
				break;
			case 'w':
				printf("write request\n");
				is_write_request = true;
				strcpy(write_file_name, optarg);
				printf("file name: %s\n", write_file_name);
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
	int socketfd;
	/* argument check */
	parse_args(argc, argv);

	/* create socket and bind with server */
	socketfd = createSocket ();

	//run_tftp(socketfd);
	printf("is_write_request: %d\n", is_write_request);
	if(is_write_request)
	{
		tftp_send(write_file_name, socketfd);
	}
	else
	{
		tftp_rcv(read_file_name, socketfd);
	}

	return 0;
}

