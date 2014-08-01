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
// create a struct for linked list entries that will hold 
// information about all the mounted servers with their alian
// ip address and port number
struct mounted_server_list {
    char *serverIpOrDomainName;
    int serverPort;
    char *localFolder; 
    int folderNameSize;    
    struct mounted_server_list *next;
};

struct mounted_server_list *mounted_server_head = NULL;
struct mounted_server_list *mounted_server_tail = NULL;

// a function to insert mounted server list entry into a linked list
// creates a list of none exist
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
// removes server entry on unmont call from linked 
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
         
        if (compareResult == 0){
           if (counter == 0){
              // remove head
              prev_entry = mounted_server_head;
              if (entry->next != NULL){
                 entry = entry->next;
                 mounted_server_head = entry;
                 break;
              }
              else{
                 mounted_server_head = NULL;
                 mounted_server_tail = NULL;
              }
           }
           else{
              
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

// used to find  the entry containint mounted server, ip addres,, port
// etc. given a pathname defined by localFolderName
// returns a mounted_server_list item
struct mounted_server_list *findEntry (const char *localFolderName){
   struct mounted_server_list *entry;
   entry = malloc(sizeof(struct mounted_server_list));
   
   entry = mounted_server_head;
   char *buf = malloc(256);
   // search each entry in the linked list and comapre the first part
   // with alias to find matching entry. returns null if no matching entry 
   // found
   while(entry != NULL){
      strcpy(buf,entry->localFolder);
      int i;
      int compareResul =0;
      for (i=0; i<=entry->folderNameSize; i++){
          char sL = localFolderName[i];
          if (i == entry->folderNameSize && sL != '\0'){
             char nC = '/';
             if (sL != nC){
                compareResul = 1;
             } 
          }
          else{
              if (buf[i] != localFolderName[i]){
                compareResul = 1;
               }
          }
      }
      if (compareResul == 0  ){
         return entry;
      }
      entry = entry->next;
   }
   
   return NULL;
}
 
// set global variables for rpc calls given a localFolderName which represents
// a path to a directory or a file
void setRpcInformation (const char *localFolderName){
   struct mounted_server_list *entry = malloc(sizeof(struct mounted_server_list));
   entry  = findEntry(localFolderName); //find the mounted server based on path
   if (entry == NULL){exit(1);} 
   serverIpOrDomainName = entry->serverIpOrDomainName;
   serverPort = entry->serverPort;
   folderNameSize = entry->folderNameSize; 
}
// prints the list containing all server entries
void printServerList(){
   struct mounted_server_list* list_entry = malloc(sizeof(struct mounted_server_list));
   
   list_entry = mounted_server_head;

   while(list_entry != NULL){
       list_entry = list_entry->next;
   }
   
}

int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {


    serverIpOrDomainName = srvIpOrDomName;
    serverPort = srvPort;
    int folderSize = strlen(localFolderName);
    addServerList((char *)srvIpOrDomName,srvPort,(char *)localFolderName, folderSize);  
    // a dummy to see if the server to be mounted can be connected with
    // after rpc call return value has to be atleast dummyCheckSum
    int  dummyCheckSum = 1;

    return_type ans = make_remote_call(serverIpOrDomainName,serverPort, "fsMount", 1, sizeof(int), (void *)(&dummyCheckSum));
    
    int return_val = *(int *)(ans.return_val);
    if (return_val < 0){
       int err = ans.return_errno;
       errno = err;
       return -1;
    }
    return 0;
}

int fsUnmount(const char *localFolderName) {
    
    int dummyCheckSum = 1;
    removeServerEntry(localFolderName); 
     return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsUnmount", 1, sizeof(int), (void *)(&dummyCheckSum));
     int return_val = (*(int *)(ans.return_val));
     if (return_val == dummyCheckSum){
        return 0;
     }
     else {
       int err = ans.return_errno;
       errno = ECOMM;

        return -1;
     }
}

FSDIR* fsOpenDir(const char *folderName) {
     setRpcInformation(folderName);

     char *dirName = malloc(strlen(folderName)-folderNameSize+1);
     if (strlen(folderName) == folderNameSize) {
        dirName[0] = '\0';
     } else {
         strcpy(dirName,(folderName+folderNameSize));
     }
          
     return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsOpenDir",1,strlen(dirName)+1, dirName);
    
     int return_size = (int)(ans.return_size);
     if (return_size == 0){
        return NULL;
     }
    int return_val = (*(int *)(ans.return_val));
    
     FSDIR* ptrDirFolder = malloc(sizeof(FSDIR));

     ptrDirFolder->num = return_val;
     ptrDirFolder->dir = NULL; 
     if (return_val == 0){
              return NULL;}
     

     else {
        return ptrDirFolder;
     } 
     
}

int fsCloseDir(FSDIR *folder) {
    int  fsDirPtr = folder->num;
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsCloseDir",1,sizeof(int),(void *)(&fsDirPtr));
    int return_val = (*(int *)(ans.return_val));
    if (return_val == 0){
       return return_val;
    }
    else {

       int err = ans.return_errno;
       errno = err;
       return -1;
    }
    return 0;
}


struct fsDirent *fsReadDir(FSDIR *folder) {
    int fsDirPtrNum = folder->num;
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsReadDir",1, sizeof(int), (void *)(&fsDirPtrNum));
    
    struct fsDirent *return_val;
    return_val  = (struct fsDirent *)(ans.return_val);
    
    if (return_val == NULL ){
        
       int err = ans.return_errno;
       errno = err;
    }
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
    
    return_type ans;    
    do {
        ans = make_remote_call(serverIpOrDomainName, serverPort,"fsOpen",2,strlen(dirName)+1,dirName, sizeof(int),(void *)(&mode));
    
        return_val =  (*(int *)(ans.return_val));
     
        if(return_val == -2){sleep(1);}
    } while (return_val == -2);

    
    if (return_val == -1){
       int err =(ans.return_errno);
       errno = err;
       return return_val;
    }
    else {
       return return_val;
    }
}

int fsClose(int fd) {
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsClose",1,sizeof(int),(void *)(& fd));

    int return_val =  (*(int *)(ans.return_val));
    if (return_val != -1){
        return return_val;
    }
    else {
       int err = ans.return_errno;
       errno = err;
        return -1;
    }

}

int fsRead(int fd, void *buf, const unsigned int count) {
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsRead",2,sizeof(int),(void *)(&fd), sizeof(int), (void *)(&count));
    int return_val = ans.return_size;
    memcpy(buf, ans.return_val,ans.return_size);
    printBuf(buf,return_val);

    if (return_val != -1){
       return return_val;
    }
    else{
       int err = ans.return_errno;
       errno = err;
       return -1;
    }
}

int fsWrite(int fd, const void *buf, const unsigned int count) {

    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsWrite",3,sizeof(int),(void *)(&fd),(int) count, (void *)(buf), sizeof(int), (void *)(&count));

    int return_val = (*(int *) ( ans.return_val));

    if (return_val != -1){
        return return_val;
    } 
    else {
       int err = ans.return_errno;
       errno = err;
        return -1;
    }
}

int fsRemove(const char *name) {
    char *dirName = malloc(strlen(name)-folderNameSize);
    strcpy(dirName, (char *)(name+folderNameSize+1));
    int return_val = 0;
    return_type ans;
    do{
    ans = make_remote_call(serverIpOrDomainName, serverPort,"fsRemove",1,strlen(name)+1, dirName);

    return_val = (*(int *)(ans.return_val));
    if (return_val == -2){
          sleep(1);
     }
    }
    while(
       return_val == -2
    );

   if (return_val == 0){
       return return_val;
   }
   else {
       int err = ans.return_errno;
       errno = err;
       return -1;
   }
   
}
