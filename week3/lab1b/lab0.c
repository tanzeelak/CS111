#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


void sighandler(int signum) {
  fprintf(stderr, "Detected segment fault: %s\n", strerror(errno));
  exit(4);
}

static int verbose_flag;

int main(int argc, char* argv[])
{
  int c, fd0, fd1, filedesc, out;
  int inflag = 0;
  int outflag = 0;
  int seg = 0;
  char* inopt = NULL;
  char* outopt = NULL;
  while (1)
    {
      static struct option long_options[] = {
	{"input", required_argument, 0, 'i'},
	{"output", required_argument, 0, 'o'},
	{"segfault", no_argument, 0, 's'},
	{"catch", no_argument, 0, 'c'},
	{0,0,0,0}
      };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long(argc, argv, "i:o:sc:", long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 'i':
	  inflag = 1;
	  inopt = optarg;
          break;

        case 'o':
	  outflag = 1;
	  outopt = optarg;
          break;

        case 's':
	  seg = 1;
          break;

        case 'c':
          signal(SIGSEGV, sighandler);
	  break;

        default:
	  fprintf(stderr, "Unrecognized argument:\n--input=filename ... use the specified file as standard input (making it the new fd0).\n--output=filename ... create the specified file and use it as standard output (making it the new fd1). \n--segfault ... force a segmentation fault\n--catch\n", strerror(errno));
          exit(1);
        }
    }

  if (seg) {
    char *fail = NULL;
    *fail = 'A';
  }


  if (inflag) {

	  int ifd = open(inopt, O_RDONLY);
	  if (ifd >= 0) {
	    close(0);
	    dup(ifd);
	    close(ifd);
	  }
	  else {
	    fprintf(stderr, "Failed to open input file. %s\n", strerror(errno));
	    exit(2);
	  }
  }

  if (outflag) {

    int ofd = creat(outopt, 0666);
    if (ofd >= 0) {
      close(1);
      dup(ofd);
      close(ofd);
    } else {
      fprintf(stderr, "Failed to create output file. %s\n", strerror(errno));
      exit(3);
    }

  }

  char buff;
  while(read(0, &buff, 1))
    {
      write(1, &buff, 1);
    } 


  close(0);
  close(1);
  exit (0);
  

}
