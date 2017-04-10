#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


void sighandler(int signum) {
  fprintf(stderr, "Segment fault: %s", strerror(errno));
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
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'i':
	  inflag = 1;
	  inopt = optarg;
          break;

        case 'o':
	  outflag = 1;
	  outopt = optarg;
          break;

        case 's':
          //char* h;
	  //h = NULL;
	  seg = 1;
          break;

        case 'c':
          signal(SIGSEGV, sighandler);
	  break;

        default:
	  fprintf(stderr, "what you should actually do", strerror(errno));
          exit(1);
        }
    }

  /* Instead of --
     -- as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts ("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
    
  if (inflag) {

	  int ifd = open(inopt, O_RDONLY);
	  if (ifd >= 0) {
	    close(0);
	    dup(ifd);
	    close(ifd);
	  }
	  else {
	    fprintf(stderr, "failed to open input file", strerror(errno));
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
      fprintf(stderr, "failed to create output file", strerror(errno));
      exit(3);
    }

  }

  if (seg) {
    char *fail = NULL;
    *fail = 'A';
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
