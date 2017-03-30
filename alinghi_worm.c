//Preprocessor
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

//IP RANGE variable :  Little Endian form
static uint32_t start_ip;
static uint32_t end_ip;
static char* ID;
static char* PW;


//Purpose : when wrong input argument is given.
void showUsage(char *argv){
    fprintf( stderr, "Usage: %s <lower ip> <upper ip> <option:fileName>\n", argv);
    exit(0);
}

//http://stackoverflow.com/questions/791982/determine-if-a-string-is-a-valid-ip-address-in-c
//Return true if IPaddr is valid
bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(struct sockaddr_in));
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

//return IP address in little endian uint32_t
uint32_t returnIpAddress(char *ipAddress){
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(struct sockaddr_in));
    inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return ntohl(sa.sin_addr.s_addr);	
}

//check out given IP range is valid or not.
//if valid set start_IP, end_IP and return true
bool isRangeOk(char *lower,char *upper){
	start_ip=returnIpAddress(lower);
	end_ip=returnIpAddress(upper);

	//debug purpose printf
	/*
	printf("start ip %u\n",start_ip);
	printf("end ip %u\n",end_ip);


	if(start_ip<=end_ip)
		printf("range ok\n");
	else
		printf("range fault\n");
	*/

	if(start_ip>end_ip)
	{
   		fprintf( stderr,"Range Setting Error\n");
   		exit(0);
	}
	else
		return true;
}

bool isValidInput(int argc, char *argv[]){
	//Variable
		//rangeOk for lower~upper topology
	bool rangeOk=false;
		//isValidIpAddress_1 check argv[1]'s IP addr
	bool isValidIpAddress_1=false;
		//isValidIpAddress_2 check argv[2]'s IP addr
	bool isValidIpAddress_2=false;
		//result => true for valid input
	bool result=false;

	//Printf for debug purpose
	/*
	printf("argc : %d\n",argc);
	for(int i=0;i<argc;i++){
		printf("argv %d : %s\n",argc, argv[i]);
	}
	*/

	//exit when argc is not 3 or 4
	if(argc!=3 && argc!=4){
		showUsage(argv[0]);
	}

	//check out IP Address

	isValidIpAddress_1=isValidIpAddress(argv[1]);
	isValidIpAddress_2=isValidIpAddress(argv[2]);
	
	//printf("isValid1 : %d\n",isValidIpAddress_1);
	//printf("isValid2 : %d\n",isValidIpAddress_2);

	//If it is invalid IP Address print out
	if(!isValidIpAddress_1)
	{
		fprintf(stderr,"%s is invalid IP Address\n",argv[1]);
		exit(0);
	}
	if(!isValidIpAddress_2)
	{
		fprintf(stderr,"%s is invalid IP Address\n",argv[2]);
		exit(0);
	}

	rangeOk=isRangeOk(argv[1],argv[2]);

	if(rangeOk)
		return true;
	else
		return false;

}

//ip given in Big ENDIAN
bool worm(uint32_t ip){
	//test purpose worm
	if(ip%2==0){
		ID=strdup("blaze");
		PW=strdup("frost");			
		return true;
	}
	else{
		return false;
	}

//Connect to Specific IP with port number 2323

//If connected try to login with superuser

//Try to get ID and Password

//File exist check and if not try to store binary into /tmp/myworm
//Should not run in vulnet client
//And Close the Connection

//Reconnect to Server with obtained ID and Password
	//-in case of login success : log the ID and Password
	//-in case of fail : log the "superuser"

}

//iterate Worm function in IP range given by User
//IP RANGE variable :  Little Endian form
void iterateWorm(uint32_t start_ip,uint32_t end_ip){
	uint32_t i;
	uint32_t bigEndian;
	char* ptr;
	struct sockaddr_in sa;
	bool success=false;
	const char* file="./slaves.csv";
	FILE *fp=fopen(file,"w");


	//htonl -> worm
	for(i=start_ip;i<=end_ip;i++){
		bigEndian=htonl(i);
		success=worm(bigEndian);

		//in case of success record
		if(success){
			//Get string of IP Address
			memset(&sa,0,sizeof(struct sockaddr_in));
			sa.sin_addr.s_addr=bigEndian;
			ptr=inet_ntoa(sa.sin_addr);

			//write success result
			fprintf(fp,"%s,",ptr);
			fprintf(fp,"%s,",ID);
			fprintf(fp,"%s\n",PW);

		}
		
	}
	fclose(fp);
}


int main(int argc, char *argv[])
{
	bool validInput=false;
	validInput=isValidInput(argc,argv);
	if(!validInput)
		showUsage(argv[0]);
	iterateWorm(start_ip,end_ip);
	return 0;
}
