
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <mraa/aio.h>
#include <math.h>
#include <ctype.h>

int
main(int argc, char** argv)
{
	    const char* board_name = mraa_get_platform_name();
	        fprintf(stdout, "hello mraa\n Version: %s\n Running on %s\n", mraa_get_version(), board_name);
		    mraa_deinit();
		        return MRAA_SUCCESS;
}
