/* Mahesh V. Tripunitara
   University of Waterloo
   Part of a simplified RPC implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ece454_fs.h"
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

int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {
    struct stat sbuf;

    return(stat(localFolderName, &sbuf));
}

int fsUnmount(const char *localFolderName) {
    return 0;
}

FSDIR* fsOpenDir(const char *folderName) {
    return(opendir(folderName));
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

    return(open(fname, flags, S_IRWXU));
}

int fsClose(int fd) {
    return(close(fd));
}

int fsRead(int fd, void *buf, const unsigned int count) {
    return(read(fd, buf, (size_t)count));
}

int fsWrite(int fd, const void *buf, const unsigned int count) {
    return(write(fd, buf, (size_t)count));
}

return_type fsRemove(const char *name) {
    r.val = &(remove(name));
    r.size = sizeof(int)

    return r;
}

return_type concatStr(const int nparams, arg_type *a) {
    /* Concatenates 5 strings and returns them.
     * We assume a is 5 null-terminated strings. */

    if(nparams != 5) {
	r.return_val = NULL;
	r.return_size = 0;
	return r;
    }

    /* else */
    char *s[5];
    int i;
    int totallen = 0;

    for(i = 0; i < 5; i++) {
	s[i] = a->arg_val;
	a = a->next;
	totallen += strlen(s[i]);
    }

    r.return_val = (void *)calloc(totallen + 1, sizeof(char));
    ((char *)(r.return_val))[totallen] = (char) 0;

    for(i = totallen = 0; i < 5; totallen += strlen(s[i++])) {
	memcpy(((char *)(r.return_val)) + totallen, s[i], strlen(s[i]));
    }

    r.return_size = strlen((char *)(r.return_val)) + 1;
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
