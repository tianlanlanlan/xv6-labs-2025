#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Decode syscall mask into syscall number.
static int decode_syscall_mask(int syscall_mask) {
  int syscall_num = 0;
  while (syscall_mask > 1) {
    syscall_mask = syscall_mask >> 1;
    syscall_num++;
  }
  return syscall_num;
}

// Set syscall mask for current process.
uint64 sys_interpose(void) {
  int syscall_mask;
  char path[MAXPATH];

  argint(0, &syscall_mask);
  if (argstr(1, path, MAXPATH) < 0) {
    return -1;
  }

  struct proc *p = myproc();
  int syscall_num = decode_syscall_mask(syscall_mask);
  if (syscall_num < 0 || syscall_num >= NELEM(p->syscall_interpose_mask)) {
    return -1;
  } else {
    p->syscall_interpose_mask[syscall_num] = 1;
  }

  (void)path; // currently path is unused
  return 0;
}
