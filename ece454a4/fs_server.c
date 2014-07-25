/* Mahesh V. Tripunitara
   University of Waterloo
   Part of a simplified RPC implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "simplified_rpc/ece454rpc_types.h"
#include "fsOtherIncludes.h"




#if 0
#define _DEBUG_1_
#endif

/* We allocate a global variable for returns. However, 
 * r.return_val is allocated dynamically. We look to
 * the server_stub to deallocate after it returns the
 * response to the client. */

struct fsDirent {
    char entName[256];
    unsigned char entType; /* 0 for file, 1 for folder,
                  -1 otherwise. */
};

return_type r;

char* base_folder;

int fsdir_num_counter = 0;

extern printRegisteredProcedures();

struct fsdir_entry* fsdir_head = NULL;
struct fsdir_entry* fsdir_tail = NULL;

/* Linked list of fsdirs opened by clients*/
struct fsdir_entry {
    FSDIR* fsdir;
    struct fsdir_entry *next;
};
void printBuf(char *buf, int size) {
    /* Should match the output from od -x */
    int i;
    for(i = 0; i < size; ) {
        if(i%16 == 0) {
            printf("%08o ", i);
        }

        int j;
        for(j = 0; j < 16;) {
            int k;
            for(k = 0; k < 2; k++) {
                if(i+j+(1-k) < size) {
                    printf("%02x", (unsigned char)(buf[i+j+(1-k)]));
                }
            }

            printf(" ");
            j += k;
        }

        printf("\n");
        i += j;
    }
}

void store_fsdir(FSDIR *fsdir) {
    struct fsdir_entry* new_fsdir_entry;
    new_fsdir_entry = malloc(sizeof(struct fsdir_entry));

    new_fsdir_entry->fsdir = fsdir;
    new_fsdir_entry->next = NULL;
  
    if (fsdir_head == NULL) {
        fsdir_head = new_fsdir_entry;
        fsdir_tail = fsdir_head;
    }
    else {
        fsdir_tail->next = new_fsdir_entry;
        fsdir_tail = new_fsdir_entry;
    }
    //printf("fsDir num value stored as: %i\n", fsdir_tail->fsdir->num);
    //printf("fsDir head num value stored as: %i\n", fsdir_head->fsdir->num);
}

DIR* get_dir_from_fsdir_num(int num) {
    DIR* d = NULL;
    
    //printf("get_dir_from_fsdir_num(), fsDir head num value stored as: %i\n", fsdir_head->fsdir->num);
    struct fsdir_entry* fsdir_p = fsdir_head;
 
    while (fsdir_p != NULL) {
        FSDIR* temp = fsdir_p->fsdir;
        //printf("temp poitner created and the num is: %i \n", temp->num);
        if (fsdir_p->fsdir->num == num) {
            d = fsdir_p->fsdir->dir;
            printf("found the D \n");
            break;
        }

        fsdir_p = fsdir_p->next;
    }

    return d;
}

int close_fsdir(FSDIR fsdir) {
    DIR* d = get_dir_from_fsdir_num(fsdir.num);

    int ret = closedir(d);

    return ret;
}

return_type fsMount(const int nparams, arg_type* a) {
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    if (a->arg_size != sizeof(int)) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    int* ret_val = malloc(sizeof(int));
    *ret_val = *(int *)a->arg_val;

    r.return_val = ret_val;
    r.return_size = sizeof(int);

    return r;
}

return_type fsUnmount(const int nparams, arg_type* a) {
     if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    if (a->arg_size != sizeof(int)) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    int* ret_val = malloc(sizeof(int));
    *ret_val = *(int *)a->arg_val;

    r.return_val = ret_val;
    r.return_size = sizeof(int);

    return r;

}

return_type fsOpenDir(const int nparams, arg_type* a) {
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    char* folder = (char *)a->arg_val;

    printf("fsOpenDir(), folder is: %s\n", folder);
    char* full_path = malloc(strlen(base_folder)+strlen(folder)+1+1);
    strcpy(full_path, base_folder);
    strcat(full_path, "/");
    strcat(full_path, folder);
    printf("fsOpenDir(), the full path is: %s\n", full_path);

    DIR *dir;
    dir = opendir(full_path);
    FSDIR *fsdir;
    fsdir = malloc(sizeof(FSDIR));
    fsdir_num_counter += 1;
    fsdir->num = fsdir_num_counter;
    printf("fsdir num assigned to %i\n", fsdir->num);
    fsdir->dir = dir;
    store_fsdir(fsdir);
    
    int* fsdirNumVal = malloc(sizeof(int));
    *fsdirNumVal = fsdir->num;
    r.return_val = fsdirNumVal;
    r.return_size = sizeof(int);

    return r;
}

