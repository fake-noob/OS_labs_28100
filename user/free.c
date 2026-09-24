#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
  printf("Free memory: %d bytes\n", freemem());
  exit(0);
}