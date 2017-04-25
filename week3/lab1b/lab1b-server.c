#include <termios.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int to_child_pipe[2];
int from_child_pipe[2];
pid_t pid = -1;
pid_t w;
int status;
int newsockfd;
char buffer[2048];
struct pollfd fds[2];

void sysFailed(char* sysCall, int exitNum)
{
  fprintf(stderr, "%s failed: %s\n", sysCall, strerror(errno));
  exit(exitNum);
}


int main( int argc, char *argv[] ) {
  int sockfd, portno, clilen;
  //  char buffer[256];
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

  fds[0].fd = newsockfd;
  fds[1].fd = from_child_pipe[0];
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[1].events = POLLIN | POLLHUP | POLLERR;
  while(1) {   
    /* If connection is established then start communicating */
    bzero(buffer,2048);
    //  n = read( newsockfd,buffer,255 );
    if (pipe(to_child_pipe) == -1)
      {
	fprintf(stderr, "pipe() failed!\n");
	exit(1);
      }
    if (pipe(from_child_pipe) == -1) 
      {
	fprintf(stderr, "pipe() failed! \n");
	exit(1);
      }

    if ((pid = fork()) == -1)
      {
	sysFailed("fork", 1);
      }

    if (pid > 0)//parent
      {

	if (close(to_child_pipe[0]) == -1)
	  {
	    sysFailed("close", 1);
	  }
      
	if (close(from_child_pipe[1]) == -1)
	  {
	    sysFailed("close", 1);
	  }

	for (;;) {
	  int value = poll(fds, 2, 0);
	  int count = 0;

	  //reading from keyboard
	  if (fds[0].revents & POLLIN) {
	    if ((count = read(newsockfd, buffer, 2048)) == -1)
	      {
		sysFailed("Read", 1);
	      }

	    int i;
	    for (i = 0; i < count; i++)
	      {

		if (write(1, &buffer[i], 1) == -1)
		  {
		    sysFailed("write", 1);
		  }

	      }
	    //forward to shell
	    if (write(to_child_pipe[1], buffer, count) == -1)
	      {
		sysFailed("write", 1);
	      }
	  }
	    
	    //reading from shell

	  if (fds[1].revents & POLLIN) {
	    if ((count = read(from_child_pipe[0], buffer, 2048)) == -1)
	      {
		sysFailed("read", 1);
	      }
	    int j;
	    for (j = 0; j < count; j++)
	      {

		
		if (write(1, &buffer[j], 1) == -1)
		  {
		    sysFailed("write", 1);
		  }

	      }

	    write(newsockfd, buffer, 2048);
	  }
	}
      }
    else if (pid == 0) {

      if (close(to_child_pipe[1]) == -1)
	{
	  sysFailed("close", 1);
	}
    
      if (close(from_child_pipe[0]) == -1)
	{
	  sysFailed("close", 1);
	}
    
      if (dup2(to_child_pipe[0], STDIN_FILENO) == -1)
	{
	  sysFailed("dup2", 1);
	}

      if (dup2(from_child_pipe[1], STDOUT_FILENO) == -1)
	{
	  sysFailed("dup2", 1);
	}

      if (dup2(from_child_pipe[1], 2) == -1)
	{
	  sysFailed("dup2", 1);
	}

      if (close(to_child_pipe[0]) == -1)
	{
	  sysFailed("close", 1);
	}
    
      if (close(from_child_pipe[1]) == -1)
	{
	  sysFailed("close", 1);
	}
      char *execvp_argv[2];
      char execvp_filename[] = "/bin/bash";
      execvp_argv[0] = execvp_filename;
      execvp_argv[1] = NULL;
      if (execvp(execvp_filename, execvp_argv) == -1) 
	{
	  fprintf(stderr, "execvp() failed!\n");
	  exit(1);
	}
    
    }
    else {
      fprintf(stderr, "fork() failed!\n");
      exit(1);
    }




 
    if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }
   
    printf("Here is the message: %s\n",buffer);
   
    /* Write a response to the client */
    //  n = write(newsockfd,"I got your message",18);
   
    if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
    }
  }
  return 0;
}
