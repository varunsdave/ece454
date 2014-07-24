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

struct fsDirent dent;

const char *serverIpOrDomainName;
unsigned int serverPort;
const char *localFolder;



int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {

    struct stat sbuf;
    int mountErrNo;

    serverIpOrDomainName = srvIpOrDomName;
    serverPort = srvPort;
    localFolder = localFolderName;    
    
    stat(localFolderName, &sbuf);
    int  dummyCheckSum = 1;
    printf("enter fsMount right before rpc call \n");

    return_type ans = make_remote_call(serverIpOrDomainName,serverPort, "fsMount", 1, sizeof(int), (void *)(&dummyCheckSum));
    
    printf("fsMount right after rpc call \n"); 
    int return_val = *(int *)(ans.return_val);
    printf ("return value generated \n");    
    if (return_val < 0){
       errno = ENOTDIR;
       return -1;
    }
    printf ("return value was correct \n"); 
    return 0;
}

int fsUnmount(const char *localFolderName) {
    
    //int dummyCheckSum = 1;
    
     return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsUnmount", 1, strlen(localFolderName)+1, localFolderName);
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
     return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsOpenDir",1,strlen(folderName)+1, folderName);
     
     printf("returnOpenDir value from server.. need to be parsed \n");
    
     int return_val = (*(int *)(ans.return_val));
     //int return_val = 1;
     printf("returnValue is correct in OpenDir \n");
     FSDIR dirFolder;
     FSDIR *ptrDirFolder;

     ptrDirFolder = &dirFolder;

     printf("FSDIR dirFolder variable Created \n");
     ptrDirFolder->num = return_val;
     ptrDirFolder->dir = NULL; 
     if (return_val == 0){
              return NULL;}

     else {
        printf("returning ptr to dir Folder: the returned value is %i    and fsDirptr value is %i \n",return_val, ptrDirFolder->num); 
        return ptrDirFolder;
     } 
     
     //return NULL;
     //opendir(folderName);
    // int return_val = (*(int *)(ans.return_val));
    // if (return_val !=-1){
        //return (int)return_val;
     ///}
 //    else {
  //      errno = ENOTDIR;
  //      return -1;
   //  }
     //opendir(folderName);
}

int fsCloseDir(FSDIR *folder) {
    printf("fsCloseDir(), enter fsCloseDir\n");    
    //int retur_val;
    int  fsDirPtr = folder->num;
    printf("fsCloseDir(): Enter: %i\n",fsDirPtr);
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort, "fsCloseDir",1,sizeof(int),(void *)(&fsDirPtr));
    int return_val = (*(int *)(ans.return_val));
    printf("fsCloseDir(), return_val is %i",return_val);
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
    const int initErrno = errno;
    struct dirent *d = readdir(folder->dir);

    if(d == NULL) {
	if(errno == initErrno) errno = 0;
	return NULL;
    }

    if(d->d_type == DT_DIR) {
	dent.entType = 1;
    }
    else if(d->d_type == DT_REG) {
	dent.entType = 0;
    }
    else {
	dent.entType = -1;
    }

    memcpy(&(dent.entName), &(d->d_name), 256);
    return &dent;
}

int fsOpen(const char *fname, int mode) {
    int flags = -1;

    if(mode == 0) {
	flags = O_RDONLY;
    }
    else if(mode == 1) {
	flags = O_WRONLY | O_CREAT;
    }
    
    // make rpc to open file structure and return the rpc value

    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsOpen",2,strlen(fname)+1,fname, sizeof(int),(void *)(&flags));
    
    // return open file structure signatue
    int return_val =  (*(int *)(ans.return_val));

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

    close(fd); // close on client side;
    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsClose",1,sizeof(int),(void *)(& fd));

    
    //return(close(fd));
    
    // return server side fd
   
    int return_val =  (*(int *)(ans.return_val));
    
    if (return_val != -1){
        return return_val;
    }
    else {
        errno = EMFILE;
        return -1;
    }

}

int fsRead(int fd, void *buf, const unsigned int count) {
    return_type ans = make_remote_call (serverIpOrDomainName, serverPort,"fsRead",3,sizeof(int),(void *)(&fd), count, &buf, sizeof(unsigned int), count);
    int return_val = (*(int *) (ans.return_val));

  
    int local_red_val = read(fd, buf, (size_t)count);

    if (return_val != -1){
       return return_val;
    }
    else{
       return -1;
    }
}

int fsWrite(int fd, const void *buf, const unsigned int count) {

    // attempt to write to the server

    return_type ans = make_remote_call (serverIpOrDomainName, serverPort,"fsWrite",3,sizeof(int),(void *)(&fd),(int) count, &buf, sizeof (unsigned int), count);
    

    int return_val = (*(int *) ( ans.return_val));


    write(fd, buf, (size_t)count);
    
    if (return_val != -1){
        return return_val;
    } 
    else {
        return -1;
    }
}

int fsRemove(const char *name) {

    return_type ans = make_remote_call(serverIpOrDomainName, serverPort,"fsRemove",1,strlen(name)+1, name);

   int return_val = (*(int *)(ans.return_val));
   
   int local_remove = remove(name);

   if (return_val != -1){
       return return_val;
   }
   else {
       errno = EMFILE;
       return -1;
   }
   
}
