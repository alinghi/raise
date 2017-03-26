#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int usageError(){
	printf("Usage : ./myworm ip_begin ip_end file_name[optional]\n");
	exit(1);
}

int main( int argc, char* argv[] )
{
	//check the input
	if(argc!=3 && argc!=4){
		usageError();
	}


	return 0;
}
