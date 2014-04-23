#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "uthread.h"
#include "signal.h"
#include "spinlock.h"
#include "x86.h"


//-----------------PATCH-------------//


struct {
  struct uthread uthreads[MAX_THREAD];
} utTable;

struct uthread* runningThread;
struct uthread* nextThread;

static struct uthread* queue[MAX_THREAD];
int utInIndex = 0;
int utOutIndex = 0;

int nextTid = 0;


struct uthread * getNextThread(){
  struct uthread *u;
  
  if(utOutIndex != utInIndex){
    u = queue[utOutIndex % MAX_THREAD];
    queue[utOutIndex % MAX_THREAD] = 0;
    utOutIndex++;
  }
  else{
//     printf(1, "in else\n");
    u = 0;
  }
//   printf(1, "getting %d in queue\n", u->tid);
  
  return u;
  
}
void putInQueue(struct uthread * ut){
  if(ut->tid == 0 ){
    printf(1, "putting %d in queue\n", ut->tid);
  }
//   printf(1, "putting %d in queue\n", ut->tid);
    queue[utInIndex % MAX_THREAD] = ut;
    utInIndex++;
}

int uthread_init(){
  int i;
  
  nextTid = 0;
  utTable.uthreads[0].tid = 0;		      /* main thread tid is 0 */
  STORE_ESP(utTable.uthreads[0].esp);        /* put current stack pointer in main thread*/
  STORE_EBP(utTable.uthreads[0].ebp);        /* put current base pointer in main thread */
  STORE_EBP((char *)utTable.uthreads[0].stack);	    /*init the main thread's stack */
  utTable.uthreads[0].state = T_RUNNING;
  nextTid++;
  
  utTable.uthreads[0].numOfWaiting = 0;
  for(i = 0; i < MAX_THREAD; i++){
    utTable.uthreads[0].waiting[i] = 0;
  }
  
  for(i = 1; i< MAX_THREAD; i++){
    utTable.uthreads[i].state = T_FREE;
  }
  runningThread = &utTable.uthreads[0];
  
  if(signal(SIGALRM, uthread_yield) == -1){
    return -1;
  }
  alarm(THREAD_QUANTA);
  return 0;
}

// This function receives as arguments a pointer to the thread’s entry function and an argument for it.
// The function  allocates space for the thread, initializes it but not runs it just yet.
int uthread_create(void (*start_func)(void *), void*arg){
  struct uthread *u;
  
  for(u = utTable.uthreads; u < &utTable.uthreads[MAX_THREAD]; u++){
    if(u->state == T_FREE){
      goto found;
    }
  }
  return -1;
  
  found:
  u->stack = (char *) malloc(STACK_SIZE);
//   if((u->stack = (char *) malloc(STACK_SIZE)) == 0) {
//     return -1;
//   }
  u->esp = (int)u->stack + STACK_SIZE;        
  u->tid = nextTid++;
  u->numOfWaiting = 0;
  int i;
  for(i = 0; i < MAX_THREAD; i++){
    u->waiting[i] = 0;
  }
  printf(1,"%d in create\n", u->tid );
  u->esp -= 4;
  *(void **)u->esp = arg;
  u->esp -= 4;
  *(void **)u->esp = uthread_exit;
  u->esp -= 4;
  *(void **)u->esp = start_func;
  u->esp -= 4;
  *(void **)u->esp = (void *)u->esp;
  u->ebp = u->esp;
  u->state = T_RUNNABLE;
  putInQueue(u);
  return u->tid;
}


void conSwitch(){
  
  STORE_ESP(runningThread->esp);
  STORE_EBP(runningThread->ebp);
  
//   printf(1, "after pushal and store\n");

  runningThread = nextThread;
  
  LOAD_ESP(runningThread->esp);
  LOAD_EBP(runningThread->ebp);
}


void uthread_exit(){
  
  struct uthread * thread;
  runningThread->state=T_FREE;
  if(runningThread->tid != 0){
    free(runningThread->stack); //free memory
  }
//     wakeup threads that join on me
  while(runningThread->numOfWaiting > 0){
    
    thread = runningThread->waiting[(runningThread->numOfWaiting)-1];
    
    printf(1,"releasing thread %d\n", thread->tid);
    thread->state = T_RUNNABLE;
    putInQueue(thread);
    runningThread->numOfWaiting--;
  }
  if((nextThread = getNextThread())==0) {
//     printf(1,"problem!!\n");
      exit();
  }
  printf(1,"%d done exit\n", runningThread->tid);
  conSwitch();
  POPAL();
  alarm(THREAD_QUANTA); 

}

