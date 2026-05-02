#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

// Found data[] offset in secret.c by run `riscv64-unknown-elf-nm user/_secret | grep freep`
#define DATA_OFFSET 0x10

// Found elf section and program segment mapping by run:
// `riscv64-unknown-elf-readelf -l user/_secret`

int
main(int argc, char *argv[])
{
  for (int i = 0; i < 100; ++i) {
    char *va = (char *) sbrk(PGSIZE); // va is page aligned
    if (memcmp(va + DATA_OFFSET, "This may help.", 14) == 0) {
      printf("%s\n", va + DATA_OFFSET + 16);
      break;
    }
  }
  exit(1);
}
