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
     char* proc_name;
     fp_type fnpoint;     
     struct fn_db* next ; 
};

typedef struct fn_db db; 
db* cur;
db* head = NULL;

bool register_procedure(const char *procedure_name,
    const int nparams,
    fp_type fnpointer)
{
    if (head == NULL){
        cur = (db *) malloc(sizeof(db));
        cur -> proc_name = (char *)  procedure_name;
        cur -> fnpoint = fnpointer;
        cur -> next = NULL;;
        head = cur;
        cur = head;
        return true;
    }
    // find if function is already registered
    else {
         db* tmp = head;
        
         while (tmp -> next != NULL){
             if (strcmp(tmp->proc_name, procedure_name) == 0){
                   fprintf(stderr, "possible duplicate entry\n");
                   return false;
             }
             tmp = tmp -> next;
         }
         // the procedure is not to be found
         // register it in the dtabase
	 db* new_entry;
         new_entry = (db *) malloc(sizeof(db));
         new_entry -> proc_name = (char *) procedure_name;
         new_entry -> fnpoint = fnpointer;
         new_entry -> next = NULL;
         cur -> next = new_entry;
         cur = new_entry;
        
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

    // Server loops forever, waiting for UDP packets
    while(1) {
       if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &client, &slen)==-1) {
           perror("recvfrom()");
       }

       printf("Received packet from %s:%d\nData: %s\n\n",
                inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);
        
         
       char* buf_ptr = buf;
       char* proc_name;
       int nparams;

        /* Copy procedure name */
        // Get size of procedure name string
       int num_chars = strlen(buf_ptr) + 1;

        // Allocate memory for procedure name
       proc_name = malloc(sizeof(char) * num_chars);

        // Copy procedure name from buffer
       strcpy(proc_name, buf_ptr);

       buf_ptr += sizeof(proc_name);

        // Copy params
       memcpy(&nparams, buf_ptr, 4);
       // generate a list of arg list
       arg_type* at, * tail;
       at = (arg_type *) malloc(sizeof(arg_type)); 
       tail = at;
       // iterate through param numbers and build arglist
       int cnt = 0;
       for ( cnt = 0; cnt < nparams; cnt ++){
           
           //declate temp node for arg type
           arg_type* at_tmp = (arg_type *)malloc(sizeof(arg_type));
           void* arg_v;
           int arg_s;
           
           buf_ptr += 4;
           memcpy(&arg_s, buf_ptr,sizeof(int));
           buf_ptr += sizeof(arg_s);
           arg_v = malloc(arg_s);
           memcpy (arg_v, buf_ptr, arg_s);
           at_tmp -> arg_val = arg_v;
           at_tmp -> arg_size = arg_s; 
           
            
           if (cnt == 0){
             at = at_tmp;
             tail = at;
           }
           else {
             tail -> next = at_tmp;
             tail = tail -> next; 
           } 


       }
       
       // lookup function and create return type 
       db* tmp;
       tmp  = head;
             
       int registered_proc = 0; // if entry is not registered break
       while (tmp->next != NULL){
            char* str = tmp->proc_name;
            if (strcmp(str,proc_name)==0){
            return_type r;
            const int params_num = nparams; 
            r = (*(tmp->fnpoint))(params_num, at);

            // return r to client
            memset(buf,0,BUFLEN);
            buf_ptr = buf;

            memcpy(buf_ptr, &r.return_size, sizeof(int));
            buf_ptr += sizeof(int);
            memcpy(buf_ptr,r.return_val,r.return_size);

            // build a client struct
            if (sendto(s,buf, BUFLEN,0,(struct sockaddr *)&client,slen)==-1){
                perror("sendto()");
            }
                   
            // successfully sent the return vlaue to clinet. Server process completed
            registered_proc = 1; 
            break;   
          }
          tmp = tmp -> next;
       }
       // process not registered, return an error to stdio
       if (registered_proc == 0){
            printf("error\n");
       }

    }

    // should never reach here
    close(s);
}
