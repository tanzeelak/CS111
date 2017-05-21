#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <mraa/aio.h>
#include <math.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>
#include <poll.h>

float tempC,tempF;

int tempType=0, stopFlag=0, shutdownFlag=0, logFlag=0;
FILE* lfd;
int per=1;

time_t timer;
char timeBuffer[9];
struct tm* timeInfo; 
mraa_aio_context tempSensor;
mraa_gpio_context btn;
struct pollfd fd [1];
const int B = 4275;
int rawTemperature;
int btnVal = 0; 


void* printer()
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

		sleep(per);

		if(shutdownFlag==1)
			exit(0);


}

int main(int argc, char** argv)
{
	
	int optParse=0, scale_flag=0;
	btn = mraa_gpio_init(3);
	tempSensor = mraa_aio_init(0);
	mraa_gpio_dir(btn, MRAA_GPIO_IN);
	static const struct option long_options[] =
	{
        	{"log",required_argument,NULL,'l',},
        	{"scale",required_argument,NULL,'s'},
       		{"period",required_argument,NULL,'p'},
        	{0, 0, 0, 0} // end of array
	};



	while (-1 != (optParse = getopt_long(argc, argv, "", long_options, NULL))) 
	{
    	switch (optParse) {
        	case 'l':
        		logFlag=1;
        		lfd = fopen(optarg, "w");
            	break;
        	case 's': 
        		if(optarg)
	            break;
	        case 'p':
	        	per = atoi(optarg);
	        	break;
        	default:
        		fprintf(stderr, "INVALID OPTION(S)\nCorrect Usage: --log=\"logfile\" --scale=C --period=500\nTerminating...\n");
	         	exit(1);
	    }
	}
  	
	fd[0].fd = STDIN_FILENO;
	fd[0].events = POLLIN | POLLHUP | POLLERR;

	while(1)
  	{	
		int val = poll(fd, 1, 0);
		if (val > 0 ) {

		if (fd[0].revents & POLLIN) {	
  			char commandBuffer[1024];
			char c;
			int buffIndex = 0;
	  		while (1)
			{
				if(read(0, &c, 1)>0)
  				{
					if (c == '\n')
					{
						commandBuffer[buffIndex] = '\0';
						buffIndex = 0; 
						break;
					}
					commandBuffer[buffIndex] = c;
					buffIndex++;	
				}
			}
  			if(strcmp(commandBuffer,"OFF") == 0)
  			{
  				stopFlag=1;
  				shutdownFlag=1;
  				if(logFlag==1)
  				{
  					fprintf(lfd,"OFF\nSHUTDOWN\n");
  					fflush(lfd);
  					fclose(lfd);
  				}
  				fprintf(stdout,"SHUTDOWN\n");
  				break;
  			}
  				else if(strcmp(commandBuffer, "STOP")==0)
  				{
	  				if(logFlag==1)
  					{
  						fprintf(lfd,"STOP\n");
  						fflush(lfd);
  					}
	  				if(stopFlag==0)
  					{
  						stopFlag=1;
  					}
  				}	
  				else if(strcmp(commandBuffer, "START")==0)
  				{
  					if(logFlag==1)
  					{
  						fprintf(lfd, "START\n");
  						fflush(lfd);
  					}
  					if(stopFlag==1)
  					{
  						stopFlag=0;
  					}

  				}
	  			else if(strcmp(commandBuffer,"SCALE=F")==0)
  				{
  					fprintf(stdout,"SCALE=F\n");
  					if(logFlag==1)
  					{
  						fprintf(lfd, "SCALE=F\n");
  						fflush(lfd);
  					}
  					tempType=0;

  				}
  				else if(strcmp(commandBuffer,"SCALE=C")==0)
  				{
  					fprintf(stdout,"SCALE=C\n");
  					if(logFlag==1)
  					{
  						fprintf(lfd, "SCALE=C\n");
  						fflush(lfd);
  					}
  					tempType=1;
  				}
				else if (strncmp(commandBuffer, "PERIOD=", 7) == 0) {
					int j = atoi(commandBuffer+7);
					if (j>0)
						per = j;
					if (logFlag == 1)
					{
						fprintf(lfd, "PERIOD=%i", per);
					}
				}

		}
		}
  		if (stopFlag == 0)
		{
			printer();
		}	
	}

  	exit(0);
}
