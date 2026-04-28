#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

typedef union {
  char bytes[2];
  int data;
} int16_type;

void
memdump(char *fmt, char *data)
{
  // Your code here.
  for (int i = 0; fmt[i] != '\0'; ++i) {
    char c = fmt[i];
    if (c == 'i') {
      // 32-bit integer, in decimal
      printf("%d\n", *(int *)data);
      data += sizeof(int);
    } else if (c == 'S') {
      // string
      printf("%s\n", data);
      break;
    } else if (c == 's') {
      uint64 *ptr_addr = (uint64 *)data;
      char *str_addr = (char *)*ptr_addr;
      printf("%s\n", str_addr);
      data += sizeof(uint64);
    } else if (c == 'p') {
      // print 64-bit integer, in hex
      printf("%lx\n", *(uint64 *)data);
      data += sizeof(uint64);
    } else if (c == 'h') {
      // print 16-bit integer, in decimal
      int16_type u;
      u.bytes[0] = *data;
      u.bytes[1] = *(data + 1);
      printf("%d\n", u.data);
      data += 2;
    } else if (c == 'c') {
      // print as char
      printf("%c\n", *(char *)data);
      data += sizeof(char);
    } else {
      printf("error: %c\n", c);
      while (1) {
        ;
      }
    }
  }
}
