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

float temp_celc,temp_farh;

int temp_type=0, stop_flag=0, shutdown_flag=0, l_flag=0;
FILE* lfd;
int per=1;

time_t timer;
char timeBuffer[9];
struct tm* timeInfo; 
mraa_aio_context tempSensor;
mraa_gpio_context btn;

void* printer()
{
	const int B = 4275;
	tempSensor = mraa_aio_init(0);	
	int rawTemperature;

	while(1)
	{
		rawTemperature = mraa_aio_read(tempSensor);
		double R = 1023.0/((double)rawTemperature) - 1.0;
		R = 100000.0*R;
		temp_celc  = 1.0/(log(R/100000.0)/B + 1/298.15) - 273.15;
		temp_farh = temp_celc * 9/5 + 32;
	
		time(&timer);
		timeInfo = localtime(&timer);
		strftime(timeBuffer, 9, "%H:%M:%S", timeInfo);
		
		printf("%s\n", timeBuffer);
		if (l_flag)
			fprintf(lfd, "%s\n", timeBuffer);

		char tempOut[64];		

		if (temp_type == 0)
		{
		//	snprintf(tempOut, 64, "TEMP=%.1f\n", temp_celc)
			printf("%f\n", temp_farh);
			if (l_flag)
				fprintf(lfd, "%f\n", temp_farh);
		}
		else 
		{
			printf("%f\n", temp_celc);
			if (l_flag)
				fprintf(lfd, "%f\n", temp_celc);
		}
		
	

		if(l_flag)
			fflush(lfd);


		sleep(2);

		if(shutdown_flag==1)
			exit(0);
	}


}

void* btnExit()
{
	btn = mraa_gpio_init(3);
	int btnVal = 0;
	while(1)
	{
		btnVal = mraa_gpio_read(btn);
		if (btnVal == -1)
		{
			fprintf(stderr, "rip \n");
			exit(2);
		}
		else if (btnVal == 1)
		{
			shutdown_flag = 1;
			fprintf(stdout, "SHUTDOWN\n");
			if (l_flag == 1)
			{
				fprintf(lfd, "SHUTDOWN\n");
				fflush(lfd);
			}
			break;
		}
	}
}

int main(int argc, char** argv)
{
	int opt=0,per=1, scale_flag=0;

	static const struct option long_opts[] =
	{
        	{"log",required_argument,NULL,'l',},
        	{"scale",required_argument,NULL,'s'},
       		{"period",required_argument,NULL,'p'},
        	{0, 0, 0, 0} // end of array
	};



	while (-1 != (opt = getopt_long(argc, argv, "", long_opts, NULL))) 
	{
    	switch (opt) {
        	case 'l':
        		l_flag=1;
        		lfd = fopen(optarg, "w");
            	break;
        	case 's': 
        		if(optarg)
	            break;
	        case 'p':
	        	per = atoi(optarg);
	        	if(per<=0)
	        	{
	        		fprintf(stderr, "Bad argument, continuing with old value of period...\nDefaule value is 1 second\n");
	        	}
	        	break;
        	default:
        		fprintf(stderr, "INVALID OPTION(S)\nCorrect Usage: --log=\"logfile\" --scale=C --period=500\nTerminating...\n");
	         	exit(1);
	    }
	}

	pthread_t tempThread;
  	if (pthread_create(&tempThread, NULL, (void *)printer, NULL) < 0) {
  		fprintf(stderr,"ERROR cannot create thread");
  		exit(1);
  	}

	pthread_t btnThread;
	if (pthread_create(&btnThread, NULL, (void *)btnExit, NULL) < 0) {
		fprintf(stderr, "lol rip thread");
		exit(1);
	}

  	while(1)
  	{
  		
  		char commandBuffer[1024];

  		if(read(0, commandBuffer, 1024)>0)
  		{
  			if(strcmp(commandBuffer,"OFF") == 0)
  			{
  				stop_flag=1;
  				shutdown_flag=1;
  				if(l_flag==1)
  				{
  					fprintf(lfd,"SHUTDOWN\n");
  					fflush(lfd);
  					fclose(lfd);
  				}
  				fprintf(stdout,"SHUTDOWN\n");
  				// pthread_join(temperatureThread, NULL):
  				break;
  			}
  			else if(strcmp(commandBuffer, "STOP")==0)
  			{
  				if(l_flag==1)
  					{
  						fprintf(lfd,"STOP\n");
  						fflush(lfd);
  					}
  				if(stop_flag==0)
  					{
  						stop_flag=1;
  					}
  			}
  			else if(strcmp(commandBuffer, "START")==0)
  			{
  				if(l_flag==1)
  					{
  						fprintf(lfd, "START\n");
  						fflush(lfd);
  					}
  				if(stop_flag==1)
  				{
  					stop_flag=0;
  					fprintf(stdout,"START\n");
  				}

  			}
  			else if(strcmp(commandBuffer,"SCALE=F")==0)
  			{
  				fprintf(stdout,"SCALE=F\n");
  				if(l_flag==1)
  				{
  					fprintf(lfd, "SCALE=F\n");
  					fflush(lfd);
  				}
  				temp_type=0;

  			}
  			else if(strcmp(commandBuffer,"SCALE=C")==0)
  			{
  				;//fprintf()
  			}
  		}
  	
  	}

  	exit(0);
}
