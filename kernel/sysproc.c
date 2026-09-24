#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
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
    sleep_prepare(&ticks); // 2026 API update
    release(&tickslock);
    sleep();               // 2026 API update
    acquire(&tickslock);
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

uint64
sys_interpose(void)
{
  int mask;
  uint64 pathaddr;
  char path[MAXPATH];
  struct proc *p = myproc();

  argint(0, &mask);
  argaddr(1, &pathaddr);

  // 1. Monotonically increase the mask (a process can never un-sandbox itself)
  p->mask |= mask;

  // 2. Only allow setting the pathname if it hasn't been set yet
  if (pathaddr != 0) {
    if (fetchstr(pathaddr, path, MAXPATH) >= 0) {
      if (p->pathname[0] == '\0') {
        safestrcpy(p->pathname, path, MAXPATH);
      }
    }
  }

  return 0;
}