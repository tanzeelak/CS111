#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


void seghandle(int signum) {
  fprintf(stderr, "Segment fault");
	  exit(2);
}

static int verbose_flag;

int main(int argc, char* argv[])
{

  
  
  int c, fd0, fd1;
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
          puts ("option -i\n");
	  int filedesc = open(optarg, O_RDONLY);
	  if (filedesc != -1)
	    dup2(filedesc, 0);
          break;

        case 'o':
          puts ("option -o\n");
	  int out = open("out", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	  if (out != -1)
	    dup2(out, 1);
          break;

        case 's':
          printf ("option -s with value `%s'\n", optarg);
          break;

        case 'c':
          printf ("option -c with value `%s'\n", optarg);
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

  
  char buff;
  while(read(0, &buff, 1))
    {
      write(1, &buff, 1);
    } 



  //  write(fd, 

  exit (0);
  

}
