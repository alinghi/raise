#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

//Preprocessor for IPaddress
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MAX_BUF   (1024)

int sockfd;

//Network Connection
//to port 2323
//return fd
int connect2323(uint32_t ipAddress){
	struct sockaddr_in servaddr;
	uint32_t actual_addr;
	struct sockaddr_in temp;
	int count=0;
	char* ipAddr;

	//socket
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("socket error\n");
		return 0;
	}

	//init server address
	memset( &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(2323);
	actual_addr=htonl(ipAddress);
	temp.sin_addr.s_addr=actual_addr;
	ipAddr=inet_ntoa(temp.sin_addr);
	//connect
	printf("ip : %s \n",ipAddr);
	if(inet_pton(AF_INET,ipAddr,&servaddr.sin_addr)<=0)
		printf("inet_pton error\n");
	if(connect(sockfd,(struct sockaddr*) &servaddr, sizeof(servaddr))<0)
		printf("connect error\n");
	return 1;
}

//Actual Worm
int worm(uint32_t ipAddress){
	int count=0;
	int r;
	char buf[MAX_BUF];
	char input[MAX_BUF];
	//Make socket file descriptor
	if(!connect2323(ipAddress))
		//In case of fail
		return 0;
	//write
	//write
	while(count<2){
		r = readSock( sockfd, buf, MAX_BUF );
		if ( r == -1 ) return -1;
		printf("%s",buf);
   		fgets(input,MAX_BUF,stdin);
   		r = send( sockfd, input, strlen( input ), 0 );
    		if ( r == -1 ) { perror( "send" ); return -1; }
    		count++;
	}
	while(1){
   		fgets(input,MAX_BUF,stdin);
   		if(input[0]=='\n')
   		{
   			printf("new\n");
   			continue;
   		}
   		r = send( sockfd, input, strlen( input ), 0 );
    		if ( r == -1 ) { perror( "send" ); return -1; }
    		printf("get the command\n");

		r = readSock( sockfd, buf, MAX_BUF );
		if ( r == -1 ) return -1;
		printf("%s\n",buf);		
	}
	//read
	//close
	close(sockfd);
}

int readSock( int sockfd, char* buf, size_t bufLen )
{
    int r;

    r = recv( sockfd, buf, bufLen - 1, 0 );
    if ( r == -1 ) { perror( "recv" ); return -1; }
    buf[r - 1] = '\0';
    return 0;
}


//In case of usageError
int usageError(){
	printf("Usage : ./myworm ip_begin ip_end file_name[optional]\n");
	exit(1);
}

//In case of IP input Error
int inputError(){
	printf("IP address is invalid or range is fault\n");
	exit(1);
}

//Check out valid IP address
//From : http://stackoverflow.com/questions/791982/determine-if-a-string-is-a-valid-ip-address-in-c
bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

//return uint32 type address from string
uint32_t returnIPAddr(char *ipAddress)
{
    struct sockaddr_in sa;
    uint32_t return_addr;
    inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return_addr=sa.sin_addr.s_addr;

    //Debug Purpose printf
    //printf("%u %u\n",sa.sin_addr,return_addr);

    //Since I use intel CPU endian conversion
    return_addr=ntohl(return_addr);

    //Debug Purpose printf
    //printf("ntohl : %u %u\n",sa.sin_addr,return_addr);

    return return_addr;	
}

//Check the IP address range
bool isRangeOK(char *ipAddress1, char *ipAddress2){
	bool validAddr[2];
	uint32_t ipAddr1;
	uint32_t ipAddr2;

	//check if IP address is valid or not
	validAddr[0]=isValidIpAddress(ipAddress1);
	validAddr[1]=isValidIpAddress(ipAddress2);
	return validAddr[0]&&validAddr[1];
}

int main( int argc, char* argv[] )
{
	bool rangeCheck=false;
	uint32_t startAddress;
	uint32_t endAddress;

	//check the input
	if(argc!=3 && argc!=4){
		usageError();
	}

	//check the range of IP input and get uint32 type address
	rangeCheck=isRangeOK(argv[1],argv[2]);
	if(rangeCheck)
	{
		startAddress=returnIPAddr(argv[1]);
		endAddress=returnIPAddr(argv[2]);
	}
	//When input IP address itself is invalid
	else
		inputError();

	//When range of IP address is wrong
	if(startAddress>endAddress)
		inputError();

	worm(returnIPAddr("127.0.0.1"));

	return 0;
}

