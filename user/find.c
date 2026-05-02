#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include <time.h>

#define FILE_PATH_MAX_LENGTH 512
#define MAX_ARGV_SIZE 64

const char *getfilename(const char *filepath) {
  const char *p = filepath;
  const char *filename = NULL;
  while (*p != '\0') {
    if (*p == '/') {
      filename = p;
    }
    ++p;
  }
  return (filename == NULL) ? "" : filename + 1;
}

typedef struct CmdArgs {
  int argc;
  char *argv[MAX_ARGV_SIZE];
} CmdArgs;

void exec_cmd(char *exec_argv[MAX_ARGV_SIZE]) {
  int pid = fork();
  if (pid) {
    // child
    exec(exec_argv[0], exec_argv);
  } else {
    // parent
    wait(NULL);
  }
}

void print_cmd_args(CmdArgs cmd_args) {
  printf("Running cmd:\n");
  for (int i = 0; i < cmd_args.argc; ++i) {
    printf("argv[%d] = '%s'\n", i, cmd_args.argv[i]);
  }
}

void find(char *input_filepath, const char *filename_to_find,
          CmdArgs cmd_args) {
  // printf("search '%s' in path: '%s'\n", filename_to_find, input_filepath);
  struct stat file_stat;
  stat(input_filepath, &file_stat);

  switch (file_stat.type) {
  case T_DEVICE:
  case T_FILE:
    // check file name
    const char *filename = getfilename(input_filepath);
    if (strcmp(filename, filename_to_find) == 0) {
      // found it!
      if (cmd_args.argc > 0) {
        // Append found path into exec_argv
        cmd_args.argv[cmd_args.argc] = input_filepath;
        cmd_args.argc += 1;

        // print_cmd_args(cmd_args);

        // Run
        if (fork() == 0) {
          // child
          exec(cmd_args.argv[0], cmd_args.argv);
        } else {
          // parent
          wait(NULL);
        }
      } else {
        printf("%s\n", input_filepath);
      }
    }
    break;

  case T_DIR:
    // Copy base dir path
    char buffer[FILE_PATH_MAX_LENGTH];
    int i = 0;
    for (i = 0; input_filepath[i] != '\0'; i++) {
      buffer[i] = input_filepath[i];
    }
    buffer[i++] = '/';
    buffer[i] = '\0';

    int fd = open(buffer, O_RDONLY);
    struct dirent dir_entry;
    while (read(fd, &dir_entry, sizeof(dir_entry)) == sizeof(dir_entry)) {
      if (dir_entry.inum == 0 /* why? */ || strcmp(dir_entry.name, ".") == 0 ||
          strcmp(dir_entry.name, "..") == 0) {
        continue;
      }

      // Concatenate file path
      memcpy(&buffer[i], dir_entry.name, DIRSIZ);
      find(buffer, filename_to_find, cmd_args);
    }
    close(fd);
    break;
  }
}

// 1. support 'find <dir> <filename>'
// 2. support "find . wc -exec echo hi" -> "hi ./wc"
int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <dir> <filename> [-exec <cmd>]\n", argv[0]);
    //              1   2       3       4     5
    exit(1);
  }

  char *filepath = argv[1];
  char *filename = argv[2];

  // Save exec command args
  CmdArgs cmd_args;
  if (argc >= 4 && strcmp(argv[3], "-exec") == 0) {
    int i = 4;
    for (; i < argc; i++) {
      cmd_args.argv[i - 4] = argv[i];
    }
    cmd_args.argc = i - 4;
  }
  find(filepath, filename, cmd_args);
  return 0;
}
