#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "uthread.h"
#include "signal.h"

//------------------PATCH-----------------PART 3
//------------------TODO make file to make FSSP------//

#define Q 0
#define P 1
#define R 2
#define Z 3
#define M 4
#define F 6
#define I -1

void transitionTableFunc(void* index);
void barrier();
void printer();
int n;
int counter = 0;
int* squad;
struct binary_semaphore arrival, departure;

void printer() { // prints the soldiers array
    int i;
    printf( 1, "I AM IN PRINTER \n");
    for (i=0; i<n; i++) {
	printf( 1, "%d ", squad[i]);
    }
    printf( 1, "\n");
}

void barrier(int toPrint) { //wait for n threads to arraive
    binary_semaphore_down(&arrival);
    counter++;
    if (counter < n)
	binary_semaphore_up(&arrival);
    else binary_semaphore_up(&departure); //last thread arrived. all threads can pass
   
    if ((toPrint) && (counter == n)){
//       printf( 1, "ABOUT TO PRINT \n");
      int i;
      for(i = 0; i < n; i++){
	printf(1,"%d ",squad[i]);
      }
      printf(1,"\n");
//       printf( 1, "I AM IN PRINTER \n");
//       printer(); //print array
    }
    binary_semaphore_down(&departure);
    counter--;
    if(counter > 0)
	binary_semaphore_up(&departure);
    else binary_semaphore_up(&arrival);
}

// void barrier(int print){
//   printf(1,"in BARRIER \n");
//   binary_semaphore_down(&arrival);
//   counter = counter + 1;
//   if(counter < n){
//     printf(1,"in BARRIER\n");
//     binary_semaphore_up(&arrival);
//   }
//   else{
//     binary_semaphore_up(&departure);
//   }
//   if(print && counter==n){
//     int i;
//     for(i = 0; i < n; i++){
//       printf(1,"%d ",squad[i]);
//     }
//     printf(1,"\n");
//   }
//   binary_semaphore_down(&departure);
//   counter = counter - 1;
//   if(counter > 0 ){
//     binary_semaphore_up(&departure);
//   }
//   else{
//     binary_semaphore_up(&arrival);
//   }
// }

int transitionTable[5][6][6] = {//Q
			       {{ Q, P, Q, Q, I, Q},
				{ P, P, I, I, I, P},
				{ Q, I, Q, I, I, I},
				{ Q, I, I, Q, I, Q},
				{ I, I, I, I, I, I},
				{ Q, P, Q, Q, I, I}},
				//P
			       {{ Z, Z, R, R, I, I},
				{ Z, I, Z, Z, I, I},
				{ R, Z, Z, I, I, Z},
				{ R, Z, I, Z, I, Z},
				{ I, I, I, I, I, I},
				{ Z, I, Z, Z, I, I}},
				//R
			       {{ I, I, R, P, Z, I},
				{ I, I, M, R, M, I},
				{ R, M, I, I, M, I},
				{ P, R, I, I, R, I},
				{ Z, M, M, R, M, I},
				{ I, I, I, I, I, I}},
				//Z
			       {{ I, I, Q, P, Q, I},
				{ I, Z, I, Z, I, I},
				{ Q, I, Q, Q, I, Q},
				{ P, Z, Q, F, Q, F},
				{ Q, I, I, Q, Q, Q},
				{ I, Z, Q, F, Q, I}},
				
				//M
			       {{ I, I, I, I, I, I},
				{ I, I, I, I, I, I},
				{ I, I, R, Z, I, I},
				{ I, I, Z, I, I, I},
				{ I, I, I, I, I, I},
				{ I, I, I, I, I, I}}};




void transitionTableFunc(void* index){
//   printf(1,"IN TRANSITION FUNC INDEX: %d\n", (int)index);
  int right, left, nextState, self;
  self = squad[(int)index];
  
  while(self != F){
//     printf(1,"IN WHILE\n");
    if( (int)index == 0 ){
      left=5;
    }
    else{
      left = squad[(int)index-1];
    }
    if( (int)index == n-1 ){
      right=5;
    }
    else{
      right = squad[(int)index+1];
    }
//     printf(1,"AFTER IFS\n");
    
    nextState = transitionTable[self][left][right];
//     printf(1,"SELF: %d LEFT: %d RIGHT: %d -> NEW: %d \n", self,left, right, nextState);
//     printf(1,"BEFORE FIRST BARRIER\n");
    
    barrier(0);
//     printf(1,"AFTER FIRST BARRIER\n");
    squad[(int)index] = nextState;
//     printf(1,"SQUAD[INDEX]: %d \n",squad[(int)index]);
    barrier(1);
//     printf(1,"AFTER SEC BARRIER\n");
    self = nextState;
  }
//   printf(1,"out of while?!?!\n");
}

int main(int argc, char **argv){

  
  
  if(argc<2){
    printf(1,"missing arguments\n");
    exit();
  }
//   printf(1,"FIRST PRINT\n");
  
  int i;
  n = atoi(argv[1]);
  squad = (int *) malloc(sizeof(int) * n);
//   printf(1,"SEC PRINT\n");
  squad[0] = P;
  
//    printf(1,"3RD PRINT\n");
  
  for(i = 1; i<n; i++){
    squad[i] = Q;
  }
  uthread_init();
  binary_semaphore_init(&arrival, 1); 
  binary_semaphore_init(&departure, 0); 
  
//   printf(1,"4TH PRINT\n");
  for(i = 0; i<n; i++){
    uthread_create(transitionTableFunc, (void *)i);
  }
//   printf(1,"5TH PRINT\n");
//   uthread_join(n);
  
  
    
  uthread_exit();
  free(squad);
  exit();
}



// void foo(void* sem) {
//   int k;
//   binary_semaphore_down((struct binary_semaphore*)sem);   
//   for (k=0; k<20; k++) {
// 	printf (1, "foo: tid: %d\n", uthread_self());
//   }
//   binary_semaphore_up((struct binary_semaphore*)sem);
// }

