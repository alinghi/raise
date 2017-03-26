#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

//Preprocessor for IPaddress
#include <arpa/inet.h>


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

	return 0;
}

