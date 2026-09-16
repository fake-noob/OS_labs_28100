#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

char*
fmtname(char *path) {
  static char buf[DIRSIZ+1];
  char *p;

  // Find last component of path
  for(p = path+strlen(path); p >= path && *p != '/'; p--);
  p++;

  // Return the name padded
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void
find(char *path, char *target, int argc, char *argv[]) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // FIXED: Check for a match BEFORE the switch statement 
  // so that both files AND directories can trigger a match.
  if(strcmp(fmtname(path), target) == 0) {
    if(argc > 3 && strcmp(argv[3], "-exec") == 0) {
      char *exec_argv[MAXARG];
      int exec_argc = 0;
      
      // Copy the command and ALL its arguments
      for (int i = 4; i < argc; i++) {
          exec_argv[exec_argc++] = argv[i];
      }
      
      // Append the matched file path and null-terminate
      exec_argv[exec_argc++] = path;
      exec_argv[exec_argc] = 0;

      if(fork() == 0) {
        exec(exec_argv[0], exec_argv);
        fprintf(2, "find: exec %s failed\n", exec_argv[0]);
        exit(1);
      } else {
        wait(0);
      }
    } else {
      // Standard find behavior (no -exec)
      printf("%s\n", path);
    }
  }

  switch(st.type){
  case T_FILE:
    // Match check is handled above, so nothing to do here.
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, target, argc, argv); 
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "usage: find path filename [-exec command ...]\n");
    exit(1);
  }
  find(argv[1], argv[2], argc, argv);
  exit(0);
}