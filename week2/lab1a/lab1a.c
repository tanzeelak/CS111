#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <poll.h>

struct termios initState;

int main()
{
  int result;

  result = tcgetattr (1, &initState);
  if (result < 0)
    {
      perror ("error in tcgetattr");
    }
  
  struct termios newState;
  newState.c_iflag = ISTRIP;
  newState.c_oflag = 0;
  newState.c_lflag = 0;
  
 
  //result = tcsetattr (desc, TCSANOW, &initState);
  // if (result < 0)
  //{
  //  perror ("error in tcsetattr");
  //}


}
