#include "uthread.h"
#include "types.h"
#include "stat.h"
#include "user.h"
 
 
void test(void *t){
  int i = 0;
//      while (i < 50){
                printf(1,"thread child %p\n", t);
                i++;
//      }
}
 
int main(int argc,char** argv){
        uthread_init();
        int i;
        int tid;
        for (i = 0; i < 500; i++) {
          tid = uthread_create(test, (void *) i);
          if (!tid)
                goto out_err;
        }
   
    while (i < 90){
      printf(1,"thread father\n");
      i++;
    }
        exit();
        out_err:
        printf(1,"Faild to create thread, we go bye bye\n");
        exit();
}



//////////////////////thraeds//////////////////


// #include "uthread.h"
// #include "types.h"
// #include "stat.h"
// #include "user.h"
//  
// struct binary_semaphore b;
//  
// void test(void *t){
//   int i = 0;
//   while (i < 1000) {
//     binary_semaphore_down(&b);
//     printf(1,"thread child %p\n", t);
//     binary_semaphore_up(&b);
//     i++;
//   }
// }
//  
// int main(int argc,char** argv){
//         uthread_init();
//         binary_semaphore_init(&b, 1);
//         int tid = uthread_create(test, (void *) 0);
//     if (!tid)
//       goto out_err;
//     tid = uthread_create(test, (void *) 1);
//     if (!tid)
//      goto out_err;
//     uthread_join(tid);
//         exit();
//         out_err:
//         printf(1,"Faild to create thread, we go bye bye\n");
//         exit();
// }

///////////////////////////////////////////////////SEMAPHORE DONE/////////////


// #include "types.h"
// #include "stat.h"
// #include "fcntl.h"
// #include "user.h"
// #include "x86.h"
// #include "uthread.h"
// #include "signal.h"
// 
// void foo(void* i);
// void bar(void* j);
// 
// //-----------------------------------PART 3
// int main(){
//    uthread_init();
//    
//    
//     struct binary_semaphore semaphore;
//     binary_semaphore_init(&semaphore, 1); 
//     
// 
//     uthread_create(&foo, &semaphore);
//     uthread_create(&foo, &semaphore);
// //     uthread_join(2);
//     
//     uthread_exit();
//     
//     exit();
// }
// 
// void foo(void* sem) {
//   int k;
//   binary_semaphore_down((struct binary_semaphore*)sem);   
//   for (k=0; k<20; k++) {
// 	printf (1, "foo: tid: %d\n", uthread_self());
//   }
//   binary_semaphore_up((struct binary_semaphore*)sem);
// }
// 
// 
// 
// // -------------------- PART 2
// // int main(){
// //    uthread_init();
// //    
// //    
// //     struct binary_semaphore semaphore;
// //     binary_semaphore_init(&semaphore, 1); 
// //     binary_semaphore_down(&semaphore);
// //     binary_semaphore_up(&semaphore);
// //     
// // //    int j=7, i=3;
// // // 
// // 
// // //     
// // //     uthread_create(&bar, &j); 
// // //     
// // //     uthread_create(&foo, &i);
// // //     uthread_join(1);
// // //     uthread_join(2);
// // //     int k,l,m;
// // //     for (k=0; k<1; k++) {
// // //       for (l=0; l<1; l++) {
// // // 	for (m=0; m<1; m++) {
// // // 	  printf (1, "main\n");
// // // 	}
// // //       }
// // //     }
// // //     printf (1, "main\n");
// //     
// //     uthread_exit();   
// //     exit();
// // }
// // 
// // void foo(void* i) {
// //   int k,l,m;
// //   for (k=0; k<2; k++) {
// //     for (l=0; l<2; l++) {
// //       for (m=0; m<2; m++) {
// // 	printf (1, "foo: %d\n", *(int *)i);
// //       }
// //     }
// //   }
// //   printf (1, "foo: %d\n", *(int *)i);
// // }
// // 
// // void bar(void* j) {
// //   int k,l,m;
// //   for (k=0; k<2; k++) {
// //     for (l=0; l<2; l++) {
// //       for (m=0; m<2; m++) {
// // 	printf (1, "bar: %d\n", *(int *)j);
// //       }
// //     }
// //   }
// //   printf (1, "bar: %d\n", *(int *)j);
// // }
// 
// //----------------------- eran -----------------------
// /*
// void test(void *t){
//         while (1){
//                 printf(1,"thread child %p\n", t);
//         }
// }
// int main(int argc,char** argv){
//         uthread_init();
//         uthread_create(test, (void *) 5);
//         while (1){
//                 printf(1,"thread father\n");
//         }
//         exit();
// 	return 0;
// }*/
