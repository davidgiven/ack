#include <lib.h>
#define alarm	_alarm
#include <unistd.h>

PUBLIC unsigned int alarm(sec)
unsigned int sec;
{
  return(_callm1(MM, ALARM, (int) sec, 0, 0, NIL_PTR, NIL_PTR, NIL_PTR));
}
