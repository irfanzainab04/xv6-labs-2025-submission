// user/sixfive.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SEP_CHARS " -\r\t\n./,"

int
is_sep(char c) {
  return strchr(SEP_CHARS, c) != 0;
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: sixfive file...\n");
    exit(1);
  }

  for(int i = 1; i < argc; i++){
    int fd = open(argv[i], 0);
    if(fd < 0){
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      continue;
    }

    char buf[32];
    int nb = 0;
    char c;
    int r;
    while((r = read(fd, &c, 1)) == 1){
      if(is_sep(c)){
        if(nb > 0){
          buf[nb] = '\0';
          int v = atoi(buf);
          if(v % 5 == 0 || v % 6 == 0)
            printf("%d\n", v);
          nb = 0;
        }
      } else if(c >= '0' && c <= '9'){
        if(nb < (int)sizeof(buf) - 1)
          buf[nb++] = c;
      } else {
        if(nb > 0){
          buf[nb] = '\0';
          int v = atoi(buf);
          if(v % 5 == 0 || v % 6 == 0)
            printf("%d\n", v);
          nb = 0;
        }
      }
    }
    if(nb > 0){
      buf[nb] = '\0';
      int v = atoi(buf);
      if(v % 5 == 0 || v % 6 == 0)
        printf("%d\n", v);
    }
    close(fd);
  }
  exit(0);
}

