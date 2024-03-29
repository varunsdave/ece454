#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ece454_fs.h"

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

int main(int argc, char *argv[]) {
    if(argc < 4) {
	fprintf(stderr, "usage: %s <srv-ip/name> <srv-port> <local dir name>\n", argv[0]);
	exit(1);
    }

    printf ("\n\n\n\nenter main fs_client \n. ***********\n test case involves mounting  3 server alias, opendir 3 aliases  and unmount 3 server alias \n");

    char *dirname = argv[3];
    printf("fsMount(): %d\n", fsMount(argv[1], atoi(argv[2]), dirname));
    printf("fsMount(): - folderAlsia %d\n", fsMount(argv[1],atoi(argv[2]), "folderAlias"));
    printf("fsMount(): - ../exFolder %d\n", fsMount(argv[1],atoi(argv[2]), "../exFolder"));
    
    FSDIR *fd = fsOpenDir(dirname);
    //printf("client app, returned fd-> num is: %i\n",fd->num) ;
    if(fd == NULL) {
	perror("fsOpenDir"); exit(1);
    }
    printf ("successfully opened dir with id of FSDIR: %i \n",fd->num);
     
    FSDIR *fd2 = fsOpenDir("folderAlias/apples/");
    //printf("client app, -- testin second open returned fd-> num is: %i\n",fd2->num) ;
    if(fd2 == NULL) {
	perror("fsOpenDir For second failed"); exit(1);
    }
    printf ("successfully opened dir with id of FSDIR: %i \n",fd2->num);
   
    FSDIR *fd3 = fsOpenDir("../exFolder/apples/bananas/");
    if(fd3 == NULL) {
         perror("fsOpenDir exFolder/apples/bananas failed \n");exit(1);
    }
    printf ("successfully oepened bananas dir with id of FSDIR : %i \n",fd3->num);


    printf ("\t now checking fsDirent part\n");
    struct fsDirent *fdent = NULL;
    printf("client app, fd->num is : %i\n",fd->num);
    for(fdent = fsReadDir(fd); fdent != NULL; fdent = fsReadDir(fd)) {
	printf("\t %s, %d\n", fdent->entName, (int)(fdent->entType));
    }

    if(errno != 0) {
	perror("fsReadDir");
    }

    printf("fsCloseDir(): %d\n", fsCloseDir(fd));
    struct fsDirent *fdent2 = NULL;
    printf("client app, fd2->num is : %i\n",fd2->num);
    for(fdent2 = fsReadDir(fd2); fdent2 != NULL; fdent2 = fsReadDir(fd2)) {
	printf("\t %s, %d\n", fdent2->entName, (int)(fdent2->entType));
    }

    if(errno != 0) {
	perror("fsReadDir");
    }

    printf("fsCloseDir(): %d\n", fsCloseDir(fd2));
    
    struct fsDirent *fdent3 = NULL;
    printf("client app, fd3->num is : %i\n",fd3->num);
    for(fdent3 = fsReadDir(fd3); fdent3 != NULL; fdent3 = fsReadDir(fd3)) {
	printf("\t %s, %d\n", fdent3->entName, (int)(fdent3->entType));
    }

    if(errno != 0) {
	perror("fsReadDir");
    }


    // part to test open close and write

    int ff = open("/dev/urandom", 0);
    if(ff < 0) {
        perror("open(/dev/urandom)"); exit(1);
    }
    else printf("open(): %d\n", ff);
    //return 0;
    char fname[256];
    sprintf(fname, "%s/", dirname);
    if(read(ff, (void *)(fname+strlen(dirname)+1), 10) < 0) {
        perror("read(/dev/urandom)"); exit(1);
    }

    int i;
    for(i = 0; i < 10; i++) {
        //printf("%d\n", ((unsigned char)(fname[i]))%26);
        fname[i+strlen(dirname)+1] = ((unsigned char)(fname[i+strlen(dirname)+1]))%26 + 'a';
    }
    fname[10+strlen(dirname)+1] = (char)0;
    printf("Filename to write: %s\n", (char *)fname);
 char buf[256];
    if(read(ff, (void *)buf, 256) < 0) {
        perror("read(2)"); exit(1);
    }

    printBuf(buf, 256);

    printf("close(): %d\n", close(ff));
    int ctr;
//    for (ctr =0; ctr < 100;ctr++){
    ff = fsOpen("../exFolder/apples/01.txt", 1);
    if(ff < 0) {
        perror("fsOpen(write)"); exit(1);
    }
    
    if(fsWrite(ff, buf, 256) < 256) {
        fprintf(stderr, "fsWrite() wrote fewer than 256\n");
    }
    sleep(60);
    //return 0;
    if(fsClose(ff) < 0) {
        perror("fsClose"); exit(1);
    }

    sleep(60);
    //return 0;
    char readbuf[256];
    if((ff = fsOpen("../exFolder/apples/01.txt", 0)) < 0) {
        perror("fsOpen(read)"); exit(1);
    }

    int readcount = -1;

    if((readcount = fsRead(ff, readbuf, 256)) < 256) {
        fprintf(stderr, "fsRead() read fewer than 256\n");
    }

    if(memcmp(readbuf, buf, readcount)) {
        fprintf(stderr, "return buf from fsRead() differs from data written!\n");
    }
    else {
        printf("fsread(): return buf identical to data written upto %d bytes.\n", readcount);
    }

    if(fsClose(ff) < 0) {
        perror("fsClose return is < 0"); exit(1);
    }
    ff = fsOpen("sample/apples/value_write_01",1);
    if (ff < 0){
       perror("fsOpen(value_write) probably failed");exit(1);
    }
    char *testBuf= "line 01 to be added to the file\n";
    if (fsWrite(ff,testBuf,strlen(testBuf)) < strlen(testBuf)){
       perror ("fsWrite line 01 on value_write failed");exit(1);
    }
    
    char *testBuf2= "line 02";
    if (fsWrite(ff,testBuf2,strlen(testBuf2)) < strlen(testBuf2)){
       perror ("fsWrite line 02 on value_write failed");exit(1);
    }
    if (fsClose(ff) < 0) { perror("fsClose with value_write file failed");exit(1);}
    
    
    printf("fsCloseDir(): %d\n", fsCloseDir(fd3));
    printf("fsUnmount(): folderAlias %i\n",fsUnmount("folderAlias"));
    printf("fsUnmount(): ../exFolder/ %i\n",fsUnmount("../exFolder"));
   printf("fsUnmount(): dirFolder entered %i \n",fsUnmount(dirname));
    //printf("fsUnmount(): %s\n",dirname,fsUnmount(dirname));    
    //return 0;
//*/
    
    printf ("\n\n successfully exit fs_client test 0_10 \n. ***********\n test case involves mounting  3 server alias, opendir 3 aliases  and unmount 3 server alias\n \t\t subsequent writes are appended, new writes are overridden\n \n open 01.txt in apples folder without sleep. write to it and then read from it after\n");
    return 0;
}
