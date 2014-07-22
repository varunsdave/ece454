#include <stdio.h>
#include <sys/stat.h>

main(){
   printf("enter stat function\n");
   struct stat sbuf;

   const char *fileName = "../simplified_rpc";
   stat(fileName,&sbuf);

   int size = sbuf.st_size;

   printf("size is: %i, dev is: \n", size, sbuf.st_dev);

   if (S_ISDIR(sbuf.st_mode)){
     printf("This is a directory\n");
   }
   else{

     printf("This is not a directory\n");
   } 
   return 0;

}
