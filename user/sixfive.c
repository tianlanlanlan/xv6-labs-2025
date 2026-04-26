#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

static const char seperator[] = {' ', '-', '\r', '\t', '\n', '.', '/', ','};

static int is_seperator(char c) {
  for (int i = 0; i < sizeof(seperator) / sizeof(char); ++i) {
    if (c == seperator[i]) {
      return 1;
    }
  }
  return 0;
}

inline int is_digit(char c) { return (c >= '0' && c <= '9') ? 1 : 0; }

void sixfive_single_file(const char *file_path) {
  int fd = open(file_path, O_RDONLY);
  if (fd < 0) {
    printf("Read file '%s' failed\n", file_path);
    exit(1);
  }

  int decimal = 0;
  int found_digit = 0;

  char c;
  while (read(fd, &c, 1) == 1) {
    if (!is_seperator(c) && is_digit(c)) {
      decimal = decimal * 10 + (c - '0');
      found_digit = 1;
    } else {
      if (found_digit == 1 && (decimal % 5 == 0 || decimal % 6 == 0)) {
        printf("%d\n", decimal);
      }

      // reset
      decimal = 0;
      found_digit = 0;
    }
  }

  if (found_digit == 1 && (decimal % 5 == 0 || decimal % 6 == 0)) {
    printf("%d\n", decimal);
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <file ...>\n", argv[0]);
    exit(1);
  }

  for (int i = 1; i < argc; ++i) {
    sixfive_single_file(argv[i]);
  }

  return 0;
}