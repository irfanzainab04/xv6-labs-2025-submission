#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "user/user.h"

// run "cmd args ... file" using fork+exec
void
runexec(char **exec_argv, char *file)
{
  if(fork() == 0){
    char *nargv[MAXARG];
    int i = 0;

    // copy command args
    while(exec_argv[i] && i < MAXARG-1){
      nargv[i] = exec_argv[i];
      i++;
    }

    // append the found file name
    nargv[i++] = file;
    nargv[i] = 0;

    exec(nargv[0], nargv);
    fprintf(2, "exec %s failed\n", nargv[0]);
    exit(1);
  }
  wait(0);
}

// recursive find
void
find(char *path, char *pattern, char **exec_argv)
{
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

  switch(st.type){
  case T_FILE:
    // match only filename, not suffix
    p = path + strlen(path);
    while(p >= path && *p != '/')
      p--;
    p++;
    if(strcmp(p, pattern) == 0){
      if(exec_argv)
        runexec(exec_argv, path);
      else
        printf("%s\n", path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0) continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, pattern, exec_argv);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "usage: find path pattern [-exec cmd args...]\n");
    exit(1);
  }

  char *path = argv[1];
  char *pattern = argv[2];
  char **exec_argv = 0;

  // check for -exec
  if(argc > 3 && strcmp(argv[3], "-exec") == 0){
    exec_argv = &argv[4];  // everything after -exec
  }

  find(path, pattern, exec_argv);
  exit(0);
}

