#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Helper function that processes any open file descriptor
void process(int fd) {
  char buf[256], num[32];
  int n, k = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      char c = buf[i];
      if (c >= '0' && c <= '9') {
        num[k++] = c;
      } else {
        if (k > 0) {
          num[k] = 0;
          int v = atoi(num);
          if (v % 5 == 0 || v % 6 == 0) printf("%d\n", v);
          k = 0;
        }
      }
    }
  }
  // Catch any number left over at the very end of the file
  if (k > 0) {
    num[k] = 0;
    int v = atoi(num);
    if (v % 5 == 0 || v % 6 == 0) printf("%d\n", v);
  }
}

int
main(int argc, char *argv[])
{
  // If no arguments, read from standard input (file descriptor 0)
  if (argc == 1) {
    process(0);
  } else {
    // Otherwise, loop through all the provided files
    for (int a = 1; a < argc; a++) {
      int fd = open(argv[a], 0);
      if (fd < 0) {
        fprintf(2, "sixfive: cannot open %s\n", argv[a]);
        continue;
      }
      process(fd);
      close(fd);
    }
  }
  exit(0);
}