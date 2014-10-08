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
#define TIMEOUT			5   /* time out in seconds */
bool is_read_request = false, is_write_request = false;
char read_file_name[MAX_FILE_NAME_LEN], write_file_name[MAX_FILE_NAME_LEN];

char data_buf[MAX_DATA_SIZE];

int rw_req_packet(int socketFd, unsigned char req_type, char *filename, char *type)
{
	int len, status = 0;
	len = sprintf (buf, "%c%c%s%c%s%c", 0x00, req_type, filename, 0x00, type, 0x00);
	if (len == 0)
	{   
		printf ("Error in creating the request packet\n");
		return -1;
	}   
	//TODO modularize
	status = sendto(socketFd, buf, 4 + strlen(filename) + strlen(type), 0, (struct sockaddr*)&server, len); //TODO 512 should not be sent
	if(status < 0)
	{
		perror("<sendData> sendto");
		return -1;
	}

	return len;
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
	int status = 0, len = 0;

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

	printf("socket_fd: %d\n", socket_fd);
	printf("data_size: %d\n", data_size);
	len = sizeof(server);
	status = sendto(socket_fd, data_buf, data_size + 4, 0, (struct sockaddr*)&server, len);
	if(status < 0)
	{
		perror("<sendData> sendto");
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
	int len, server_len, opcode, i, j, n, tid = 0, flag = 1;
	unsigned short int count = 0, rcount = 0;
	unsigned char filebuf[DATA_SIZE + 1];
	unsigned char packetbuf[DATA_SIZE + 12];
	extern int errno;
	char filename[128], mode[12], *bufindex, ackbuf[512];
	struct sockaddr_in data;
	FILE *fp;			/* pointer to the file we will be getting */

//	strcpy (filename, pFilename);	//copy the pointer to the filename into a real array
//	strcpy (mode, pMode);		//same as above

	strcpy (mode, "netascii");
	fp = fopen (read_file_name, "w");	/* open the file for writing */
	if (fp == NULL)
	{				//if the pointer is null then the file can't be opened - Bad perms 
		printf ("Client requested bad file: cannot open for writing (%s)\n",
					filename);
		return;
	}
	
	printf ("Getting file... (destination: %s) \n", filename);
	rw_req_packet(socketFd, READ, read_file_name, "netascii");
	/* zero the buffer before we begin */
	memset (filebuf, 0, sizeof (filebuf));
	n = DATA_SIZE + 4;
	do
	{
		/* zero buffers so if there are any errors only NULLs will be exposed */
		memset (packetbuf, 0, sizeof (packetbuf));
		memset (ackbuf, 0, sizeof (ackbuf));
		if (n != (DATA_SIZE + 4))	/* remember if our DATA_SIZE is less than a full packet this was the last packet to be received */
		{
			printf("Last chunk detected (file chunk size: %d). exiting while loop\n",n - 4);
			len = sprintf (ackbuf, "%c%c%c%c", 0x00, 0x04, 0x00, 0x00);
			ackbuf[2] = (count & 0xFF00) >> 8;	//fill in the count (top number first)
			ackbuf[3] = (count & 0x00FF);	//fill in the lower part of the count
			printf ("Sending ack # %04d (length: %d)\n", count, len);
			if (sendto(socketFd, ackbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)
			{
				printf ("Mismatch in number of sent bytes\n");
				return;
			}
			printf ("The Client has sent an ACK for packet\n");
			goto done;		/* gotos are not optimal, but a good solution when exiting a multi-layer loop */
		}

		count++;

		for (j = 0; j < RETRIES; j++)	/* this allows us to loop until we either break out by getting the correct ack OR time out because we've looped more than RETRIES times */
		{
			server_len = sizeof (data);
			errno = EAGAIN;	/* this allows us to enter the loop */
			n = -1;
			for (i = 0; errno == EAGAIN && i <= TIMEOUT && n < 0; i++)	/* this for loop will just keep checking the non-blocking socket until timeout */
			{

				n = recvfrom (socketFd, packetbuf, sizeof (packetbuf) - 1,
							MSG_DONTWAIT, (struct sockaddr *) &data,
							(socklen_t *) & server_len);
				usleep (1000);
			}
			/*if(debug)
			 * 	     ip_port (data);  print the vlaue recived from the server */
			if (!tid)
			{
				tid = ntohs (data.sin_port);	//get the tid of the server.
				server.sin_port = htons (tid);	//set the tid for rest of the transfer 
			}

			if (n < 0 && errno != EAGAIN)	/* this will be true when there is an error that isn't the WOULD BLOCK error */
			{
				printf("The server could not receive from the client (errno: %d n: %d)\n",errno, n);
			}
			else if (n < 0 && errno == EAGAIN)	/* this is true when the error IS would block. This means we timed out */
			{
				printf ("Timeout waiting for data (errno: %d == %d n: %d)\n",
							errno, EAGAIN, n);
			}
			else
			{
				if (server.sin_addr.s_addr != data.sin_addr.s_addr)	/* checks to ensure get from ip is same from ACK IP */
				{
					printf("Error recieving file (data from invalid address)\n");
					j--;
					continue;	/* we aren't going to let another connection spoil our first connection */
				}
				if (tid != ntohs (server.sin_port))	/* checks to ensure get from the correct TID */
				{
					printf ("Error recieving file (data from invalid tid)\n");
					len = sprintf ((char *) packetbuf, "%c%c%c%cBad/Unknown TID%c", 0x00, 0x05, 0x00, 0x05, 0x00);
					if (sendto (socketFd, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)
					{
						printf("Mismatch in number of sent bytes while trying to send mode error packet\n");
					}
					j--;
					continue;	/* we aren't going to let another connection spoil our first connection */
				}
				/* this formatting code is just like the code in the main function */
				bufindex = (char *) packetbuf;	/*start our pointer going*/
				if (bufindex++[0] != 0x00)
					printf ("bad first nullbyte!\n");
				opcode = *bufindex++;
				rcount = *bufindex++ << 8;
				rcount &= 0xff00;
				rcount += (*bufindex++ & 0x00ff);
				memcpy ((char *) filebuf, bufindex, n - 4);	/* copy the rest of the packet (data portion) into our data array */
				printf("Remote host sent data packet #%d (Opcode: %d packetsize: %d filechunksize: %d)\n",
						 rcount, opcode, n, n - 4);
#if 0
				if (flag)
				{
					if (n > 516)
						DATA_SIZE = n - 4;
					else if (n < 516)
						DATA_SIZE = 512;
					flag = 0;
				}
#endif
				if (opcode != 3)	/* ack packet should have code 3 (data) and should be ack+1 the packet we just sent */
				{
					printf("Badly ordered/invalid data packet (Got OP: %d Block: %d) (Wanted Op: 3 Block: %d)\n",
							 opcode, rcount, count);
					if (opcode > 5)
					{
						len = sprintf ((char *) packetbuf,
								"%c%c%c%cIllegal operation%c",
								0x00, 0x05, 0x00, 0x04, 0x00);
						if (sendto (socketFd, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
						{
							printf("Mismatch in number of sent bytes while trying to send mode error packet\n");
						}
					}
				}
				else
				{
					len = sprintf (ackbuf, "%c%c%c%c", 0x00, 0x04, 0x00, 0x00);
					ackbuf[2] = (count & 0xFF00) >> 8;	//fill in the count (top number first)
					ackbuf[3] = (count & 0x00FF);	//fill in the lower part of the count
					printf ("Sending ack # %04d (length: %d)\n", count, len);
					//if (((count - 1) % ackfreq) == 0)
					if (count -1 == 0)
					{
						if (sendto(socketFd, ackbuf, len, 0, (struct sockaddr *) &server,
								 sizeof (server)) != len)
						{
							printf ("Mismatch in number of sent bytes\n");
							return;
						}
						printf ("The client has sent an ACK for packet %d\n",
									count);
					}		/*check for ackfreq*/
					else if (count == 1)
					{
						if (sendto(socketFd, ackbuf, len, 0, (struct sockaddr *) &server,
								 sizeof (server)) != len)
						{
							printf ("Mismatch in number of sent bytes\n");
							return;
						}
						printf ("The Client has sent an ACK for packet 1\n");
					}
					break;
				}		//end of else
			}
		}
		if (j == RETRIES)
		{
			printf ("Data recieve Timeout. Aborting transfer\n");
			fclose (fp);
			return;
		}
	}
	while (fwrite (filebuf, 1, n - 4, fp) == n - 4);	/* if it doesn't write the file the length of the packet received less 4 then it didn't work */
	fclose (fp);
	sync ();
	printf("fclose and sync unsuccessful. File failed to recieve properly\n");
	return;

done:

	fclose (fp);
	sync ();
	printf ("fclose and sync successful. File received successfully\n");
	return;


#if 0
	tftp_read();
#endif


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

	memcpy(server_ip_address, ip_addr_str, IP_ADDRESS_LEN);
	token = strtok(ip_addr_str, ip_address_delim);
	while(token)
	{
		if(atoi(token) > 255)
		{
			printf("ERROR: Invalid ip address string. Exiting...\n");
			memcpy(server_ip_address, 0, IP_ADDRESS_LEN);
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
	struct sockaddr_in server;
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

