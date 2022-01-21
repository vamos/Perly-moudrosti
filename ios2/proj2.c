// IOS projekt 2
// Little Book Of Semaphores - River Crossing Problem
// Autor: xdvora2t

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

// shared memories
#define shmNAME "/ios-xdd63t-shm"               // line/action counter
#define shm_hcc_NAME "/ios-xdd63t-shm_hcc"      // hacker counter
#define shm_scc_NAME "/ios-xdd63t-shm_scc"      // serf counter
#define shm_wh_NAME "/ios-xdd63t-shm_hw"        // waiting hackers
#define shm_ws_NAME "/ios-xdd63t-shm_hs"        // waiting serfs
#define shm_barcnt_NAME "/ios-xdd63t-shm_bc"    // barrier counter

// semaphors
#define semNAME "/ios-xdd63t"                   // mutex
#define sembarrierNAME "/ios-xdd63t-barrier"    // barrier semafor
#define hqueueNAME "/ios-xdd63t-hqueue"         // hacker queue
#define squeueNAME "/ios-xdd63t-squeue"         // serf queue
#define semgetoutNAME "/ios-xdd63t-getout"      // captain last
#define semwriteNAME "/ios-xdd63ti-write" 

#define BOATCAPACITY 4

sem_t *mutex;
sem_t *hackersQueue;
sem_t *serfsQueue;
sem_t *getout;
sem_t *mutex_write;
int *shm;                                       // line/action counter
int *hck_cur_cnt;                               // hacker current count
int *srf_cur_cnt;                               // serf current count
int *hck_w_cnt;                                 // hacker waiting count
int *srf_w_cnt;                                 // serf waiting count
FILE *output = NULL;
int voyage;                                     // sail time
typedef struct {                                // barrier definition
   int threshold;                               
   int *counter;
   sem_t *sem;

} barrier_t;

void open_all(barrier_t *barrier) {

   mutex = sem_open(semNAME, O_RDWR);
   hackersQueue = sem_open(hqueueNAME, O_RDWR);
   serfsQueue = sem_open(squeueNAME, O_RDWR);
   barrier->sem = sem_open(sembarrierNAME, O_RDWR);
   barrier->threshold = BOATCAPACITY;
   getout = sem_open(semgetoutNAME, O_RDWR);
   mutex_write = sem_open(semwriteNAME, O_RDWR);
   
   int shmID;
   shmID = shm_open(shmNAME, O_RDWR, S_IRUSR | S_IWUSR);
   shm = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
   close(shmID);
   
   int hck_cur_cntID;
   hck_cur_cntID = shm_open(shm_hcc_NAME, O_RDWR, S_IRUSR | S_IWUSR);
   hck_cur_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, hck_cur_cntID, 0);
   close(hck_cur_cntID);
   
   int srf_cur_cntID;
   srf_cur_cntID = shm_open(shm_scc_NAME, O_RDWR, S_IRUSR | S_IWUSR);
   hck_cur_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, srf_cur_cntID, 0);
   close(srf_cur_cntID);
   
   int hck_w_cntID;
   hck_w_cntID = shm_open(shm_wh_NAME, O_RDWR, S_IRUSR | S_IWUSR);
   hck_w_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, hck_w_cntID, 0);
   close(hck_w_cntID);
   
   int srf_w_cntID;
   srf_w_cntID = shm_open(shm_ws_NAME, O_RDWR, S_IRUSR | S_IWUSR);
   srf_w_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, srf_w_cntID, 0);
   close(srf_w_cntID);
   
   int counterID;
   counterID = shm_open(shm_barcnt_NAME, O_RDWR, S_IRUSR | S_IWUSR);
   barrier->counter = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, counterID, 0);
   close(counterID);
}

