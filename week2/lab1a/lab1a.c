#include <termios.h>
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
  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

int
main (void)
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
	    //putchar(&temp);
	  }     
	else
	  write(1, &c, 1);
	//putchar (c);
      }
      else {
	fprintf(stderr, "Failed to read file. %s\n", strerror(errno));
	exit(2);
      }
    }

  return EXIT_SUCCESS;
}
