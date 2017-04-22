#include <termios.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Use this variable to remember original terminal attributes. */

struct termios saved_attributes;
int to_child_pipe[2];
int from_child_pipe[2];
pid_t pid = -1;
pid_t w;
int status;
struct pollfd fds[2];
int timeout_msecs = 500;
int ret;
int i;

void sysFailed(char* sysCall, int exitNum)
{
    fprintf(stderr, "%s failed: %s\n", sysCall, strerror(errno));
    exit(exitNum);
}

void
reset_input_mode (void)
{
  if (tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes) == -1)
    {
      sysFailed("tcsetattr", 1);
    }
}

void
set_input_mode (void)
{
  struct termios tattr;
  char *name;

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  /* Save the terminal attributes so we can restore them later. */
  if (tcgetattr (STDIN_FILENO, &saved_attributes) == -1)
    {
      sysFailed("tcgetattr", 1);
    }
  if(atexit (reset_input_mode) == -1) 
    {
      sysFailed("atexit", 1);
    }

  /* Set the funny terminal modes. */
  if (tcgetattr (STDIN_FILENO, &tattr) == -1)
    {
      sysFailed("tcgetattr", 1);
    }
  tattr.c_iflag = ISTRIP;
  tattr.c_oflag = 0;
  tattr.c_lflag = 0;
  
  if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr) == -1)
    {
      sysFailed("tcsetattr", 1);
    }
}

void signal_callback_handler(int signum){
  //WAIT PID CASE 2

  //  fprintf(stderr, "in signal handler: %d", pid);

  if (kill(pid, SIGINT) == -1)
    {
      sysFailed("kill", 1);
    }

  w = waitpid(pid, &status, 0);
  if (w == -1)
    {
      fprintf(stderr, "Waitpid failed: %s\n", strerror(errno));
      exit(1);
    }

  int upper = status&0xF0;
  int lower = status&0x0F;

  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", lower, upper);

  printf("Caught signal SIGPIPE %d\n",signum);
  exit(1);
}

int inputOutput(void)
{
  char buffer[2048];
  int rfd;
  while (1)
    {
      if ((rfd = read (STDIN_FILENO, buffer, 1)) ==  -1)
	{
	  sysFailed("read", 2);
	}
      if (rfd > 0) {
	if (*buffer == '\004')
	  {  
	    reset_input_mode();
	    break;
	  }
	else if (*buffer == '\n' || *buffer == '\r')
	  {
	    char smol[2] = {'\r', '\n'};
	    if (write(1, &smol, 2) == -1)
	      {
		sysFailed("write", 1);
	      }
	  }     
	else
	  {
	    if(write(1, buffer, 1) == -1)
	      {
		sysFailed("write", 1);
	      }
	  }
      }

    }
  
}

