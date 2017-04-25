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
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#define h_addr h_addr_list[0] /* for backward compatibility */

struct termios saved_attributes;
struct pollfd fds[2];

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



int main(int argc, char *argv[]) {
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[2048];

  set_input_mode();

  int optParse = 0;
  int portFlag = 0;
  char* portopt = NULL;

  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {0,0,0,0}
  };

  int option_index = 0;
  while((optParse = getopt_long(argc, argv, "i:o:sc:", long_options, &option_index)) != -1){
    switch (optParse)
      {
      case 'p':
	portFlag = 1;
	portopt = optarg;
	break;
      case '?':
	fprintf(stderr, "--shell argument to pass input/output between the terminal and a shell:");
	exit(1);
      default:
	break;
      }
  }




  portno = atoi(portopt);

  /* Create a socket point */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  server = gethostbyname("localhost");

  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }


  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  memmove((char *) &serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  /* Now connect to the server */
  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    exit(1);
  }

  /* Now ask for a message from the user, this message
   * will be read by server
   */

  fds[0].fd = STDIN_FILENO;
  fds[1].fd = sockfd;
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[1].events = POLLIN | POLLHUP | POLLERR;


  while(1) {
    int value = poll(fds, 2, 0);



    if (fds[0].revents & POLLIN) {
      //      printf("Please enter the message: ");
    //   bzero(buffer,2048);
      memset(buffer, 0, 2048);
      //      fgets(buffer,255,stdin);
      int rfd;
      if ((rfd = read (STDIN_FILENO, buffer, 1)) ==  -1)
	sysFailed("read", 2);
      for (int i = 0; i < rfd; i++)
	{
	  if (buffer[i] == '\n' || buffer[i] == '\r')
	    {
	      char smol[2] = {'\r', '\n'};
	      if (write(1, &smol, 2) == -1)
		{
		  sysFailed("write ha", 1);
		}
	    }
	  else
	    {
	      if(write(1, &buffer[i], 1) == -1)
		{
		  sysFailed("write haha", 1);
		}
	    }
	}





      /* Send message to the server */
      n = write(sockfd, buffer, strlen(buffer));

      if (n < 0) {
	perror("ERROR writing to socket");
	exit(1);
      }

    /* Now read server response */

    }
    else if (fds[1].revents & POLLIN) {
    //   bzero(buffer,2048);
      memset(buffer, 0, 2048);
      n = read(sockfd, buffer, 2047);
      if (n == 0)
      {
          reset_input_mode();
          exit(0);
      }

      if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
      }

      write(STDOUT_FILENO, buffer, n);
    //   printf("%s\n",buffer);
    }
  }
  return 0;
}
