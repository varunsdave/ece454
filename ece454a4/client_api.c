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

int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {
    struct stat sbuf;
    int mountErrNo;
    stat(localFolderName, &sbuf);
    int dummyCheckSum = 1;
    return_type ans = make_remote_call("127.0.0.1",10003, "fsMount", 1, sizeof(int), dummyCheckSum);
     
    int return_val = (*(int *)(ans.return_val));
    
    if (return_val < 0){
       errno = ENOTDIR;
       return -1;
    }
    
    if(S_ISDIR(sbuf.st_mode)){
        return 0;
    }
    else{
        mountErrNo = -1;

        errno = ENOTDIR;
        return -1;
    }

}

int fsUnmount(const char *localFolderName) {
    return_type ans = make_remote_call("127.0.0.1",10003, "fsUnmount", 1, strlen(localFolderName)+1, localFolderName);
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
    // return_type ans = make_remote_call("127.0.0.1",10003,"fsOpen",1,strlen(folderName)+1, folderName);
     
     opendir(folderName);
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
    
    return(closedir(folder));
}


struct fsDirent *fsReadDir(FSDIR *folder) {
    const int initErrno = errno;
    struct dirent *d = readdir(folder);

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

    return_type ans = make_remote_call("127.0.0.1",10003,"fsOpen",2,strlen(fname)+1,fname, sizeof(int),(void *)(&flags));
    
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
    return_type ans = make_remote_call("127.0.0.1",10003,"fsClose",1,sizeof(int),(void *)(& fd));

    
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
    return_type ans = make_remote_call ("127.0.0.1",10003,"fsRead",3,sizeof(int),(void *)(&fd), count, &buf, sizeof(unsigned int), count);
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

    return_type ans = make_remote_call ("127.0.0.1",10003,"fsWrite",3,sizeof(int),(void *)(&fd),(int) count, &buf, sizeof (unsigned int), count);
    

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

    return_type ans = make_remote_call("127.0.0.1",10003,"fsRemove",1,strlen(name)+1, name);

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
