#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

/*
 * Run the command in argv once for every line read from standard input.
 * The complete input line is passed as one additional argument, matching
 * the -n 1 behaviour used by the xv6 lab.
 */
int main(int argc, char *argv[]) {
  char *args[MAXARG];
  char line[512];
  int narg;
  int nline;
  char c;

  if(argc < 2){
    fprintf(2, "usage: xargs command [args ...]\n");
    exit(1);
  }
  if(argc >= MAXARG){
    fprintf(2, "xargs: too many command arguments\n");
    exit(1);
  }

  /* argv[0] is the xargs program name; the remaining arguments are command. */
  for(narg = 1; narg < argc; narg++)
    args[narg - 1] = argv[narg];
  narg--;
  nline = 0;

  while(read(0, &c, 1) == 1){
    if(c != '\n'){
      if(nline < sizeof(line) - 1){
        line[nline++] = c;
      } else {
        fprintf(2, "xargs: input line too long\n");
        exit(1);
      }
      continue;
    }

    line[nline] = 0;
    args[narg] = line;
    args[narg + 1] = 0;
    if(fork() == 0){
      exec(args[0], args);
      fprintf(2, "xargs: exec %s failed\n", args[0]);
      exit(1);
    }
    wait(0);
    nline = 0;
  }

  /* Also process a final line that is not terminated by a newline. */
  if(nline > 0){
    line[nline] = 0;
    args[narg] = line;
    args[narg + 1] = 0;
    if(fork() == 0){
      exec(args[0], args);
      fprintf(2, "xargs: exec %s failed\n", args[0]);
      exit(1);
    }
    wait(0);
  }

  exit(0);
}
