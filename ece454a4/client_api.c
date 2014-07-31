/* 
 * Mahesh V. Tripunitara
 * University of Waterloo
 * A dummy implementation of the functions for the remote file
 * system. This file just implements those functions as thin
 * wrappers around invocations to the local filesystem. In this
 * way, this dummy implementation helps clarify the semantics
 * of those functions. Look up the man pages for the calls
 * such as opendir() and read() that are made here.
 */
#include "ece454_fs.h"
#include <string.h>
#include "simplified_rpc/ece454rpc_types.h"
#include "fsOtherIncludes.h"
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>

struct fsDirent dent;

const char *serverIpOrDomainName;
unsigned int serverPort;
int folderNameSize;

struct mounted_server_list {
    char *serverIpOrDomainName;
    int serverPort;
    char *localFolder; 
    int folderNameSize;    
    struct mounted_server_list *next;
};

struct mounted_server_list *mounted_server_head = NULL;
struct mounted_server_list *mounted_server_tail = NULL;


void addServerList(char *srvIpOrDomName, int srvPort, char *localFolderName, int folderSize){
   struct mounted_server_list* new_mounted_server;
   new_mounted_server = malloc(sizeof(struct mounted_server_list));

   new_mounted_server->serverIpOrDomainName = srvIpOrDomName;
   new_mounted_server->serverPort = srvPort;
   new_mounted_server->localFolder = localFolderName;
   new_mounted_server->folderNameSize = folderSize;
   new_mounted_server->next = NULL;


   if (mounted_server_head == NULL){
       mounted_server_head = new_mounted_server;
       mounted_server_tail = mounted_server_head;
   }
   else {
       mounted_server_tail->next = new_mounted_server;
       mounted_server_tail = new_mounted_server;
   }
   

}

void removeServerEntry(const char *localFolderName){

    struct mounted_server_list *entry;
    struct mounted_server_list *prev_entry;

    entry = malloc(sizeof(struct mounted_server_list));
    prev_entry = malloc(sizeof(struct mounted_server_list));    
    entry = mounted_server_head;
    prev_entry = NULL;
    int counter = 0;
    // traverse through entries to find the server
    while(entry != NULL){
        int compareResult = strcmp(localFolderName,entry->localFolder);
        //printf("\t comparing values %s with %s\n",localFolderName, entry->localFolder);         
        if (compareResult == 0){
           printf("\t foundValues values %s with %s\n",localFolderName, entry->localFolder);         
           if (counter == 0){
              // remove head
              prev_entry = mounted_server_head;
              if (entry->next != NULL){
                 printf("head Removed \n");
                 entry = entry->next;
                 mounted_server_head = entry;
                 break;
              }
              else{
                 mounted_server_head = NULL;
                 mounted_server_tail = NULL;
                 printf("removed the only entry in the list \n");
              }

           }
           else{
              //entry = NULL;
              prev_entry->next = entry->next;
              
              free(entry);
              break;
           }
           
           
        }
        counter = counter+1;
        prev_entry = entry;
        entry = entry->next; 
    }
    
}

struct mounted_server_list *findEntry (const char *localFolderName){
   struct mounted_server_list *entry;
   entry = malloc(sizeof(struct mounted_server_list));
   
   entry = mounted_server_head;
   char *buf = malloc(256);
   while(entry != NULL){
     // printf("localFolderName=%s  and comparign value is %s and next value is \n",localFolderName,entry->localFolder);
      strcpy(buf,entry->localFolder);
      int i;
      int compareResul =0;
      printf("comparing localFolderName %s , entry %s \n", localFolderName, buf);
      for (i=0; i<=entry->folderNameSize; i++){
        //  printf("\t\t printing ith characters of localeNTRY %c and %c of serverEntry\n",localFolderName[i],buf[i]);
          char sL = localFolderName[i];
          if (i == entry->folderNameSize && sL != '\0'){
             char nC = '/';
             
          //   if (strcmp(&localFolderName[i],&nC)!=0){
             if (sL != nC){
                compareResul = 1;
             } 
          }
          else{
              if (buf[i] != localFolderName[i]){
             // if (strcmp(&sC,&sL) != 0){ 
              //printf("%c %c\t",sC, sL);
                compareResul = 1;
               }
          }
      }
      
      //int compareResult = strncmp(localFolderName,entry->localFolder,(entry->localFolder)+1);
      if (compareResul == 0  ){
         //printf ("\t\t found matched %s with %s\n",localFolderName,entry->localFolder);
         return entry;
      }
      entry = entry->next;
   }
   printf("findEntry(), %s not fount\n",localFolderName); 
   
