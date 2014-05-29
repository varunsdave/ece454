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

    // Server loops forever, waiting for UDP packets
    while(1) {
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &client, &slen)==-1) {
            perror("recvfrom()");
        }

        printf("Received packet from %s:%d\nData: %s\n\n",
                inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);
        
        char* buf_ptr = buf;
        char* proc_name;;
        int nparams;
        void* param1;
        void* param2;

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
       int cnt = 0;
       // generate a list of arg list
       arg_type * at;
       at = (arg_type *) malloc(sizeof(arg_type)); 
    //   at -> next = NULL;
       for (cnt = 0; cnt < nparams; cnt ++){
           arg_type * at_tmp;
           at_tmp = (arg_type *)malloc(sizeof(arg_type));
           void* arg_v;
           int arg_s;
           buf_ptr += 4;
           memcpy(&arg_s, buf_ptr,4);
           buf_ptr += sizeof(arg_s);
           memcpy (&arg_v, buf_ptr, arg_s);
           at_tmp -> arg_val = arg_v;
           at_tmp -> arg_size = arg_s; 
           if (cnt == 0){
             at = at_tmp;
           }
           else {
             at -> next = at_tmp;
           } 

           printf("%i\n", at_tmp->arg_val);    
           //delete at_tmp; 
       }
       
       // copy first param
      // buf_ptr += 8;
      // memcpy(&param1, buf_ptr,4);
       // copy second param
    //   buf_ptr += 8;
     //  memcpy(&param2, buf_ptr,4);
      // printf("number of params is: %i  name of procedure is : %s name of param1: %i name of param2: %i\n ",nparams, &proc_name, param1, param2);  
        
       db * tmp;
       tmp  = head;
             
       int registered_proc = 0; // if entry is not registered break
       while (tmp->next != NULL){
           char * str = tmp->proc_name;
         if (strcmp(str,proc_name)==0){
       //        return_type rt (*(tmp->fnpoint)) (nparams, arg_type *);     
              printf("matching string found \n");  
              registered_proc = 1; 
              break;   
          }
          printf("%s\n",tmp->proc_name);
          tmp = tmp -> next;
       }

       if (registered_proc == 0){
            printf("error\n");
       }
        memcpy(&nparams, buf+sizeof(proc_name), 4);
        memcpy(&param1, buf+sizeof(proc_name)+8,4);
        memcpy(&param2, buf+sizeof(proc_name)+16,4);
        printf("number of params is: %i  name of procedure is : %s name of param1: %i name of param2: %i \n ",nparams, proc_name, param1, param2);  
  
       /*
        int j = 0;
        for ( j =0; j < BUFLEN; j++){
             printf ("%i ",buf[j]);
             if (j%20 == 0 && j >0 ){
                printf("\n");
             }

        }*/
    }

    // should never reach here
    close(s);
}
