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

/* Use this variable to remember original terminal attributes. */

struct termios saved_attributes;
int to_child_pipe[2];
int from_child_pipe[2];
pid_t pid = -1;
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


int pipeRead(void)
{
  char c;
  set_input_mode ();
  int rfd;

  while(1)
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
	    char temp2 = '\n';
	    write(1, &temp, 2);
	    write(to_child_pipe[1], &temp2, 2); 

	  }     
	else
	  {
	    write(1, &c, 1);
	    write(to_child_pipe[1], &c, 1);
	  }
      }
      else {
	fprintf(stderr, "Failed to read file. %s\n", strerror(errno));
	exit(1);
      }
    }
}




void setupPoll()
{
  printf("or even here");
  //goes outside the parent process
  fds[0].fd = open(STDIN_FILENO, O_RDWR);
  fds[1].fd = open(0, O_RDWR);
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[1].events = POLLIN | POLLHUP | POLLERR;
  //  ret = poll(fds, 2, 0);
  printf("right before loo");

  //loop goes in the parent process
  for (;;) {
    int value = poll(fds, 2, 0);
    
    if (fds[0].revents & POLLIN) {
      
    }
    if (fds[0].revents & (POLLHUP+POLLERR)) {
    
    }
  }
  /*  while ((ret = poll(fds, 2, 0)) > 0 ) {
  
    for (i=0; i<2; i++) {
      if (fds[i].revents & POLLIN) {
  
	//READ GOES HERE
       	pipeRead();
	printf("lol does it enter ere\n");
      }
      if ((fds[i].revents & POLLHUP) || (fds[i].revents & POLLERR)) {
  
	exit(1);
	printf("or here?\n");
      }
    }
  }
  */

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
  fds[1].fd = open(0, O_RDWR);
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[1].events = POLLIN | POLLHUP | POLLERR;

  pid = fork();
  if (pid > 0)//parent
    {


      for (;;) {
	int value = poll(fds, 2, 0);
    
	if (fds[0].revents & POLLIN) {
	  close(to_child_pipe[0]);
	  close(from_child_pipe[1]);
	  char buffer[2048];
	  int count = 0;
	  count = read(STDIN_FILENO, buffer, 2048);
	  write(to_child_pipe[1], buffer, count);
	  count = read(from_child_pipe[0], buffer, 2048);
	  write(STDOUT_FILENO, buffer, count);

	  //	  pipeRead();
	}
	if (fds[0].revents & (POLLHUP+POLLERR)) {
	  exit(1);
	}
      }
      //  setupPoll();
      //      close(to_child_pipe[0]);
      //      close(from_child_pipe[1]);

    }
  else if (pid == 0) {
    close(to_child_pipe[1]);
    close(from_child_pipe[0]);
    dup2(to_child_pipe[0], STDIN_FILENO);
    dup2(from_child_pipe[1], STDOUT_FILENO);
    close(to_child_pipe[0]);
    close(from_child_pipe[1]);
    //    execvp("/bin/bash", NULL);
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
  //  pipeRead();
}





int
main (int argc, char* argv[])
{

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
      fprintf(stderr, "hell yeah\n");
      pipeSetup();
    }
  else {
    readWrite();
  }
  return EXIT_SUCCESS;
}
