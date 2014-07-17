/* Mahesh V. Tripunitara
   University of Waterloo
   Part of a simplified RPC implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplified_rpc/ece454rpc_types.h"

#if 0
#define _DEBUG_1_
#endif

/* We allocate a global variable for returns. However, 
 * r.return_val is allocated dynamically. We look to
 * the server_stub to deallocate after it returns the
 * response to the client. */

return_type r;

extern printRegisteredProcedures();

return_type fsMount(const int nparams, arg_type* a) {
    //struct stat sbuf;

    //return(stat(localFolderName, &sbuf));
    return r;
}

return_type fsUnmount(const int nparams, arg_type* a) {
    //return 0;
    return r;
}

return_type fsOpenDir(const int nparams, arg_type* a) {
    //return(opendir(folderName));
    return r;
}

return_type fsCloseDir(const int nparams, arg_type* a) {
    //return(closedir(folder));
    return r;
}

return_type fsReadDir(const int nparams, arg_type* a) {
    /*const int initErrno = errno;
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
    */
    return r;
}

return_type fsOpen(const int nparams, arg_type* a) {
    /*int flags = -1;

    if(mode == 0) {
    flags = O_RDONLY;
    }
    else if(mode == 1) {
    flags = O_WRONLY | O_CREAT;
    }

    return(open(fname, flags, S_IRWXU));*/
    return r;
}

return_type fsClose(const int nparams, arg_type* a) {
    //return(close(fd));
    return r;
}

return_type fsRead(const int nparams, arg_type* a) {
    //return(read(fd, buf, (size_t)count));
    return r;
}

return_type fsWrite(const int nparams, arg_type* a) {
    //return(write(fd, buf, (size_t)count));
    return r;
}

return_type fsRemove(const int nparams, arg_type* a) {
    //return(remove(name));
    return r;
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        printf("usage: %s <folder_name>\n", argv[0]);
        return 0;
    } 

    // Register all procedures
    register_procedure("fsRemove", 1, fsRemove);

#ifdef _DEBUG_1_
    printRegisteredProcedures();
#endif

    launch_server(argv[1]);
    return 0;
}
