#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include <time.h>

const char *getfilename(char *filepath) {
  char *p = filepath;
  char *filename = NULL;
  while (*p != '\0') {
    if (*p == '/') {
        filename = p;
    }
    ++p;
  }
  return (filename == NULL)? "" : filename + 1;
}

int main(int argc, char *argv[]) {
  struct stat st;
  int fd;

  if (argc < 3) {
    printf("Usage: %s <filepath> <filename>\n", argv[0]);
    exit(1);
  }

  char *filepath = argv[1];
  char *filename = argv[2];

  if ((fd = open(filepath, O_RDONLY)) < 3) {
    printf("error: open %s failed\n", filepath);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    printf("error: stat\n");
    exit(1);
  }

  struct dirent dir_entry;

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    const char *name = getfilename(filepath);
    printf("%s\n", name);
    if (strcmp(name, filename) == 0) {
      printf("%s\n", filepath);
    }
    break;
  case T_DIR:
    while (read(fd, &dir_entry, sizeof(dir_entry)) == sizeof(dir_entry)) {
      if (dir_entry.inum == 0) { // why ?
        continue;
      }
      printf("%s\n", dir_entry.name);
    }
    break;
  }

  return 0;
}
