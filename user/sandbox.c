#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"

void usage(char *s) {
  fprintf(2, "Usage: %s <mask> <path> <command>\n", s);
  fprintf(2, "  mask: the mask of system calls to reject\n");
  fprintf(2, "  path: file path allowed to be accessed\n");
  fprintf(2, "  command: comands will be sandboxed\n");
  exit(1);
}

// Sandbox a command by disallowing system calls in mask and
// system calls that are using path
int main(int argc, char *argv[]) {
  int i;
  char *nargv[MAXARG]; // New argv

  if (argc < 4) {
    usage(argv[0]);
  }
  char *mask_str = argv[1];
  char *path_str = argv[2];

  if (mask_str[0] < '0' || mask_str[0] > '9') {
    usage(argv[0]);
  }

  // strip off the first n arguments to sandbox
  int n = 0;
  for (i = 3; i < argc && i < MAXARG; i++) {
    // printf("argv[%d] = %s\n", n, argv[i]);
    nargv[n++] = argv[i];
  }
  nargv[n] = 0;

  int pid = fork();
  if (pid < 0) {
    printf("%s: exec fork failed\n", argv[0]);
    exit(1);
  }
  if (pid == 0) {
    // printf("sandbox: mask: '%s', path: '%s'\n", mask_str, path_str);
    if (interpose(atoi(mask_str), path_str) < 0) {
      printf("%s: interpose failed", argv[0]);
      exit(1);
    }
    exec(nargv[0], nargv);
    printf("%s: exec %s failed\n", argv[0], nargv[0]);
    exit(1);
  } else {
    wait(0);
  }

  return 0;
}
