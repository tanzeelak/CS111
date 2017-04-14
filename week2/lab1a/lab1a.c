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
pid_t pid;

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
  tattr.c_iflag = ISTRIP; /* Clear ICANON and ECHO. */
  tattr.c_oflag = 0;
  tattr.c_lflag = 0;
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

void
read_from_pipe (int file)
{
  FILE *stream;
  int c;
  stream = fdopen (file, "r");
  while ((c = fgetc (stream)) != EOF)
    putchar (c);
  fclose (stream);
}

/* Write some random text to the pipe. */

void
write_to_pipe (int file)
{
  FILE *stream;
  stream = fdopen (file, "w");
  fprintf (stream, "hello, world!\n");
  fprintf (stream, "goodbye, world!\n");
  fclose (stream);
}



int
main (int argc, char* argv[])
{
  char c;

  set_input_mode ();
  int rfd = 0;
  int optParse = 0;
  int shellFlag = 0;
  int pipe1[2], pipe2[2];


  while (1)
    {
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


      if (shellFlag) {
	pid = fork();
	if (pid == 0)
	  {
	    close(pipe1[1]);
	    close(pipe2[0]);
	    close(0);
	    close(1);
	    dup(pipe1[0]);
	    dup(pipe2[1]);
	    close(pipe1[0]);
	    close(pipe2[1]);
	    
	    
	    //read_from_pipe(pipe1[0]);
	    execvp("/bin/bash", NULL);





	    return(2);
	  }
	else if (pid < 0)
	  {
	    fprintf(stderr, "Failed to fork. %s\n", strerror(errno));
	    exit(2);
	  }
	else 
	  {

	    close(pipe1[0]);
	    close(pipe2[1]);
	    close(1);
	    close(0);
	    dup(pipe1[1]);
	    dup(pipe2[0]);
	    close(pipe1[1]);
	    close(pipe2[0]);

	    char buff;
	    int rfd = read (STDIN_FILENO, &c, 1);
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
		  write(pipe1[0], &buff, 1);
		}
	      else
		{
		  write(1, &c, 1);
		  write(pipe1[0], &buff, 1);
		}
	    }
	    else {
	      fprintf(stderr, "Failed to read file. %s\n", strerror(errno));
	      exit(1);
	    }

	    return(2);
	  }

	
      }


      else {
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
	      //putchar(&temp);
	    }     
	  else
	    write(1, &c, 1);
	  //putchar (c);
	}
	else {
	  fprintf(stderr, "Failed to read file. %s\n", strerror(errno));
	  exit(1);
	}
      }

    }

  exit(1);
}
