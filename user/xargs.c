
#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define MAX_ARG_LEN 128

char *get_former_cmds_outputs(char *buf, int max) {
  int i, cc;
  char c;

  for (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    if (cc < 1)
      break;
    buf[i++] = c;
    if (c == '\0')
      break;
  }
  buf[i] = '\0';
  return buf;
}

void run_cmd(const int argc, const char *argv[], const char *one_cmd_outputs,
             const int outputs_len) {
  char *exec_argv[MAXARG] = {0};
  int exec_argc = 0;

  // Step1: Copy raw argv into new argv
  for (int i = 1; i < argc; i++) {
    exec_argv[exec_argc] = malloc(MAX_ARG_LEN * sizeof(char));
    strcpy(exec_argv[exec_argc], argv[i]);
    exec_argc++;
  }
  const char *exec_cmd = exec_argv[0];

  // Step2: Append former cmd outputs into new argv
  for (int i = 0; i < outputs_len && one_cmd_outputs[i] != '\0'; i++) {
    int j = 0;
    exec_argv[exec_argc] = malloc(MAX_ARG_LEN * sizeof(char));
    while (one_cmd_outputs[i] != '\0' && one_cmd_outputs[i] != ' ' &&
           i < outputs_len) {
      exec_argv[exec_argc][j++] = one_cmd_outputs[i++];
    }
    if (j > 0) {
      exec_argv[exec_argc][j] = '\0';
      exec_argc++;
    }
  }

  //   for (int i = 0; i < exec_argc; i++) {
  //     printf("exec_argv[%d]: %s\n", i, exec_argv[i]);
  //   }

  // Step3. Run command
  if (fork() == 0) {
    // Child process
    // printf("Running: %s\n", exec_cmd);
    exec(exec_cmd, exec_argv);
  } else {
    // Parent process
    wait(0);
  }

  // Step4. Free argv
  for (int i = 0; i < exec_argc; i++) {
    free(exec_argv[i]);
  }
}

int main(const int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(2, "Usage: xargs <command> [arguments...]\n");
    exit(1);
  }

  //   for (int i = 0; i < argc; i++) {
  //     printf("argv[%d]: %s\n", i, argv[i]);
  //   }

  char buf[MAX_ARG_LEN];
  get_former_cmds_outputs(buf, sizeof(buf));
  //   printf("former cmd output: '%s'", buf);

  for (int i = 0; i < sizeof(buf) && buf[i] != '\0'; i++) {
    // Extract one command's outputs
    char one_cmd_outputs[MAX_ARG_LEN] = {0};
    int j = 0;
    while (buf[i] != '\n' && i < sizeof(buf)) {
      one_cmd_outputs[j++] = buf[i++];
    }
    if (j > 0) {
      one_cmd_outputs[j] = '\0';
      //   printf("one_cmd_outputs = '%s'\n", one_cmd_outputs);
      run_cmd(argc, argv, one_cmd_outputs, strlen(one_cmd_outputs));
    }
  }

  exit(0);
}
