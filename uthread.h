#include "types.h"

#define THREAD_QUANTA 5
//-------PATCH---------------//

/********************************
	Macors which inline assembly
 ********************************/
// Saves the value of esp to var
#define STORE_ESP(var) 	asm("movl %%esp, %0;" : "=r" ( var ))

// Loads the contents of var into esp
#define LOAD_ESP(var) 	asm("movl %0, %%esp;" : : "r" ( var ))

// Saves the value of ebp to var
#define STORE_EBP(var) 	asm("movl %%ebp, %0;" : "=r" ( var ))

// Loads the contents of var into ebp
#define LOAD_EBP(var) 	asm("movl %0, %%ebp;" : : "r" ( var ))

// Calls the function func
#define CALL(addr)		asm("call *%0;" : : "r" ( addr ))

// Pushes the contents of var to the stack
#define PUSH(var)		asm("movl %0, %%edi; push %%edi;" : : "r" ( var ))

// Push all
#define PUSHAL() asm("pushal;")
// Pop all
#define POPAL() asm("popal;")

/* Possible states of a thread; */
typedef enum  {T_FREE, T_RUNNING, T_RUNNABLE, T_SLEEPING, T_FRESH} uthread_state;


#define STACK_SIZE  4096
#define MAX_THREAD  64

typedef struct uthread uthread_t, *uthread_p;

// simpler version of exclusion lock.
struct semaLock {
  uint locked;       // Is the lock held?
  
  // For debugging:
  char *name;        // Name of lock.
};

struct uthread {
	int				tid;
	int 	       	esp;        /* current stack pointer */
	int 	       	ebp;        /* current base pointer */
	char		   *stack;	    /* the thread's stack */
	uthread_state   state;     	/* running, runnable, sleeping */
	int		numOfWaiting;
	struct uthread* waiting[MAX_THREAD]; /*waiting threads */
};

struct binary_semaphore{
  struct uthread* semaQueue[MAX_THREAD];
  struct semaLock* lock;
  int value;
  int semaIn;
  int semaOut;
};

 
int uthread_init(void);
int  uthread_create(void (*func)(void *), void* value);
void uthread_exit(void);
void uthread_yield(void);
int  uthread_self(void);
int  uthread_join(int tid);
void binary_semaphore_init(struct binary_semaphore* semaphore, int value);
void binary_semaphore_down(struct binary_semaphore* semaphore);
void binary_semaphore_up(struct binary_semaphore* semaphore);

