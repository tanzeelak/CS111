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
#include <mcrypt.h>

#define h_addr h_addr_list[0] /* for backward compatibility */

struct termios saved_attributes;
struct pollfd fds[2];
char buffer[2048];
int logfd;


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

int encrypt(ssize_t size)
{
  MCRYPT td;
  int i;
  char *key;
  char password[20];
  char *IV;
  int keysize=19; /* 128 bits */

  key=calloc(1, keysize);
  strcpy(password, "A_large_key");

  /* Generate the key using the password */
  /*  mhash_keygen( KEYGEN_MCRYPT, MHASH_MD5, key, keysize, NULL, 0, password, strlen(password));
   */
  memmove( key, password, strlen(password));

  td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  if (td==MCRYPT_FAILED) {
    return 1;
  }
  IV = malloc(mcrypt_enc_get_iv_size(td));

  /* Put random data in IV. Note these are not real random data,
   * consider using /dev/random or /dev/urandom.
   */

  /*  srand(time(0)); */
  for (i=0; i< mcrypt_enc_get_iv_size( td); i++) {
    IV[i]='a';
  }

  i=mcrypt_generic_init( td, key, keysize, IV);
  if (i<0) {
    mcrypt_perror(i);
    return 1;
  }

  /* Encryption in CFB is performed in bytes */
  mcrypt_generic (td, &buffer, size);

  /* Comment above and uncomment this to decrypt */
  /*    mdecrypt_generic (td, &block_buffer, 1);  */

  mcrypt_generic_deinit(td);
  mcrypt_module_close(td);

  return 0;
}

int decrypt(ssize_t size)
{
  MCRYPT td;
  int i;
  char *key;
  char password[20];
  char *IV;
  int keysize=19; /* 128 bits */

  key=calloc(1, keysize);
  strcpy(password, "A_large_key");

  /* Generate the key using the password */
  /*  mhash_keygen( KEYGEN_MCRYPT, MHASH_MD5, key, keysize, NULL, 0, password, strlen(password));
   */
  memmove( key, password, strlen(password));

  td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  if (td==MCRYPT_FAILED) {
    return 1;
  }
  IV = malloc(mcrypt_enc_get_iv_size(td));

  /* Put random data in IV. Note these are not real random data,
   * consider using /dev/random or /dev/urandom.
   */

  /*  srand(time(0)); */
  for (i=0; i< mcrypt_enc_get_iv_size( td); i++) {
    IV[i]='a';
  }

  i=mcrypt_generic_init( td, key, keysize, IV);
  if (i<0) {
    mcrypt_perror(i);
    return 1;
  }

  /* Encryption in CFB is performed in bytes */
  // mcrypt_generic (td, &buffer, size);

  /* Comment above and uncomment this to decrypt */
  mdecrypt_generic (td, &buffer, size);

  mcrypt_generic_deinit(td);
  mcrypt_module_close(td);

  return 0;
}

int main(int argc, char *argv[]) {
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  set_input_mode();

  int optParse = 0;
  int portFlag = 0;
  int encFlag = 0;
  int logFlag = 0;
  char* portopt = NULL;
  char* encopt = NULL;
  char* logopt = NULL;

  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"encrypt", required_argument, 0, 'e'},
    {"log", required_argument, 0, 'l'},
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
      case 'e':
	encFlag = 1;
	encopt = optarg;
	break;
      case 'l':
	logFlag = 1;
	logopt = optarg;
	break;
      default:
	fprintf(stderr, "--shell argument to pass input/output between the terminal and a shell:");
	exit(1);
      }
  }

  if (logFlag)
    {
      logfd = creat(logopt, 0666);
      if (logfd < 0)
	{
	  fprintf(stderr, "Failed to open input file. %s\n", strerror(errno));
	  exit(2);
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
      memset(buffer, 0, 2048);

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


      //encryption

      MCRYPT td;
      int i;
      char *key;
      char password[20];
      //      char block_buffer;
      char *IV;
      int keysize=19; /* 128 bits */

      key=calloc(1, keysize);
      strcpy(password, "A_large_key");

      /* Generate the key using the password */
      /*  mhash_keygen( KEYGEN_MCRYPT, MHASH_MD5, key, keysize, NULL, 0, password, strlen(password));
       */
      memmove( key, password, strlen(password));

      td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
      if (td==MCRYPT_FAILED) {
	return 1;
      }
      IV = malloc(mcrypt_enc_get_iv_size(td));

      /* Put random data in IV. Note these are not real random data, 
       * consider using /dev/random or /dev/urandom.
       */

      /*  srand(time(0)); */
      for (i=0; i< mcrypt_enc_get_iv_size( td); i++) {
	IV[i]=rand();
      }

      i=mcrypt_generic_init( td, key, keysize, IV);
      if (i<0) {
	mcrypt_perror(i);
	return 1;
      }

      mcrypt_generic (td, &buffer, rfd);

	/* Comment above and uncomment this to decrypt */
	/*    mdecrypt_generic (td, &block_buffer, 1);  */
     
      mcrypt_generic_deinit(td);
      mcrypt_module_close(td);

      /* Send message to the server */
      n = write(sockfd, buffer, strlen(buffer));
      if (logfd == 1)
	write(logfd, buffer, n);
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
      if (logfd == 1)
	write(logfd, buffer, n);

    }
  }
  return 0;
}
