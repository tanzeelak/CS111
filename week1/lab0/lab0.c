#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void sighandler(int signum) {
  fprintf(stdout, "Segment fault");
  exit(4);
}

static int verbose_flag;

int main(int argc, char* argv[])
{

  
  
  int c, fd0, fd1, filedesc, out;
  int seg = 0;
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

      c = getopt_long (argc, argv, "i:o:sc:",
                       long_options, &option_index);

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
	  filedesc = open(optarg, O_RDONLY);
	  if (filedesc != -1)
	    dup2(filedesc, 0);
	  else
	    fprintf(stderr, "failed to open input file");
          break;

        case 'o':
	  out = open(optarg, O_WRONLY | O_TRUNC | O_CREAT);
	  if (out != -1)
	    dup2(out, 1);
	  else
	    fprintf(stderr, "failed to create output file");
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
          abort ();
        }
    }

  /* Instead of --
     -- as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts ("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
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


  exit (0);
  

}
