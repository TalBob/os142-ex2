#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int add_path(char* path){
  if (lastPath==10){
    cprintf("could not add path - all paths in use\n");
    exit();
  }
  strncpy(PATH[lastPath], path, strlen(path)+1);
  cprintf("path added '%s'\n", PATH[lastPath]);
  lastPath++;
  return 0;
}

int
sys_add_path(void)
{
  char *path;
  if(argstr(0, &path) < 0)
    return -1;
  return add_path(path);
}

//-------------------------PATCH----------------//


int
sys_signal(void){
  
  int *signum;
  sighandler_t *handler;
  if (argptr(0, (void*)&signum, sizeof(signum)) <0) {
      return -1;
  }
  if (argptr(1, (void*)&handler, sizeof(handler)) <0 ) {
      return -1;
  }
  return signal(signum, handler);

}

int
sys_sigsend(void){
  int* pid,signum;
  
  if (argptr(0, (void*)&pid, sizeof(pid)) <0) {
      return -1;
  }
  if (argptr(1, (void*)&signum, sizeof(signum)) <0 ) {
      return -1;
  }
  return sigsend(pid, signum);
}
//-------------------------PATCH----------------//

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