   return NULL;
}

void setRpcInformation (const char *localFolderName){
   struct mounted_server_list *entry = malloc(sizeof(struct mounted_server_list));
   entry  = findEntry(localFolderName);
   if (entry == NULL){exit(1);} 
   serverIpOrDomainName = entry->serverIpOrDomainName;
   serverPort = entry->serverPort;
   //localFolder = entry->localFolder;
   folderNameSize = entry->folderNameSize; 
}

void printServerList(){
   //printf("printServerList(), enter\n");
   struct mounted_server_list* list_entry = malloc(sizeof(struct mounted_server_list));
   
   list_entry = mounted_server_head;

   while(list_entry != NULL){
       printf("mounted_serverList(), iporDom = %s  port = %i folder = %s size = %i \n",list_entry->serverIpOrDomainName, list_entry->serverPort,list_entry->localFolder, list_entry->folderNameSize);
       list_entry = list_entry->next;
   }
   
   //free(list_entry);
   printf("printServerList(), exit\n");
}

int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {

    struct stat sbuf;
    int mountErrNo;

    serverIpOrDomainName = srvIpOrDomName;
    serverPort = srvPort;
    //localFolder = localFolderName;    
    int folderSize = strlen(localFolderName);
    printf("foldernamesize: %i\n",strlen(localFolderName));
    //printServerList();
    addServerList((char *)srvIpOrDomName,srvPort,(char *)localFolderName, folderSize);  
    //printServerList();
    //printf("call rpc info next\n");  
     //setRpcInformation(localFolderName);
    //printf("exit rpc info\n");
    //removeServerEntry((char *)localFolderName);

//    stat(localFolderName, &sbuf);
    int  dummyCheckSum = 1;
   // printf("enter fsMount right before rpc call \n");

    return_type ans = make_remote_call(serverIpOrDomainName,serverPort, "fsMount", 1, sizeof(int), (void *)(&dummyCheckSum));
    
    printf("fsMount right after rpc call \n"); 
    int return_val = *(int *)(ans.return_val);
   // printf ("return value generated \n");    
    if (return_val < 0){
       errno = ENOTDIR;
       return -1;
    }
    printf ("return value was correct \n"); 
    return 0;
}

int fsUnmount(const char *localFolderName) {
    
    int dummyCheckSum = 1;
    removeServerEntry(localFolderName); 
     return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsUnmount", 1, sizeof(int), (void *)(&dummyCheckSum));
     int return_val = (*(int *)(ans.return_val));
     if (return_val != -1){
        return return_val;}
     else{
        errno = ENOTDIR;

        return -1;
     }
     //return 0;
}

FSDIR* fsOpenDir(const char *folderName) {
     printf("fsOpenDir(), enter with folderName =%s\n",folderName);
     setRpcInformation(folderName);

     char *dirName = malloc(strlen(folderName)-folderNameSize+1);
     if (strlen(folderName) == folderNameSize) {
        dirName[0] = '\0';
     } else {
         strcpy(dirName,(folderName+folderNameSize));
     }
          
     printf("new directory name is %s, the folderNameSize = %i \n",dirName,folderNameSize);
     return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsOpenDir",1,strlen(dirName)+1, dirName);
     
     printf("returnOpenDir value from server.. need to be parsed \n");
    
     //int return_val = (*(int *)(ans.return_val));
     int return_size = (int)(ans.return_size);
     if (return_size == 0){
        printf("fsOpenDir(), return size is null \n");
        return NULL;
     }
     //int return_val = 1;
    int return_val = (*(int *)(ans.return_val));
     printf("returnValue is correct in OpenDir \n");
    
     FSDIR* ptrDirFolder = malloc(sizeof(FSDIR));

     printf("FSDIR dirFolder variable Created \n");
     ptrDirFolder->num = return_val;
     ptrDirFolder->dir = NULL; 
     if (return_val == 0){
              return NULL;}
     

     else {
        printf("returning ptr to dir Folder: the returned value is %i    and fsDirptr value is %i \n",return_val, ptrDirFolder->num); 
        return ptrDirFolder;
     } 
     
}

int fsCloseDir(FSDIR *folder) {
    printf("fsCloseDir(), enter fsCloseDir\n");    
    //int retur_val;
    int  fsDirPtr = folder->num;
    printf("fsCloseDir(): folder num value is: %i\n",fsDirPtr);
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsCloseDir",1,sizeof(int),(void *)(&fsDirPtr));
    int return_val = (*(int *)(ans.return_val));
    printf("fsCloseDir(), return_val is %i\n",return_val);
    if (return_val == 0){
       return return_val;
    }
    else {
       return -1;
    }
    //return(closedir(folder));
    return 0;
}


