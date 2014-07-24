/* Mahesh V. Tripunitara
   University of Waterloo
   Part of a simplified RPC implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
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
    printf("fsDir num value stored as: %i\n", fsdir_tail->fsdir->num);
    printf("fsDir head num value stored as: %i\n", fsdir_head->fsdir->num);
}

DIR* get_dir_from_fsdir_num(int num) {
    DIR* d = NULL;
    
    printf("fsCloseDir(), fsDir head num value stored as: %i\n", fsdir_head->fsdir->num);
    struct fsdir_entry* fsdir_p = fsdir_head;
 
    while (fsdir_p != NULL) {
        FSDIR* temp = fsdir_p->fsdir;
        printf("temp poitner created and the num is: %i \n", temp->num);
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
    printf("close_fsdir(): %i with errono %s\n", ret,strerror(errno));

    return ret;
}

return_type fsMount(const int nparams, arg_type* a) {
    if (nparams != 1) {
        printf("np\n"); fflush(stdout);
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    if (a->arg_size != sizeof(int)) {
        printf("as\n"); fflush(stdout);
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    int* ret_val = malloc(sizeof(int));

    memcpy(ret_val, a->arg_val, sizeof(int));

    r.return_val = ret_val;
    r.return_size = sizeof(int);

    return r;
}

return_type fsUnmount(const int nparams, arg_type* a) {
    if (nparams != 0) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    int return_val = 0;
    r.return_val = &return_val;
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

    char* full_path = base_folder;
    strcat(full_path, "/");
    strcat(full_path, folder);

    DIR *dir;
    printf("fsOpenDir(), the full path is: %s\n", full_path);
    dir = opendir(full_path);
    printf("entered fsOpenFDir on server\n");
    FSDIR *fsdir;
    fsdir = malloc(sizeof(FSDIR));
    printf("succefful malloc for fsdir \n");
    fsdir->num = fsdir_num_counter+1;
    //fsdir -> num = 1;
    fsdir->dir = dir;
    printf("initialized fsDir values now about to enter storeFsDir\n");
    store_fsdir(fsdir);
    
    printf("stored fsDir succeffully \n");
    int* fsdirNumVal = malloc(sizeof(int));
    *fsdirNumVal = fsdir->num;
    r.return_val = fsdirNumVal;
    r.return_size = sizeof(int);

    printf ("returning fsOpenDir\n");
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
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    int fd = *(int *)a->arg_val;

    int return_val = close(fd);

    r.return_val = &return_val;
    r.return_size = sizeof(return_val);

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
    //return(read(fd, buf, (size_t)count));
    return r;
}

return_type fsWrite(const int nparams, arg_type* a) {
    if (nparams != 3) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    //int fd = *(int *)a->arg_val;
    //write(fd, buf, (size_t)count);

    return r;
}

return_type fsRemove(const int nparams, arg_type* a) {
    if (nparams != 1) {
        // error
        r.return_val = NULL;
        r.return_size = 0;
        return r;
    }

    char* file_name = (char *)a->arg_val;
    char* full_path = base_folder;
    strcat(full_path, file_name);

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