// void uthread_exit(void){
//   runningThread->state= T_FREE;
// }



int  uthread_self(void){
  return runningThread->tid;
}

void updateJoined(struct uthread * aThread, struct uthread * waitingThread){
  aThread->waiting[aThread->numOfWaiting] = waitingThread;
  aThread->numOfWaiting++;
  printf(1,"%d is waiting on %d\n",waitingThread->tid, aThread->tid);
  
}

int uthread_join(int tid){
  struct uthread *u;
  
  for(u = utTable.uthreads; u < &utTable.uthreads[MAX_THREAD]; u++){
    if(u->tid == tid){
      goto join_found;
    }
  }
  return 0;
  join_found:
    if(u->tid == runningThread->tid ||(nextThread = getNextThread()) == 0){
      return -1;
    }
    runningThread->state = T_SLEEPING;
    updateJoined(u, runningThread);
    PUSHAL(); //push all registers
    conSwitch();
    runningThread->state = T_RUNNING;

  return 0;
}


// This function picks up the next thread from the threads table, according to the scheduling policy,
// and restores its context.
void uthread_yield(void)
{
  alarm(THREAD_QUANTA);  
  printf(1, "thread %d yields\n", runningThread->tid);
  if((nextThread = getNextThread()) == 0){
//     printf(1, "in if\n");
    return;
  }
//   printf(1, "got next thread tid is: %d\n", &nextThread->tid);
//   printf(1, "got next thread state is: %s\n", &nextThread->state);
  runningThread->state = T_RUNNABLE;
  putInQueue(runningThread);
  
  PUSHAL(); //push all registers
  conSwitch();
  runningThread->state = T_RUNNING;
  POPAL(); 

}

////////////////////TODO ASS2.3/////////////////
void initlock(struct semaLock *lk, char *name){
  lk->locked = 0;
  lk->name = name;
}

int holding(struct semaLock *lock){
  return lock->locked;
}

void acquire(struct semaLock *lk){
  
  if(holding(lk)){
    printf(1, "PANIC! at acquire");
  }
  while(xchg(&lk->locked, 1) != 0);
}

void release(struct semaLock *lk){
  if(!holding(lk)){
    printf(1, "PANIC! at release");
//     uthread_exit();
  }
  xchg(&lk->locked, 0);
  
}

void binary_semaphore_init(struct binary_semaphore* semaphore, int value){
  if ((value < 0) || (value > 1)){
    return;
  }
  
  semaphore->value = value;
  semaphore->semaIn = 0;
  semaphore->semaOut = 0;
  initlock(semaphore->lock, "semaLock");
  
   
}

struct uthread * getNextSemaThread(struct binary_semaphore* semaphore){
  struct uthread *u;
  
  if(semaphore->semaIn != semaphore->semaOut ){
    u = semaphore->semaQueue[semaphore->semaOut % MAX_THREAD];
    semaphore->semaQueue[semaphore->semaOut % MAX_THREAD] = 0;
    semaphore->semaOut++;
  }
  else{
//     printf(1, "in else\n");
    u = 0;
  }
//   printf(1, "getting %d in queue\n", u->tid);
  
  return u;
  
}
void putInSemaQueue(struct binary_semaphore* semaphore, struct uthread * ut){
  
  semaphore->semaQueue[semaphore->semaIn % MAX_THREAD] = ut;
  semaphore->semaIn++;

}


void binary_semaphore_down(struct binary_semaphore* semaphore){
//   printf(1, "in sema down\n");
  acquire(semaphore->lock);
  if(semaphore->value<=0){
    runningThread->state = T_SLEEPING;
    
    putInSemaQueue(semaphore, runningThread);
    
    if((nextThread = getNextThread())==0){
      printf(1, "someone took my cookie\n");
      release(semaphore->lock);
      return ;
    }
    PUSHAL();
    release(semaphore->lock);
    conSwitch();
    POPAL();
    alarm(THREAD_QUANTA);
  }
  else{
    semaphore->value--;
    release(semaphore->lock);
  }
}

void binary_semaphore_up(struct binary_semaphore* semaphore){
  struct uthread* ut;
  acquire(semaphore->lock);
  if(semaphore->semaIn == semaphore->semaOut){
    semaphore->value++;
    release(semaphore->lock);
  }
  else{
//     printf(1, "in sema up\n");
    ut = getNextSemaThread(semaphore);
    ut->state = T_RUNNABLE;
//     printf(1, "%d is last in queue\n", ut->tid);
    putInQueue(ut);
    release(semaphore->lock);
  }
}



