#include "kernel/types.h"
#include "user/user.h"

int main() {
  // pipe: p[0] is read end, p[1] is write end
  int p2c[2], c2p[2];
  char buf;
  pipe(p2c);
  pipe(c2p);
  if (fork() == 0) { // child process never reached
    // child reads from parent
    close(p2c[1]);
    close(c2p[0]);
    read(p2c[0], &buf, 1);
    printf("%d: received ping\n", getpid());

    // child writes to parent
    write(c2p[1], "c", 1);

    exit(0);
  } else { // parent
    // parent writes to child
    close(p2c[0]);
    close(c2p[1]);
    write(p2c[1], "p", 1);

    // parent reads from child
    read(c2p[0], &buf, 1);
    printf("%d: received pong\n", getpid());

    wait(0);
  }
  exit(0);
}