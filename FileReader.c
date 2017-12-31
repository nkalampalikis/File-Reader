
/* Developed by Nikolaos Kalampalikis

   Project 4 for CS3013 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>

// ----- Global Variables ----- //
#define BUF_SIZE 1024
#define MAXTHREAD 15

// ----- Structs ----- //
struct  TotalValues{
  int BadFiles; //total number of “bad files.”
  int NumOfDirectories; //total number of directories
  int NumOfRegularFiles;  //total number of “regular” files
  int NumOfSpecialFiles; //total number of “special” files
  off_t NumOfRegularFilesBytes; // total number of bytes used by regular files
  int NumOfRegularFilesAllText; //total number of regular files that contain all text
  off_t NumOfTextFilesBytes;  //total number of bytes used by text files
};

// ----- Function Protypes ----- //
int PrintStats(struct  TotalValues TempTotal);
int IsText(char* FileName);
void *ThreadMain(void* arg);
int CopyToGlobal(struct  TotalValues *TempTotal);

// ----- Semaphore ----- //
sem_t sem_p;

// ----- Global Structs ----- //
struct  TotalValues Final;

/*

  IsText function return true if the file is text othewise it returns 0

*/
int IsText(char* FileName){

   int fdIn =0,num =0,i;
   char File[BUF_SIZE];
   //Open the File
    if ((fdIn = open(FileName, O_RDONLY)) < 0) {
	   return 0;
    }
    // Pass file info into an array and check if its elements are printable or space
    while ((num = read(fdIn, File, BUF_SIZE)) > 0) {
      for(i =0; i< num;i++){
          if (!(isspace(File[i])|isprint(File[i]))){
            return 0;
          }//End if
      }// End for
    }//End while
    // Close file
    if (fdIn > 0){
      close(fdIn);
    }
    return 1;
}
/*

  PrintStats function that prints the stats of the total struct

*/
int PrintStats(struct  TotalValues TempTotal){
  printf("The total number of “bad files” is %d\n", TempTotal.BadFiles);
  printf("The total number of directories is %d\n", TempTotal.NumOfDirectories);
  printf("The total number of “regular” files is %d\n", TempTotal.NumOfRegularFiles);
  printf("The total number of “special” files is %d\n", TempTotal.NumOfSpecialFiles);
  printf("The total number of bytes used by regular files is %zu\n", TempTotal.NumOfRegularFilesBytes);
  printf("The total number of regular files that contain all text is %d\n", TempTotal.NumOfRegularFilesAllText);
  printf("The total number of bytes used by text files is %zu\n", TempTotal.NumOfTextFilesBytes);
  return 0;
}// End PrintStats
/*

  Copy to global function

*/
int CopyToGlobal(struct  TotalValues *TempTotal){
  sem_wait(&sem_p);
  Final.BadFiles += TempTotal->BadFiles;
  Final.NumOfDirectories += TempTotal->NumOfDirectories;
  Final.NumOfRegularFiles += TempTotal->NumOfRegularFiles;
  Final.NumOfSpecialFiles += TempTotal->NumOfSpecialFiles;
  Final.NumOfRegularFilesBytes += TempTotal->NumOfRegularFilesBytes;
  Final.NumOfRegularFilesAllText += TempTotal->NumOfRegularFilesAllText;
  Final.NumOfTextFilesBytes += TempTotal->NumOfTextFilesBytes;
  sem_post(&sem_p);
  return 0;
}
/*

  Main routine that will be executed by each thread to process the stats of files

*/
void *ThreadMain(void* arg) {
  char *FileName = (void *) arg;
  struct  TotalValues TempTotal ={0,0,0,0,0,0,0};
  struct stat TempStat;
  // Detecting the Bad Files
  if (stat(FileName,&TempStat) <0){
    TempTotal.BadFiles++;
    // Copy all the Stats to the global
    CopyToGlobal(&TempTotal);
    // Exit thread when done
    pthread_exit(0);
  }// End if
  //Check is the file is regular
  if ((TempStat.st_mode & S_IFMT) == S_IFREG) {
    TempTotal.NumOfRegularFiles++;
    TempTotal.NumOfRegularFilesBytes += TempStat.st_size;
    // Check if a file is text or not
    if(IsText(FileName)>0){
      TempTotal.NumOfRegularFilesAllText++;
      TempTotal.NumOfTextFilesBytes += TempStat.st_size;
    }// End if
  //Check is the file is a directory
  }else if ((TempStat.st_mode & S_IFMT) == S_IFDIR) {
    TempTotal.NumOfDirectories++;
  // If the file is neither regular nor a directory then it is special
  }else{
    TempTotal.NumOfSpecialFiles++;
  }// End if
  // Copy all the Stats to the global
  CopyToGlobal(&TempTotal);
  // Exit thread when done
  pthread_exit(0);
}
/*

  Main function of the program

*/
int main(int argc, char const *argv[]) {

  // TIMER
  struct rusage *myrusage = malloc(sizeof (struct rusage));
  struct timeval start, end;
  gettimeofday(&start, NULL);

  //Number of threads value extracted from input
  int NumofThreads;
  // Getting the command line input
  NumofThreads = atoi(argv[2]); // Number of threads to be created
  // Check cmd input
  if ((argc >3 )|(NumofThreads> MAXTHREAD)){
      printf("Wrong input\n");
      exit(0);
  }// End if
// ----- MULTI THREADED ARCHITECTURE -----//
  if (argv[1] != NULL){
    const char *Arg2 = argv[1];

    if (strcmp(Arg2,"thread")== 0){
      printf("MULTI THREADED ARCHITECTURE\n");

      // Initialize values of the global struct
      Final.BadFiles =0;
      Final.NumOfDirectories =0;
      Final.NumOfRegularFiles =0;
      Final.NumOfSpecialFiles =0;
      Final.NumOfRegularFilesBytes =0;
      Final.NumOfRegularFilesAllText =0;
      Final.NumOfTextFilesBytes =0;

      char Input[BUF_SIZE];
      // Initialize semaphore
      if (sem_init(&sem_p, 0, 1) < 0) {
        perror("sem_init");
        exit(1);
      }// End if

      // Define thread IDs
      pthread_t threads[NumofThreads + 1];

      int i =0;
      while (fgets(Input, BUF_SIZE, stdin) != NULL) {
        //Getting the name of a file
        char *FileName;
        FileName = strtok(Input, " \t\r\n\a");
        char* token = malloc(strlen(FileName));
        strcpy(token,FileName);

        i++;// Plus one thread

        //Thread creation
        if (pthread_create(&threads[i], NULL, ThreadMain,(void *) token) != 0) {
          perror("pthread_create");
          exit(1);
        }//End if
        // Check if it reaches the Max threads number
        if (i == NumofThreads){
          // Wait for all threads to finish
          for(int j = 1 ; j <= NumofThreads ; j++) {
            (void)pthread_join(threads[j], NULL);
          }// End for
          i=0;
        }// End if
      }//End while
      // Join the left over threads
      if(i>0){
        for(int k = 1 ; k<= i; k++) {
          (void)pthread_join(threads[k], NULL);
        }//End for
      }// End if
      // Clean the semaphore
      (void)sem_destroy(&sem_p);

      // Print all the Stats
      PrintStats(Final);

    }// End MULTI THREADED ARCHITECTURE

    // ----- SERIAL ARCHITECTURE -----//
  }else{
    printf("SERIAL ARCHITECTURE\n");

    struct  TotalValues TempTotal ={0,0,0,0,0,0,0};
    struct stat TempStat;
    char Input[BUF_SIZE];

    // Read from stdin and process as you read every file
    while (fgets(Input, BUF_SIZE, stdin) != NULL) {

      //Getting the name of a file
      char *FileName;
      FileName = strtok(Input, " \t\r\n\a");
      // Detecting the Bad Files
      if (stat(FileName,&TempStat) <0){
        TempTotal.BadFiles++;
        continue;
      }// End if

      //Check is the file is regular
      if ((TempStat.st_mode & S_IFMT) == S_IFREG) {
        TempTotal.NumOfRegularFiles++;
        TempTotal.NumOfRegularFilesBytes += TempStat.st_size;

        // Check if a file is text or not
        if(IsText(FileName)>0){
          TempTotal.NumOfRegularFilesAllText++;
          TempTotal.NumOfTextFilesBytes += TempStat.st_size;
        }// End if

      //Check is the file is a directory
      }else if ((TempStat.st_mode & S_IFMT) == S_IFDIR) {
        TempTotal.NumOfDirectories++;
      // If the file is neither regular nor a directory then it is special
      }else{
        TempTotal.NumOfSpecialFiles++;
      }// End if
    }// End of while

    // Print all the Stats
    PrintStats(TempTotal);

  }// End SERIAL ARCHITECTURE

  //END TIMER
  gettimeofday(&end, NULL);
  getrusage(RUSAGE_SELF,myrusage);

  printf("The amount of CPU  user time used was %ld milliseconds.\n", ((myrusage->ru_utime.tv_sec * 1000000 + myrusage->ru_utime.tv_usec)/1000));
  printf("The amount of CPU  system time used was %ld milliseconds.\n", ((myrusage->ru_stime.tv_sec * 1000000 + myrusage->ru_stime.tv_usec)/1000));
  printf("The elapsed wall-clock time was %ld milliseconds.\n",(((end.tv_sec * 1000000 + end.tv_usec)/1000)- ((start.tv_sec * 1000000 + start.tv_usec)/1000)));
  return 0;
}
