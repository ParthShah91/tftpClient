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
	/* open file which is requested to send */
	/* if such file is not exist then send error packet with error number */
	/* create write request and send it */
	/* start timer and wait for ack until time out if error packet get then abort transfer */
	/* if time occurs and ack is not there then retransmit write request */
	/* get tid from ack if first run then store it otherwise compare */
	/* read data from file and create data packet with block number. send data packet */
	/* start timer and wait for ack until timeout. no ack then retransmit packet */

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

	if(/* receive */)
		tftp_rcv(/* ptr of receive struct */);
	else if(/* send */ )
		tftp_send(/* ptr of send struct */ );

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

}


int main(int argc, char** argv)
{
	/* argument check */
	print_usage();


	while(getopt()) {
		/* Note :- sequence of switch may get changed */
		switch() {

			case /* read or write request */
				/* if write request then on one flag */
				break;
			case /* get file  */
				/* if request is to get file from server and local 
				 * name is given then store it on different variable */
				break;
			case /* extract server ip address */
				if(validate_ip_address() < 0)
					/* error with suitable error message */

		}
	}

	/* create socket and bind with server */
	sockfd = creat_socket(char* addr,int port);

	run_tftp(socketfd);

}

