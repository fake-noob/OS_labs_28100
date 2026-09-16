#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // The uptime command doesn't take any arguments
  if (argc > 1) {
    fprintf(2, "usage: uptime\n");
    exit(1);
  }

  // Call the system call defined in user/user.h
  int ticks = uptime();
  
  if (ticks < 0) {
    fprintf(2, "uptime: system call failed\n");
    exit(1);
  }

  printf("%d\n", ticks);
  exit(0);
}