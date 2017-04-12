#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>

struct termios initState;
struct pollfd fds[1];
int timeout_msecs = 500;
int ret;
int i;


int main(int argc, char* argv[])
{
  int result;
  fds[0].fd = open(optarg, O_RDONLY);
  fds[0].events = POLLOUT | POLLWRBAND;

  result = tcgetattr (1, &initState);
  if (result < 0)
    {
      perror ("error in tcgetattr");
    }
  
  struct termios newState;
  newState.c_iflag = ISTRIP;
  newState.c_oflag = 0;
  newState.c_lflag = 0;
  
  ret = poll(fds, 1, timeout_msecs);
  char buff;
  if (ret > 0) {
    /* An event on one of the fds has occurred. */
    for (i=0; i<2; i++) {
      if (fds[i].revents & POLLWRBAND) {
        /* Priority data may be written on device number i. */
	while(read(0, &buff, 1))
	  {
	    write(1, &buff, 1);
	  }
      }
      if (fds[i].revents & POLLOUT) {
        /* Data may be written on device number i. */
      }
      if (fds[i].revents & POLLHUP) {
        /* A hangup has occurred on device number i. */
      }
    }
  }

 
  //result = tcsetattr (desc, TCSANOW, &initState);
  // if (result < 0)
  //{
  //  perror ("error in tcsetattr");
  //}


}
