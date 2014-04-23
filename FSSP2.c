#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "uthread.h"
#include "signal.h"

void transitions(void* i);
void barrier();
void printer();
  
int counter = 0;
struct binary_semaphore arrival;
struct binary_semaphore departure;
//Q = 0 //R = 1 //L = 2 //S = 3 //* = 4 //F = 5
int stateQ_0[5][5] = {{0,0,2,0,0},{1,1,-1,-1,1},{0,-1,2,3,0},{0,3,-1,-1,-1},{0,0,2,0,-1}};
int stateR_1[5][5] = {{1,1,0,2,-1},{0,1,2,-1,2},{3,-1,0,5,-1},{0,1,2,-1,2},{3,-1,0,5,-1}};
int stateL_2[5][5] = {{2,3,0,0,-1},{0,0,1,1,0},{2,-1,2,2,-1},{1,5,-1,-1,5},{-1,-1,1,1,-1}};
int stateS_3[5][5] = {{0,3,2,0,-1},{1,-1,-1,5,-1},{3,-1,-1,3,-1},{0,3,5,-1,-1},{0,3,5,-1,-1}};
int* soldiersState;
int numOfSoldiers;

int main(int argc,char** argv){
    
    if ( argc != 2 ){ /* argc should be 2 for correct execution */
    
        printf( 1, " no args, please deliver the number of soldiers\n" );
	exit();
    }
    int i;
    numOfSoldiers=atoi(argv[1]);
    
    soldiersState = (int *) malloc(sizeof(int) * numOfSoldiers); 
    soldiersState[0] = 1; //initialize general
    // initialize in t=0 all soldiers
    for (i=1; i<numOfSoldiers; i++) {
	soldiersState[i] = 0;
    }
    
    // initialize threads
    uthread_init();
    binary_semaphore_init(&arrival, 1);
    binary_semaphore_init(&departure, 0); 
 
    
    // create new thread per soldier
    for (i = 0; i<numOfSoldiers; i++) {
	uthread_create(transitions, (void *)i);
    }
    
    // exit
    uthread_exit();
    exit();
}


void transitions(void* i) { // handle one soldier
    
    // find next state
    int index = (int)i;    
    int next_state;
    int currState = soldiersState[index];
    while (currState!=5) { // run until finish state
      // update left state if exist
      int leftState;
      if (index>0) {
	leftState = soldiersState[index-1];
      }
      else leftState = 4;
      
      // update right state if exist
      int rightState;
      
      if (index<numOfSoldiers-1) {
	rightState = soldiersState[index+1];
      }
      else rightState = 4;
      
      // get state
      if (currState == 0) {
	  next_state = stateQ_0[leftState][rightState];
      } 
      else if (currState == 1) {
	  next_state = stateR_1[leftState][rightState];
      }
      else if (currState == 2) {
	  next_state = stateL_2[leftState][rightState];
      }
      else if (currState == 3) {
	  next_state = stateS_3[leftState][rightState];
      }
      else {
	printf( 1, "algorithem works only on 2^k numbers. index: %d, currState %d\n", index, currState);
	exit();
      }
           
      barrier(0); //wait for all soldiers to find current state
      
      soldiersState[index] = next_state; //update state for time t+1
      
      barrier(1); //print array
      
      currState = soldiersState[index]; // get current state
    }
}

void barrier(int toPrint) { //wait for n threads to arraive
    binary_semaphore_down(&arrival);
    counter++;
    if (counter < numOfSoldiers)
	binary_semaphore_up(&arrival);
    else binary_semaphore_up(&departure); //last thread arrived. all threads can pass
    
    if ((toPrint) && (counter == numOfSoldiers))
      printer(); //print array

    binary_semaphore_down(&departure);
    counter--;
    if(counter > 0)
	binary_semaphore_up(&departure);
    else binary_semaphore_up(&arrival);
}


void printer() { // prints the soldiers array
    int i;
//     printf( 1, "im going to print\n ");
    
    for (i=0; i<numOfSoldiers; i++) {
	printf( 1, "%d ", soldiersState[i]);
    }
    printf( 1, "\n");
}