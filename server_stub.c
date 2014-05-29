#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "ece454rpc_types.h"

#define BUFLEN 512
#define PORT 9001

struct fn_db{
     char * proc_name;
     fp_type fnpoint;     
     struct fn_db *next ; 
};

typedef struct fn_db db; 
db * cur;
db * head = NULL;

bool register_procedure(const char *procedure_name,
    const int nparams,
    fp_type fnpointer)
{
   // printf("trying to add %s\n", procedure_name);
    if (head == NULL){
        cur = (db *) malloc(sizeof(db));
        cur -> proc_name = (char *)  procedure_name;
        cur -> fnpoint = fnpointer;
        cur -> next = NULL;;
        head = cur;
        cur = head;
     //   printf("%s\n",head->proc_name);
        return true;
    }
    // find if function is already registered
    else {
         db * tmp = head;
        
         while (tmp -> next != NULL){
            // printf("%s\n", tmp->proc_name);
             if (strcmp(tmp->proc_name, procedure_name) == 0){
                   fprintf(stderr, "possible duplicate entry\n");
                   return false;
             }
             tmp = tmp -> next;
         }
         // the procedure is not to be found
         // register it in the dtabase
	 db * new_entry;
         new_entry = (db *) malloc(sizeof(db));
         new_entry -> proc_name = (char *) procedure_name;
         new_entry -> fnpoint = fnpointer;
         new_entry -> next = NULL;
         cur -> next = new_entry;
         cur = new_entry;
        
         /*
         // print the linked database
         db *print = head;
         while (print){
            printf("%s\n", print -> proc_name);
            print = print -> next;
         }*/
         return true;
        
    }
    printf("register\n");
    return 0;
}

void launch_server()
{
    
    int fd; 
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM,0 );
    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name , "eth0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);
    close (fd);
    
    printf("%s %d\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),PORT);
    struct sockaddr_in server, client;
    int s, i, slen=sizeof(client);
    char buf[BUFLEN];

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
        perror("socket");
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (struct sockaddr *) &server, sizeof(server))==-1) {
        perror("bind");
    }
    while(1) {
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &client, &slen)==-1) {
            perror("recvfrom()");
        }

        printf("Received packet from %s:%d\nData: %s\n\n",
                inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);
        
       char *  buf_ptr = buf; 
        char * proc_name;;
       // printf ("before increment of buf: %i \n", buf_ptr);
       // buf_ptr += sizeof(procedure_name);
       // printf ("after increment of buf ptr: %i \n", buf_ptr); 
       // void *  nparams = &buf[*buf_ptr];
       // printf ("procedure name is : %s \n", procedure_name);
       // printf ("naparams is: %i \n", nparams);
       int nparams;
       void * param1;
       void * param2; 
       memcpy(&proc_name, buf_ptr, 6);
       memcpy(&nparams, buf+sizeof(proc_name), 4);
       memcpy(&param1, buf+sizeof(proc_name)+8,4);
       memcpy(&param2, buf+sizeof(proc_name)+16,4);
       printf("number of params is: %i  name of procedure is : %s name of param1: %i name of param2: %i \n ",nparams, &proc_name, param1, param2);  
  
        int j = 0;
        for ( j =0; j < BUFLEN; j++){
             printf ("%i ",buf[j]);
             if (j%20 == 0 && j >0 ){
                printf("\n");
             }

        }
    }

    // should never reach here
    close(s);
}
