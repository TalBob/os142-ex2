#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "uthread.h"
#include "signal.h"

void foo(void* i);
void bar(void* j);

//-----------------------------------PART 3
int main(){
   uthread_init();
   
   
    struct binary_semaphore semaphore;
    binary_semaphore_init(&semaphore, 1); 
    

    uthread_create(&foo, &semaphore);
    uthread_create(&foo, &semaphore);
//     uthread_join(2);
    
    uthread_exit();
    
    exit();
}

void foo(void* sem) {
  int k;
  binary_semaphore_down((struct binary_semaphore*)sem);   
  for (k=0; k<20; k++) {
	printf (1, "foo: tid: %d\n", uthread_self());
  }
  binary_semaphore_up((struct binary_semaphore*)sem);
}



// -------------------- PART 2
// int main(){
//    uthread_init();
//    
//    
//     struct binary_semaphore semaphore;
//     binary_semaphore_init(&semaphore, 1); 
//     binary_semaphore_down(&semaphore);
//     binary_semaphore_up(&semaphore);
//     
// //    int j=7, i=3;
// // 
// 
// //     
// //     uthread_create(&bar, &j); 
// //     
// //     uthread_create(&foo, &i);
// //     uthread_join(1);
// //     uthread_join(2);
// //     int k,l,m;
// //     for (k=0; k<1; k++) {
// //       for (l=0; l<1; l++) {
// // 	for (m=0; m<1; m++) {
// // 	  printf (1, "main\n");
// // 	}
// //       }
// //     }
// //     printf (1, "main\n");
//     
//     uthread_exit();   
//     exit();
// }
// 
// void foo(void* i) {
//   int k,l,m;
//   for (k=0; k<2; k++) {
//     for (l=0; l<2; l++) {
//       for (m=0; m<2; m++) {
// 	printf (1, "foo: %d\n", *(int *)i);
//       }
//     }
//   }
//   printf (1, "foo: %d\n", *(int *)i);
// }
// 
// void bar(void* j) {
//   int k,l,m;
//   for (k=0; k<2; k++) {
//     for (l=0; l<2; l++) {
//       for (m=0; m<2; m++) {
// 	printf (1, "bar: %d\n", *(int *)j);
//       }
//     }
//   }
//   printf (1, "bar: %d\n", *(int *)j);
// }

//----------------------- eran -----------------------
/*
void test(void *t){
        while (1){
                printf(1,"thread child %p\n", t);
        }
}
int main(int argc,char** argv){
        uthread_init();
        uthread_create(test, (void *) 5);
        while (1){
                printf(1,"thread father\n");
        }
        exit();
	return 0;
}*/
