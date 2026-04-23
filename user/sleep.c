#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage:\n\t%s <ticks>\n", argv[0]);
    exit(1);
  }

  int ticks = atoi(argv[1]);
  printf("sleeping %d ticks...\n", ticks);
  pause(ticks);

  return 0;
}