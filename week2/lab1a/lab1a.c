#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <poll.h>

struct termios initState;

int
set_istrip (int desc, struct termios state)
{
  int result;

  result = tcgetattr (desc, &state);
  if (result < 0)
    {
      perror ("error in tcgetattr");
      return 0;
    }
  state.c_iflag = ISTRIP;
  state.c_oflag = 0;
  state.c_lflag = 0;

  result = tcsetattr (desc, TCSANOW, &state);
  if (result < 0)
    {
      perror ("error in tcsetattr");
      return 0;
    }
  return 1;
}



int main()
{
  set_istrip(0, initState);


}
