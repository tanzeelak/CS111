#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>


void seghandle(int signum) {
  fprintf(stderr, "Segment fault");
	  exit(2);
}


int main(int argc, char* argv[])
{
  struct option long_options[] = {
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"segfault", no_argument, 0, 's'},
    {"catch", no_argument, 0, 'c'},
    {0,0,0,0}
  };

  int option;
  int fd0, fd1;
  FILE *file;
  file = fopen(argv[1], "r");
  int option_index = 0;
  if (file) {
    while((option = getopt_long(argc, argv, "i:o:sc", long_options, 0)) != -1)
      {
        switch(option)
          {
	  case 0:
	    if (long_options[option_index].flag != 0)
	      break;
	    printf("option %s", long_options[option_index].name);
	    if (optarg)
	      printf(" with arg %s", optarg);
	    printf ("\n");
	    break;
          case 'i':
	    if ((file = fopen(argv[1], "r")) != NULL){
	      printf("case i");
              }
              else {
                perror("Error opening file");
                exit(2);
		  }
	      break;
	    case 'o':
	      fd1 = creat(optarg, 0666);
	      if (fd1 > -1)
		{
		  printf("case o");
		  
		}
	      else {
		perror("Error creating file");
		exit(2);
	      }
	      break;
	    case 's':
	      printf("case s");
	      char fail = NULL;
	      break;
	    case 'c':
	      printf("case c");
	      signal(SIGSEGV, seghandle);
	      break;
	  default: 
	    abort();
	    break;
	      }

	  }
  }
    exit(0);

  }