int pipeSetup(void)
{
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
  
  fds[0].fd = STDIN_FILENO;
  fds[1].fd = from_child_pipe[0];
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[1].events = POLLIN | POLLHUP | POLLERR;

  if ((pid = fork()) == -1)
    {
      sysFailed("fork", 1);
    }

  if (pid > 0)//parent
    {

      signal(SIGPIPE, signal_callback_handler);

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
	char buffer[2048];
	int count = 0;

	//reading from keyboard
	if (fds[0].revents & POLLIN) {

	  if ((count = read(STDIN_FILENO, buffer, 2048)) == -1)
	    {
	      sysFailed("Read", 1);
	    }

          int i;
          for (i = 0; i < count; i++)
            {
	      if (*buffer == '\004') //control D
		{
		  //WAIT PID CASE 1


		  if(close(to_child_pipe[1]) == -1)
		    {
		      sysFailed("Close", 1);
		    }
		  
		  if (read(from_child_pipe[0], buffer, 2048) == -1)
		    {
		      sysFailed("Read", 1);
		    }
		  w = waitpid(pid, &status, 0);
		  if (w == -1)
		    {
		      fprintf(stderr, "Waitpid failed: %s\n", strerror(errno));
		      exit(1);
		    }

		  int upper = status&0xF0;
		  int lower = status&0x0F;

		  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", lower, upper);
		  exit(0);
	
		}
	      if (*buffer == 0x03) //control C
		{
                  if(kill(pid, SIGINT) == -1)
		    {
		      sysFailed("Kill", 1);
		    }

		  waitpid(pid, &status, 0);
		  if (w == -1)
		    {
		      fprintf(stderr, "waitpid failed: %s\n", strerror(errno));
		      exit(1);
		    }

		  int upper = status&0xF0;
		  int lower = status&0x0F;

		  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", lower, upper);

                  exit(0);
		}
              if (buffer[i] == '\r' || buffer[i] == '\n' )
                {
                  buffer[i] = '\n';
		  char temp[2] = {'\r', '\n'};
		  if (write(1, temp, 2) == -1)
		    {
		      sysFailed("write", 1);
		    }
                }
	      else {
		if (write(1, &buffer[i], 1) == -1)
		  {
		    sysFailed("write", 1);
		  }
	      }
            }
	  //forward to shell
	  if (write(to_child_pipe[1], buffer, count) == -1)
	    {
	      sysFailed("write", 1);
	    }
	}
	if (fds[0].revents & (POLLHUP+POLLERR)) {
	  fprintf(stderr, "ya entered");
	
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
	      if (buffer[j] == '\n')
		{
                  char temp[2] = {'\r', '\n'};
		  
                  if (write(1, temp, 2) == -1)
		    {
		      sysFailed("write", 1);
		    }
		}
	      else {
		
		if (write(1, &buffer[j], 1) == -1)
		  {
		    sysFailed("write", 1);
		  }
	      }
	    }
	}

	if (fds[1].revents & (POLLHUP+POLLERR)) {

	  w = waitpid(pid, &status, 0);
	  if (w == -1)
	    {
	      fprintf(stderr, "waitpid failed: %s\n", strerror(errno));
	      exit(1);
	    }

	  int upper = status&0xF0;
	  int lower = status&0x0F;

	  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", lower, upper);

	  exit(1);
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

}


int
main (int argc, char* argv[])
{

  set_input_mode();
  int optParse = 0;
  int portFlag = 0;
  int logFlag = 1;
  int encFlag = 1;

  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"log", required_argument, 0, 'l'},
    {"encrypt", required_argument, 0, 'e'},
    {0,0,0,0}
  };

  int option_index = 0;
  while((optParse = getopt_long(argc, argv, "i:o:sc:", long_options, &option_index)) != -1){
    switch (optParse)
      {
      case 'p':
	portFlag = 1;
	break;
      case 'l':
	logFlag = 1;
	break;
      case 'e':
	encFlag = 1;
      case '?':
	fprintf(stderr, "--shell argument to pass input/output between the terminal and a shell:");
	exit(1);
      default:
	break;
      }
  }

  if (portFlag)
    {
      int sockfd = 0, n = 0;
      char recvBuff[1024];
      struct sockaddr_in serv_addr; 

      if(argc != 2)
	{
	  printf("\n Usage: %s <ip of server> \n",argv[0]);
	  return 1;
	} 

      memset(recvBuff, '0',sizeof(recvBuff));
      if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	  printf("\n Error : Could not create socket \n");
	  return 1;
	} 

      memset(&serv_addr, '0', sizeof(serv_addr)); 

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(5000); 

      char* link = "127.0.0.1";

      if(inet_pton(AF_INET, link, &serv_addr.sin_addr)<=0)
	{
	  printf("\n inet_pton error occured\n");
	  return 1;
	} 

      if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
	  printf("\n Error : Connect Failed \n");
	  return 1;
	} 

      while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
	{
	  recvBuff[n] = 0;
	  if(fputs(recvBuff, stdout) == EOF)
	    {
	      printf("\n Error : Fputs error\n");
	    }
	} 

      if(n < 0)
	{
	  printf("\n Read error \n");
	} 

      return 0;
    }

  return EXIT_SUCCESS;
}
