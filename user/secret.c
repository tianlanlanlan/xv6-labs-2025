#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define DATASIZE (8*4096)

char data[DATASIZE];

int
main(int argc, char *argv[])
{
  if(argc != 2){
    printf("Usage: secret the-secret\n");
    exit(1);
  }

  char *str = "This may help.";

  strcpy(data, str);

  strcpy(data + 16, argv[1]);

  // printf("str addr = %p\n", &(str[0]));

  exit(0);
}

