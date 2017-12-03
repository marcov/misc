#include <stdio.h>
#include <string.h>
#include <fcntl.h>

//Debug printf...
#define dfprintf(...)
//#define dfprintf(...) fprintf(__VA_ARGS__)

#define READ_CMD	"read"
#define WRITE_CMD	"write"
#define INPUT_CMD	"input"
#define OUTPUT_CMD	"output"

const char * gpiocmd [] = { READ_CMD,
						    WRITE_CMD,
						    INPUT_CMD,
						    OUTPUT_CMD};


const char * gpiodev_filename = "/dev/gpiochar";


void usage (char * appname)
{
int idx = 0;
	printf("Usage: %s  ", appname);
	for (; ;){
		printf("%s", gpiocmd[idx]);
		if (++idx < sizeof(gpiocmd) / sizeof(char *)) {
			printf(" | ");
		} else {
			break;
		}
	}
	puts(" <gpionumber> <gpiovalue>\n");
}



int main(int argc, char * argv[])
{
char * reqcmd;
int gpionum;
char gpioval = '0';
char cmdtoken;
int gpiodev_file = 0;
char rwbuff[5];

	if (argc < 3){	
		usage(argv[0]);
		// dont be scared by goto statements :)
		goto exitpoint;
	}
	
	//Check that gpio is in range 00-99
	if( ( (strlen(argv[2]) == 1) && 
		  !(argv[2][0] < '0' || argv[2][0] > '9' )
		)
		||
		( (strlen(argv[2]) == 2) && 
		  !(argv[2][0] < '0' || argv[2][0] > '9' ) &&
		  !(argv[2][1] < '0' || argv[2][1] > '9' ) ) ) {

		gpionum = atoi(argv[2]);
	} else {
		usage(argv[0]);
		goto exitpoint;
	}
	
	if (strncmp(argv[1],WRITE_CMD,strlen(WRITE_CMD) ) == 0) {
		cmdtoken = 'w';
		if (argc >= 4 && 
			(argv[3][0] == '0' || argv[3][0] == '1')) {
			gpioval = argv[3][0];
		} else {
			usage(argv[0]);
			goto exitpoint;
		}
		
	} else if (strncmp(argv[1],READ_CMD,strlen(READ_CMD) ) == 0) {
		cmdtoken = 'r';
	} else if (strncmp(argv[1],INPUT_CMD,strlen(INPUT_CMD) ) == 0) {
		cmdtoken = 'i';
	} else if (strncmp(argv[1],OUTPUT_CMD,strlen(OUTPUT_CMD) ) == 0) {
		cmdtoken = 'o';
	} else {
		usage(argv[0]);
		goto exitpoint;
	}

	if ( (gpiodev_file = open(gpiodev_filename, O_RDWR)) < 0 ) {
		perror("Cannot open gpio char device");
		goto exitpoint;
	}

	snprintf(rwbuff,sizeof(rwbuff), "%c%02d%c", cmdtoken, gpionum, gpioval); 
	dfprintf(stderr, "Sending %s\n", rwbuff);

	if ( write(gpiodev_file,rwbuff, 4) < 0 ){ 
		printf("Cannot write to gpio device.\n");
		goto exitpoint;
	}

	if ( cmdtoken == 'r') {
		if ( read(gpiodev_file, rwbuff, 1) >= 1 ) {
			printf("Value: %c\n", rwbuff[0]); 
		}
	}

exitpoint:	
	return 0;
}