struct fsDirent *fsReadDir(FSDIR *folder) {
    printf("fsReadDir(), entering function \n");
    int fsDirPtrNum = folder->num;
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsReadDir",1, sizeof(int), (void *)(&fsDirPtrNum));
    
    struct fsDirent *return_val;
    return_val  = (struct fsDirent *)(ans.return_val);
    
    //int return_val = (*(int *)(ans.return_val));
    //printf("readDir(),  return ent value = %i \n",return_val); 
    /*
    char* buf = malloc(sizeof(ans.return_size));
    memcpy(buf,(char *)(ans.return_val));
   
    char* buf_ptr = buf;
    
    // getSizeOfEntityVal
    int entityNameSize;
    memcpy(&entityNameSize, buf_ptr, sizeof(int));
    
    char *entityName = 
    */
    if (return_val == NULL ){
        printf("fsReadDir(), error or end of folder found\n");
        
    }
    
 

  //  printf("fsReadDir(), exiting function\n");
    return return_val;;
}

int fsOpen(const char *fname, int mode) {
    
    int return_val = 0;    
    setRpcInformation(fname);
    char *dirName = malloc(strlen(fname)-folderNameSize+1);
    if (strlen(fname) == folderNameSize) {
        dirName[0] = '\0';
    } else {
        strcpy(dirName, (char *)(fname+folderNameSize+1));
    }
    
    printf("fsOpen(), entering client call function,\n\t folderNameSize=%i dirName %s and fname: %s\n",folderNameSize,dirName,fname);
    do {
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsOpen",2,strlen(dirName)+1,dirName, sizeof(int),(void *)(&mode));
    
    // return open file structure signatue
     return_val =  (*(int *)(ans.return_val));
     if(return_val == -2){sleep(1);}
    } while (
       return_val == -2
    );
    printf("fsOpen(), client, returned value is : %i\n",return_val);
    
    if (return_val == -1){
       errno = ENOENT;
       return return_val;
    }
    else {
       return return_val;
    }
    //return(open(fname, flags, S_IRWXU));
}

int fsClose(int fd) {
    printf("fsClose(), entering fsClose function with int fd as: %i\n",fd);
//    close(fd); // close on client side;
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsClose",1,sizeof(int),(void *)(& fd));

    
    //return(close(fd));
    
    // return server side fd
   
    int return_val =  (*(int *)(ans.return_val));
    printf("fsClose(), returned value is: %i\n",return_val);
    if (return_val != -1){
        return return_val;
    }
    else {
        errno = EMFILE;
        return -1;
    }

}

int fsRead(int fd, void *buf, const unsigned int count) {
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsRead",2,sizeof(int),(void *)(&fd), sizeof(int), (void *)(&count));
    int return_val = ans.return_size;
    printf("fsRead(), return_val = %i\n",return_val);
    memcpy(buf, ans.return_val,ans.return_size);
    printBuf(buf,return_val);
    //int local_red_val = read(fd, buf, (size_t)count);

    if (return_val != -1){
       return return_val;
    }
    else{
       return -1;
    }
}

int fsWrite(int fd, const void *buf, const unsigned int count) {

    // attempt to write to the server
    printf("fsWrite(), entering function with fd %i\n", fd); 
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsWrite",3,sizeof(int),(void *)(&fd),(int) count, (void *)(buf), sizeof(int), (void *)(&count));
     

    int return_val = (*(int *) ( ans.return_val));


 //   write(fd, buf, (size_t)count);
    printf("fsWrite(), return value after rpc is %i\n",return_val);    
    if (return_val != -1){
        return return_val;
    } 
    else {
        return -1;
    }
}

int fsRemove(const char *name) {
    printf("fsRemove(), enter with name %s\n",name);
    char *dirName = malloc(strlen(name)-folderNameSize);
    strcpy(dirName, (char *)(name+folderNameSize+1));
    int return_val = 0;
    do{
    printf("sending remove request\n");
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsRemove",1,strlen(name)+1, dirName);

    return_val = (*(int *)(ans.return_val));
    if (return_val == -2){
          sleep(1);
     }
    }
    while(
       return_val == -2
    );
//   int local_remove = remove(name);

   if (return_val == 0){
       return return_val;
   }
   else {
       errno = EMFILE;
       return -1;
   }
   
}
