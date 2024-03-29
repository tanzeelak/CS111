#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mraa/aio.h>
#include <math.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>
#include <poll.h>
#include <errno.h>

float tempC,tempF;
int tempType=0;
FILE* lfd;
int perNum=1;
time_t timer;
char timeBuffer[9];
struct tm* timeInfo; 
mraa_aio_context tempSensor;
mraa_gpio_context btn;
struct pollfd fd [1];
const int B = 4275;
int rawTemperature;
int btnVal = 0; 
int stopFlag = 0;
int shutdownFlag = 0;
int logFlag = 0;
time_t curr;
time_t prev = 0;

int sockfd = 0;
int tcpPort = 18000;
//struct sock_addr_in serveradd;
struct hostent *server;
char* hostname = "lever.cs.ucla.edu";


void sysFailed(char* sysCall, int exitNum)
{
	fprintf(stderr, "%s failed: %s\n", sysCall, strerror(errno));
	exit(exitNum);
}

void* tempPrint()
{
	rawTemperature = mraa_aio_read(tempSensor);
	double R = 1023.0/((double)rawTemperature) - 1.0;
	R = 100000.0*R;
	tempC  = 1.0/(log(R/100000.0)/B + 1/298.15) - 273.15;
	tempF = tempC * 9/5 + 32;

	time(&timer);
	timeInfo = localtime(&timer);
	strftime(timeBuffer, 9, "%H:%M:%S", timeInfo);
	
	if (tempType == 0)
	{
		fprintf(stdout,"%s %.1f\n", timeBuffer, tempF);
		if (logFlag)
			fprintf(lfd, "%s %.1f\n", timeBuffer, tempF);
	}
	else 
	{
		fprintf(stdout,"%s %.1f\n", timeBuffer, tempC);
		if (logFlag)
			fprintf(lfd, "%s %.1f\n", timeBuffer, tempC);
	}
	
	if(logFlag)
		fflush(lfd);

	btnVal = mraa_gpio_read(btn);
	if (btnVal == -1)
	{
		fprintf(stderr, "rip \n");
		exit(2);
	}
	else if (btnVal == 1)
	{
		shutdownFlag = 1;
		fprintf(stdout, "SHUTDOWN\n");
		if (logFlag == 1)
		{
			fprintf(lfd, "SHUTDOWN\n");
			fflush(lfd);
		}
		exit(0);
	}

	if(shutdownFlag==1)
		exit(0);
}

int main(int argc, char** argv)
{
	

	int optParse = 0;
	int scaleFlag = 0;
       	int perFlag = 0;
	int idFlag = 0;
	int hostFlag = 0;

	int idNum = 0;
	int hostNum = 0; 

	char* logopt = NULL;
	char* scaleopt = NULL;
	char* peropt = NULL;
	char* idopt = NULL;
	char* hostopt = NULL;

	struct sockaddr_in serv_addr;
	btn = mraa_gpio_init(3);
	tempSensor = mraa_aio_init(0);
	mraa_gpio_dir(btn, MRAA_GPIO_IN);
	
	static const struct option long_options[] =
	{
        	{"log",required_argument,0,'l',},
        	{"scale",required_argument,0,'s'},
       		{"period",required_argument,0,'p'},
		{"port",no_argument,0,'o'},
		{"host",required_argument,0,'h'},
		{0, 0, 0, 0}
	};


	int option_index = 0;
	while ((optParse = getopt_long(argc, argv, "", long_options, &option_index)) != -1) 
	{
    	switch (optParse) {
        	case 'l':
        		logFlag = 1;
			logopt = optarg;
            		break;
        	case 's': 
			scaleFlag = 1;
			scaleopt = optarg;
	           	 break;
	        case 'p':
			perFlag = 1;
		       	peropt = optarg;	
	        	break;
		case 'h':
			hostFlag = 1;
			hostopt = optarg;
			break;
		case 'o':
			logFlag = 1;
			logopt = optarg;
			break;
        	default:
        		fprintf(stderr, "Proper usage of options: --log=logfile --scale=C/F --period=# \n");
	         	exit(1);
	    }
	}

	if (logFlag)
	{
        	lfd = fopen(logopt, "w");
	}
	if (scaleFlag)
	{
		if (scaleopt[0] == 'C')
			tempType = 1;
	}
	if (perFlag)
	{
		perNum = atoi(peropt);
	}
	if (idFlag)
	{
		idNum = atoi(idopt);	
	}
	if (hostFlag)
	{
		hostNum = atoi(hostopt);
	}



	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {sysFailed("sockfd", 1);}

	server = gethostbyname(hostname);
	if (server == NULL){sysFailed("server",1);}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	memmove((char *) &serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
	  perror("ERROR connecting");
	  exit(1);
	}
	

	fd[0].fd = STDIN_FILENO;
	fd[0].events = POLLIN | POLLHUP | POLLERR;
	while(1)
  	{
		curr = time(NULL);	
		int val = poll(fd, 1, 0);

		if (fd[0].revents & POLLIN) 
		{	
  			char commBuff[1024];
			char c;
			int buffIndex = 0;
	  		while (1)
			{
				if(read(0, &c, 1)>0)
  				{
					if (c == '\n')
					{
						commBuff[buffIndex] = '\0';
						buffIndex = 0; 
						break;
					}
					commBuff[buffIndex] = c;
					buffIndex++;	
				}
			}
  			if(strcmp(commBuff,"OFF") == 0)
  			{
  				stopFlag = 1;
  				shutdownFlag = 1;
  				if(logFlag == 1)
  				{
  					fprintf(lfd,"OFF\nSHUTDOWN\n");
  					fflush(lfd);
  					fclose(lfd);
  				}
  				fprintf(stdout,"SHUTDOWN\n");
  				break;
  			}
  			else if(strcmp(commBuff, "STOP") == 0)
  			{
	  			if(logFlag == 1)
  				{
  					fprintf(lfd,"STOP\n");
  					fflush(lfd);
  				}
	  			if(stopFlag == 0)
  				{
  					stopFlag = 1;
  				}
  			}	
  			else if(strcmp(commBuff, "START")==0)
  			{
  				if(logFlag == 1)
  				{
  					fprintf(lfd, "START\n");
  					fflush(lfd);
  				}
  				if(stopFlag == 1)
  				{
  					stopFlag = 0;
  				}

			}
  			else if(strcmp(commBuff,"SCALE=F") == 0)
  			{
  				fprintf(stdout,"SCALE=F\n");
  				if(logFlag == 1)
  				{
  					fprintf(lfd, "SCALE=F\n");
  					fflush(lfd);
  				}
  				tempType = 0;

			}
 			else if(strcmp(commBuff,"SCALE=C")==0)
  			{
  				fprintf(stdout,"SCALE=C\n");
  				if(logFlag==1)
  				{
  					fprintf(lfd, "SCALE=C\n");
  					fflush(lfd);
  				}
  				tempType=1;
  			}
			else if (strncmp(commBuff, "PERIOD=", 7) == 0) 
			{
				int j = atoi(commBuff+7);
				if (j>0)
					perNum = j;
				if (logFlag == 1)
				{
					fprintf(lfd, "PERIOD=%i", perNum);
				}
			}
	
		}
		
  		if (stopFlag == 0 && (curr-prev >= perNum))
		{	
			tempPrint();
			prev = curr;
		}	
	}

 	exit(0);
}
