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


void
reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
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
  tcgetattr (STDIN_FILENO, &saved_attributes);
  atexit (reset_input_mode);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_iflag = ISTRIP;
  tattr.c_oflag = 0;
  tattr.c_lflag = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

void signal_callback_handler(int signum){
  //WAIT PID CASE 2
  printf("Caught signal SIGPIPE %d\n",signum);
  exit(1);
}


int readWrite(void)
{
  char c;
  set_input_mode ();
  int rfd;
  while (1)
    {
      rfd = read (STDIN_FILENO, &c, 1);
      if (rfd >= 0) {
	if (c == '\004')          /* C-d */
	  {  
	    reset_input_mode();
	    break;
	  }
	else if (c == '\n' || c == '\r')
	  {
	    char temp[2] = {'\r', '\n'};
	    write(1, &temp, 2);
	  }     
	else
	  write(1, &c, 1);
      }
      else {
	fprintf(stderr, "Failed to read file. %s\n", strerror(errno));
	exit(2);
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
  

  //  setupPoll();
  fds[0].fd = STDIN_FILENO;
  fds[1].fd = from_child_pipe[0];
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[1].events = POLLIN | POLLHUP | POLLERR;

  pid = fork();
  fprintf(stderr, "%d", pid);

  if (pid > 0)//parent
    {

      signal(SIGPIPE, signal_callback_handler);
      for (;;) {
	int value = poll(fds, 2, 0);
    
	close(to_child_pipe[0]);
	close(from_child_pipe[1]);
	char buffer[2048];
	int count = 0;

	//reading from keyboard
	if (fds[0].revents & POLLIN) {

	  count = read(STDIN_FILENO, buffer, 2048);

          int i;
          for (i = 0; i < count; i++)
            {
	      if (*buffer == '\004') //control D
		{
		  //WAIT PID CASE 1
		  close(to_child_pipe[1]);
		  close(to_child_pipe[0]);
		  
		  write(to_child_pipe[1], buffer, count);
	      
		  close(from_child_pipe[1]);
		  close(from_child_pipe[0]);
		  
		  kill(pid, SIGHUP);
	      
		  exit(0);
	
		}
	      if (*buffer == 0x03) //control C
		{
                  kill(pid, SIGINT);
                  exit(0);
		}
              if (buffer[i] == '\r' || buffer[i] == '\n' )
                {
                  buffer[i] = '\n';
		  char temp[2] = {'\r', '\n'};
		  write(1, temp, 2);
                }
	      else {
		write(1, &buffer[i], 1);
	      }
            }
	  //forward to shell
	  write(to_child_pipe[1], buffer, count);
	}
	if (fds[0].revents & (POLLHUP+POLLERR)) {
	  fprintf(stderr, "ya entered");
	
	}
	
	//reading from shell
	if (fds[1].revents & POLLIN) {

	  count = read(from_child_pipe[0], buffer, 2048);
	  int j;
	  for (j = 0; j < count; j++)
	    {
	      if (buffer[j] == '\n')
		{
                  char temp[2] = {'\r', '\n'};
                  write(1, temp, 2);
		}
	      else {
		write(1, &buffer[j], 1);
	      }
	    }
	  //	  write(STDOUT_FILENO, buffer, count);
	}

	if (fds[1].revents & (POLLHUP+POLLERR)) {
	  //WAIT PID #3
	  
	  //testing
	  //on terminal: ./lab1a --shell (will show first four numbers)
	  //second terminal: ps -ef | grep shell
	  //kill -n 13 (those four numbers)

	  fprintf(stderr, "%d", pid);

	  w = waitpid(pid, &status, 0);

	  int upper = status&0xF0;
	  int lower = status&0x0F;

	  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", lower, upper);

	  exit(1);
	}
      }
    }
  else if (pid == 0) {

    close(to_child_pipe[1]);
    close(from_child_pipe[0]);
    dup2(to_child_pipe[0], STDIN_FILENO);

    dup2(from_child_pipe[1], STDOUT_FILENO);
    dup2(from_child_pipe[0], STDERR_FILENO);

    close(to_child_pipe[0]);
    close(from_child_pipe[1]);
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
  //getoptparse
  int optParse = 0;
  int shellFlag = 0;

  static struct option long_options[] = {
    {"shell", no_argument, 0, 's'},
    {0,0,0,0}
  };

  int option_index = 0;
  optParse = getopt_long(argc, argv, "i:o:sc:", long_options, &option_index);
  switch (optParse)
    {
    case 's':
      shellFlag = 1;
    default:
      break;
    }



  if (shellFlag)
    {
      pipeSetup();
    }
  else {
    readWrite();
  }
  return EXIT_SUCCESS;
}
