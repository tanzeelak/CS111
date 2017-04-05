#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>


int main(int arc, char* argv[])
{
  struct option long_options[] = {
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"segfault", no_argument, 0, 's'},
    {"catch", no_argument, 0, 'c'},
    {0,0,0,0}
  }

  int option;
  int fd0, fd1;
  FILE *file;
  file = fopen(argv[1], "r");
  if (file) {
    while(option = getopt_long(argc, argv, "i:o:sc", long_options, 0) != -1)
      {
        switch(option)
          {
          case 'i':
            if ((file = fopen(argv[1], "r") != -1){

              }
              else {
                stderr("Error opening file");
                exit
		  }
	      break;
	    case 'o':
	      break;
	    case 's':
	      break;
	    case 'c':
	      break;
	      }

	  }

	while ((c = getc(file)) != EOF)
	  putchar(c);
	fclose(file);
      }

  }