return_type fsCloseDir(const int nparams, arg_type* a) {
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    FSDIR fsdir;
    fsdir.num = *(int *)a->arg_val;

    int* return_val = malloc(sizeof(int));

    *return_val = close_fsdir(fsdir);

    r.return_val = return_val;
    r.return_size = sizeof(return_val);

    return r;
}

return_type fsReadDir(const int nparams, arg_type* a) {
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    FSDIR fsdir;
    fsdir.num = *(int *)a->arg_val;

    printf("num %i\n", fsdir.num);
    DIR* dir = get_dir_from_fsdir_num(fsdir.num);

    struct fsDirent* dent = malloc(sizeof(struct fsDirent));

    //const int initErrno = errno;
    struct dirent *d = readdir(dir);

    if(d == NULL) {
        //if(errno == initErrno) errno = 0;
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    if(d->d_type == DT_DIR) {
        dent->entType = 1;
    }
    else if(d->d_type == DT_REG) {
        dent->entType = 0;
    }
    else {
        dent->entType = -1;
    }

    memcpy(&(dent->entName), &(d->d_name), 256);

    r.return_val = dent;
    r.return_size = sizeof(dent);
    printf("exiting fsReadDir \n");

    return r;
}

return_type fsOpen(const int nparams, arg_type* a) {
    printf("entered open \n");
    if (nparams != 2) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    char* fname = (char *)a->arg_val;
    int mode = *(int *)a->next->arg_val;

    char* full_path = malloc(strlen(base_folder)+strlen(fname)+1+1);
    strcpy(full_path, base_folder);
    strcat(full_path, "/");
    strcat(full_path, fname);

    int flags = -1;

    if(mode == 0) {
        flags = O_RDONLY;
    }
    else if(mode == 1) {
        flags = O_WRONLY | O_CREAT;
    }

    printf("opening :%s \n", full_path);
    int* return_val = malloc(sizeof(int));
    *return_val = open(full_path, flags, S_IRWXU);
    *return_val = -2;
    r.return_val = return_val;
    
    r.return_size = sizeof(int);
    printf("fsOpen(), end\n");
    return r;
}

return_type fsClose(const int nparams, arg_type* a) {
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    int fd = *(int *)a->arg_val;

    int *return_val = malloc(sizeof(int));
    *return_val =  close(fd);

    r.return_val = return_val;
    r.return_size = sizeof(int);

    return r;
}

return_type fsRead(const int nparams, arg_type* a) {
    if (nparams != 3) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    int fd = *(int *)a->arg_val;
    int count = *(int *)a->next->next->arg_val;
    void *buf = malloc(count);

    int *return_val = malloc(sizeof(int));
    *return_val = read(fd,buf,(size_t)count);
    
    r.return_val = buf;
    r.return_size = count;
    printf("%i \n", r.return_size);
    printBuf(buf,r.return_size);
    return r;
}

return_type fsWrite(const int nparams, arg_type* a) {
    if (nparams != 3) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }
    printf("fsWrite(), entering");
    int fd = *(int *)a->arg_val;
    //int fd = *(int *)a->arg_val;
    unsigned int count = *(int *)a->next->next->arg_val;
    void *buf = malloc(count);
    memcpy(buf, a->next->arg_val, count);
    printf("fsWrite(), server \n");
    printBuf(buf,count);
    int *return_val = malloc(sizeof(int));
    *return_val =  write(fd,buf,(size_t)count);
    //write(fd, buf, (size_t)count);
    r.return_val = return_val;
    r.return_size = sizeof(int);
    return r;
}

return_type fsRemove(const int nparams, arg_type* a) {
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    char* fname = (char *)a->arg_val;

    char* full_path = malloc(strlen(base_folder)+strlen(fname)+1+1);
    strcpy(full_path, base_folder);
    strcat(full_path, "/");
    strcat(full_path,fname);

    int* return_val = malloc(sizeof(int));
    *return_val = remove(full_path);

    r.return_val = return_val;
    r.return_size = sizeof(return_val);

    return r;
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        printf("usage: %s <folder_name>\n", argv[0]);
        return 0;
    }

    base_folder = argv[1];

    // Register all procedures
    register_procedure("fsMount", 1, fsMount);
    register_procedure("fsUnmount", 1, fsUnmount);
    register_procedure("fsOpenDir", 1, fsOpenDir);
    register_procedure("fsCloseDir", 1, fsCloseDir);
    register_procedure("fsReadDir", 1, fsReadDir);
    register_procedure("fsOpen", 2, fsOpen);
    register_procedure("fsClose", 1, fsClose);
    register_procedure("fsRead", 3, fsRead);
    register_procedure("fsWrite", 3, fsWrite);
    register_procedure("fsRemove", 1, fsRemove);

#ifdef _DEBUG_1_
    printRegisteredProcedures();
#endif

    launch_server();
    return 0;
}
