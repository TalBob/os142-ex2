#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "signal.h"
#include "x86.h"
#include "uthread.h"

int
main(int argc, char *argv[])
{
    alarm(50);
    sleep(100);
    exit();
    
}