void init_all(barrier_t *barrier) {

   mutex = sem_open(semNAME, O_CREAT, 0666, 1);
   sem_close(mutex);
   hackersQueue = sem_open(hqueueNAME, O_CREAT, 0666, 0);
   sem_close(hackersQueue);
   serfsQueue = sem_open(squeueNAME, O_CREAT, 0666, 0);
   sem_close(serfsQueue);
   barrier->sem = sem_open(sembarrierNAME, O_CREAT, 0666, 0);
   sem_close(barrier->sem);
   getout = sem_open(semgetoutNAME, O_CREAT, 0666, 1);
   sem_close(getout);
   mutex_write = sem_open(semwriteNAME, O_CREAT, 0666, 1);
   sem_close(mutex_write);
   
   int shmID; // action number shared memory init
   shmID = shm_open(shmNAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   ftruncate(shmID, sizeof(int));
   shm = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
   close(shmID);
   shm[0] = 0;
   munmap(shm, sizeof(int));
   
   int hck_cur_cntID; // current hacker count shared memory init
   hck_cur_cntID = shm_open(shm_hcc_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   ftruncate(hck_cur_cntID, sizeof(int));
   hck_cur_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, hck_cur_cntID, 0);
   close(hck_cur_cntID);
   hck_cur_cnt[0] = 0;
   munmap(hck_cur_cnt, sizeof(int));
   
   int srf_cur_cntID; // current serf count shared memory init
   srf_cur_cntID = shm_open(shm_scc_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   ftruncate(srf_cur_cntID, sizeof(int));
   srf_cur_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, srf_cur_cntID, 0);
   close(srf_cur_cntID);
   srf_cur_cnt[0] = 0;
   munmap(srf_cur_cnt, sizeof(int));
   
   int hck_w_cntID; // waiting hackers count shared memory init
   hck_w_cntID = shm_open(shm_wh_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   ftruncate(hck_w_cntID, sizeof(int));
   hck_w_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, hck_w_cntID, 0);
   close(hck_w_cntID);
   hck_w_cnt[0] = 0;
   munmap(hck_w_cnt, sizeof(int));
   
   int srf_w_cntID; // waiting hackers count shared memory init
   srf_w_cntID = shm_open(shm_ws_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   ftruncate(srf_w_cntID, sizeof(int));
   srf_w_cnt = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, srf_w_cntID, 0);
   close(srf_w_cntID);
   srf_w_cnt[0] = 0;
   munmap(srf_w_cnt, sizeof(int));
   
   int counterID; // barrier counter
   counterID = shm_open(shm_barcnt_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   ftruncate(counterID, sizeof(int));
   barrier->counter = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, counterID, 0);
   close(counterID);
   barrier->counter[0] = 0;
   munmap(barrier->counter, sizeof(int));
}

void unlink_all() {
   sem_unlink(semNAME);
   sem_unlink(hqueueNAME);
   sem_unlink(squeueNAME);
   sem_unlink(sembarrierNAME);
   sem_unlink(semgetoutNAME);
   sem_unlink(semwriteNAME);
   
   shm_unlink(shmNAME);
   shm_unlink(shm_hcc_NAME);
   shm_unlink(shm_scc_NAME);
   shm_unlink(shm_wh_NAME);
   shm_unlink(shm_ws_NAME);
   shm_unlink(shm_barcnt_NAME);
}

void rowBoat() {
   //fprintf(output, "sail!\n");
   sleep(rand()%(voyage+1)*0.001); 
}

void hacker_process(barrier_t *barrier) { // each hacker generated
   //fprintf(output,"start of hakerprocess\n");
   bool isCaptain = false;
   int process_number;
   sem_wait(mutex);
         sem_wait(mutex_write);
      fprintf(output, "%d   : HACK %d  : starts\n", ++shm[0], ++hck_cur_cnt[0]);
         sem_post(mutex_write);
      ++hck_w_cnt[0];
      process_number = hck_cur_cnt[0];
         sem_wait(mutex_write);
      fprintf(output, "%d   : HACK %d  : waits  : %d   : %d\n", ++shm[0], hck_cur_cnt[0], hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
      if (hck_w_cnt[0] == 4 ) {
         hck_w_cnt[0] = 0;
         isCaptain = true;
         sem_wait(mutex_write);
         fprintf(output, "%d   : HACK %d  : boards : %d   : %d\n", ++shm[0], hck_cur_cnt[0], hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(hackersQueue);
         sem_post(hackersQueue);
         sem_post(hackersQueue);
         sem_post(hackersQueue);
      }
      else if (hck_w_cnt[0] == 2 && srf_w_cnt[0] >= 2) {
         hck_w_cnt[0] = 0;
         --srf_w_cnt[0];
         --srf_w_cnt[0];
         isCaptain = true;
         sem_wait(mutex_write);
         fprintf(output, "%d   : HACK %d  : boards : %d   : %d\n", ++shm[0], hck_cur_cnt[0], hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(hackersQueue);
         sem_post(hackersQueue);
         sem_post(serfsQueue);
         sem_post(serfsQueue);
      }
      else {
         sem_post(mutex);
      }
   
   sem_wait(hackersQueue);
//   sem_wait(barrier);
barrier->counter[0]++;
if ( *barrier->counter == barrier->threshold ) {
      barrier->counter[0] = 0;
      sem_post(barrier->sem);
      sem_post(barrier->sem);
      sem_post(barrier->sem);
} else {
   sem_wait(barrier->sem);
}

      if(isCaptain) {
         rowBoat();
         sem_wait(getout);
         sem_wait(getout);
         sem_wait(getout);
         sem_wait(mutex_write);
         fprintf(output, "%d   : HACK %d  : captain exits : %d   : %d\n", ++shm[0], process_number, hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(mutex);
      } else {
         sem_wait(mutex_write);
         fprintf(output, "%d   : HACK %d  : member exits : %d   : %d\n", ++shm[0], process_number, hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(getout);
      }
   //fprintf(output,"end of hackerprocess\n");
   exit(0);
}

void hackerborn(int boh, int in_hst, barrier_t *barrier) { // generating hackers
   pid_t hackers[boh];
      for (int i=0; i<boh; i++) {
         if ( (hackers[i] = fork()) < 0 ) {
            exit(1);
         } else {
            if ( hackers[i] == 0 ) {
               hacker_process(barrier);
            } else {
               sleep(rand()%(in_hst+1)*0.001);
            }
         } 
      }
      for (int i=0; i<boh; i++) {
         waitpid(hackers[i],NULL,0);
      }
   //fprintf(output,"end of hackerborn\n");
   exit(0);
}

void serf_process(barrier_t *barrier) { // each serf generated
   //fprintf(output,"start of serfprocess\n");
   bool isCaptain = false;
   int process_number;
   sem_wait(mutex);
      sem_wait(mutex_write);
      fprintf(output, "%d   : SERF %d  : starts\n", ++shm[0], ++srf_cur_cnt[0]);
      sem_post(mutex_write);
      //if (srf_w_cnt[[0] + hck_w_cnt[0] < pier_capacity[0] ) {
      ++srf_w_cnt[0];
      process_number = srf_cur_cnt[0];
      sem_wait(mutex_write);
      fprintf(output, "%d   : SERF %d  : waits  : %d   : %d\n", ++shm[0], srf_cur_cnt[0], hck_w_cnt[0], srf_w_cnt[0]);
      sem_post(mutex_write);
      if (srf_w_cnt[0] == 4 ) {
         srf_w_cnt[0] = 0;
         isCaptain = true;
         sem_wait(mutex_write);
         fprintf(output, "%d   : SERF %d  : boards  : %d   : %d\n", ++shm[0], srf_cur_cnt[0], hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(serfsQueue);
         sem_post(serfsQueue);
         sem_post(serfsQueue);
         sem_post(serfsQueue);
      }
      else if (srf_w_cnt[0] == 2 && hck_w_cnt[0] >= 2) {
         srf_w_cnt[0] = 0;
         --hck_w_cnt[0];
         --hck_w_cnt[0];
         isCaptain = true;
         sem_wait(mutex_write);
         fprintf(output, "%d   : SERF %d  : boards  : %d   : %d\n", ++shm[0], srf_cur_cnt[0], hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(hackersQueue);
         sem_post(hackersQueue);
         sem_post(serfsQueue);
         sem_post(serfsQueue);
      }
      else {
         sem_post(mutex);
      }
   
   sem_wait(serfsQueue);
barrier->counter[0]++;
if ( *barrier->counter == barrier->threshold ) {
      barrier->counter[0] = 0;
      sem_post(barrier->sem);
      sem_post(barrier->sem);
      sem_post(barrier->sem);
} else {
   sem_wait(barrier->sem);
}

      if(isCaptain) {
         rowBoat();
         sem_wait(getout);
         sem_wait(getout);
         sem_wait(getout);
         sem_wait(mutex_write);
         fprintf(output, "%d   : SERF %d  : captain exits : %d   : %d\n", ++shm[0], process_number, hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(mutex);
      } else {
         sem_wait(mutex_write);
         fprintf(output, "%d   : SERF %d  : member exits : %d   : %d\n", ++shm[0], process_number, hck_w_cnt[0], srf_w_cnt[0]);
         sem_post(mutex_write);
         sem_post(getout);
      }
   //fprintf(output,"end of serfprocess\n");
   exit(0);
}

void serfpop(int gos, int in_sst, barrier_t *barrier) { // generating serfs
   pid_t serfs[gos];
      for (int i=0; i<gos; i++) {
         if ( (serfs[i] = fork()) < 0 ) {
            exit(1);
         } else {
            if ( serfs[i] == 0 ) {
               serf_process(barrier);
               exit(0);
            } else {
               sleep(rand()%(in_sst+1)*0.001);
            }
         } 
      }
      for (int i=0; i<gos; i++) {
         waitpid(serfs[i],NULL,0);
      }
   //fprintf(output,"end of serfpop\n");
   exit(0);
}

void parseargs(int argc, char *argv[]) {
   if ( argc != 7 ) {  // check number of args
      fprintf(stderr,"%s", "Wrong number of arguments.\n");
      exit(1);
   }
   char *ptr = NULL;
   for ( int alpha = 1; alpha < argc; alpha++ )
   {  // check if arguments are numbers
      strtol(argv[alpha], &ptr, 10);
      if ( strcmp(ptr,"")) {
         fprintf(stderr,"%s", "Argument is not a number.\n"); exit(1);
      }
   }
   if ( (strtol(argv[1], &ptr, 10) < 2) || 
      (( strtol(argv[1], &ptr, 10) % 2 ) != 0) ) {
      fprintf(stderr,"%s", "Wrong P argument.\n");
      exit(1);
   }
   for ( int beta = 2; beta < 5; beta++) {
      if ( (strtol(argv[beta], &ptr,10) < 0) || 
            strtol(argv[beta], &ptr, 10) > 2000 ) {
         fprintf(stderr,"%s", "Wrong H,S or R argument.\n");
         exit(1);
      }
   }
   if ( (strtol(argv[5], &ptr, 10) < 20) || 
        (strtol(argv[5], &ptr, 10) > 2000) ) {
      fprintf(stderr,"%s", "Wrong W argument.\n");
      exit(1);
   }
   if ( (strtol(argv[6], &ptr, 10) < 5) ) {
      fprintf(stderr,"%s", "Wrong C argument.\n");
      exit(1);
   }
}
int main(int argc, char *argv[])
//int main(void)
{
   parseargs(argc, argv);

   int pid;
   int hspawns;  // number of hackers spawned  
   int sspawns;  // number of serfs spawned
   int hst;      // hacker spawn time
   int sst;      // serf spawn time

   output = fopen("proj2.out", "w");
   fclose(output);
   output = fopen("proj2.out", "a");

   char *ptr = NULL;
   hspawns = strtol(argv[1], &ptr, 10);
   sspawns = strtol(argv[1], &ptr, 10);
   hst = strtol(argv[2], &ptr, 10);
   sst = strtol(argv[2], &ptr, 10);
   voyage = (int) (long) strtol(argv[4], &ptr, 10);

   setbuf(stdout,NULL);
   setbuf(stderr,NULL);
   setbuf(output,NULL);

   barrier_t barrier;

   init_all(&barrier);
   open_all(&barrier);

   if ((pid = fork()) < 0) {
       fprintf(stderr, "fork\n");
       exit(2);
   }
   if (pid == 0) { // child
       hackerborn(hspawns, hst, &barrier);
       exit(0);
   } else {

      if ((pid = fork()) < 0) {
         fprintf(stderr, "fork\n");
         exit(2);
      }
      if (pid == 0 ) { // child
         serfpop(sspawns, sst, &barrier);
         exit(0);
      }  
   }
   sem_close(mutex);
   sem_close(hackersQueue);
   sem_close(serfsQueue);
   sem_close(getout);
   sem_close(mutex_write);
   
   munmap(shm, sizeof(int));           // line/action count
   munmap(hck_cur_cnt, sizeof(int));   // hacker current count
   munmap(srf_cur_cnt, sizeof(int));   // serf current count
   munmap(hck_w_cnt, sizeof(int));     // hacker waiting count
   munmap(srf_w_cnt, sizeof(int));     // serf waiting count

   for (int i = 0; i < 2; i++) {
      wait(NULL);
   }
   
   unlink_all();
   fclose(output);
   return 0;
}
