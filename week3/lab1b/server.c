#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#define h_addr h_addr_list[0] /* for backward compatibility */

#define MAX_SIZE 2048
int to_child_pipe[2], from_child_pipe[2];
pid_t cd;
char buffer[MAX_SIZE];
int newsockfd, portno;

void port_handler();
void chid_process();
void parent_process();
void create_pipe(int pip[2]);

void fin_exit();



void port_handler()
{
    int sockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int  n;
    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    
    /* Now start listening for the clients, here process will
     * go in sleep mode and will wait for the incoming connection
     */
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    /* Accept actual connection from the client */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    
    /* If connection is established then start communicating */
    
    while(1)
    {
        bzero(buffer,MAX_SIZE);
        
        // n = read( newsockfd,buffer,MAX_SIZE-1 );
        //fprintf(stderr, "Error in first read   %d\n", n);
        
        create_pipe(to_child_pipe);
        create_pipe(from_child_pipe);
        
        cd =fork();
        if(cd<0)
        {
            fprintf(stderr, "Error in fork() function!\n\n");
            exit(1);
        }
        else if (cd ==0)
        {
            child_process();
        }
        else if(cd>0)
        {
            parent_process();
        }
        
        printf("Here is the message: %s\n",buffer);
        
        /* Write a response to the client */
        n = write(newsockfd,"I got your message",18);
        
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
}






void child_process()
{
    //printf("\nchild_process()\n");
    close(to_child_pipe[1]);
    dup2(to_child_pipe[0], 0);
    
    close(from_child_pipe[0]);
    
    dup2(from_child_pipe[1], 1);
    dup2(from_child_pipe[1], 2);
    
    close(to_child_pipe[0]);
    close(to_child_pipe[1]);
    
    char execvp_filename[]="/bin/bash";
    char *execvp_argv[2];
    execvp_argv[0]=execvp_filename;
    execvp_argv[1]=NULL;
    
    
    if(execvp(execvp_filename, execvp_argv) == -1)
    {
        fprintf(stderr, "execvp() failed!\n");
        exit(1);
    }
    
}



void fin_exit()
{
    int status =0;
    waitpid(cd, &status, 0);
    //reset();
    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS %d\n",WTERMSIG(status), WEXITSTATUS(status) );
    exit(0);
}

void signal_handler(int sig_num)
{
    
    if(sig_num == SIGINT)
    {
        fin_exit();
    }
    if(sig_num==SIGPIPE)
    {
        fin_exit();
    }
}
void parent_process()
{
    //printf("\nparent_process()\n");
    close(to_child_pipe[0]);
    close(from_child_pipe[1]);
    struct pollfd pfds[2];
    
    
    
    pfds[0].events = POLLIN | POLLERR | POLLHUP;
    pfds[0].fd = newsockfd;
    pfds[1].fd = from_child_pipe[0];
    pfds[1].events = POLLIN | POLLERR | POLLHUP ;
    
    while(1)
    {
        int ret_val = poll(pfds, 2, 0);
        if(ret_val<0)
        {
            fprintf(stderr, "Error in poll() call!\n");
            exit(1);
        }
        
        
        if(pfds[0].revents & POLLIN)
        {
            ssize_t lt = read(newsockfd, &buffer, MAX_SIZE);
            if (lt < 0) {
                perror("ERROR reading from socket");
                exit(1);
            }
            //fprintf(stderr,"%d\n", lt);//delete later
            //char buf[MAX_SIZE];
            for(ssize_t i = 0; i<lt;i++)
            {
                
                if(buffer[i]=='\003')
                {
                    //kill(cd, SIGINT);
                }
                
                if(buffer[i] =='\n' || buffer[i] == '\r')
                {
                    char lf = '\n';
                    char cr = '\r';
                    buffer[i] = '\n';
                    write(1, &cr, 1);
                    write(1, &lf, 1);
                }
                else
                {
                    //write(1, &buffer[i], 1);
                }
                
            }
            write(to_child_pipe[1], &buffer, lt);
            
        }
        
        
        else if(pfds[1].revents & POLLIN)
        {
            
            
            //fprintf(stderr, "!!!\n");
            ssize_t lt2 = read(from_child_pipe[0], buffer, MAX_SIZE);
            //	fprintf(stderr, "%d", len);
            for(ssize_t i=0; i<lt2; i++)
            {
                if (buffer[i] == '\n')
                {
                    char n = '\n';
                    char r= '\r';
                    write(newsockfd,&r, 1);
                    write(newsockfd, &n, 1);
                    continue;
                }	
                write(newsockfd, &buffer[i], 1);
            }
            
            //write(newsockfd, &buffer, MAX_SIZE);
            
            
        }
    }
}






void create_pipe(int pip[2])
{
    int a = pipe(pip);
    
    if(a==-1)
    {
        fprintf(stderr, "Error in creating pipe!\n");
        exit(EXIT_FAILURE);
    }
    //printf("pipe created!");
    
}


static struct option long_opts[] =
{
    {"port", 1, NULL, 'p'},
    {"encrypt", 0, NULL, 'e'},
    
    {0,0,0,0}
};

int main( int argc, char *argv[] ) {
    
    int opt =0;
    int portflag=0;
    
    while((opt = getopt_long(argc, argv, "", long_opts, NULL))!=-1)
    {
        switch(opt)
        {
            case 'p':
                //fprintf(stdout,"shell==1");
                signal(SIGINT, signal_handler);
                portflag=1;	
                portno=atoi(optarg);
                break;
                
            case 'e':
                break;
                
                
                
            default:
                fprintf(stderr, "Invalid Option!!\n");
                exit(1);
                break;
                
        }
    }
    
    
    
    if(portflag)
    {
        port_handler();
        
    }
    
    
    
    
    
    return 0;
}
