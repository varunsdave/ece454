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

/*
    Database struct 
    A linked list that registers by name and function pointer
*/ 
struct fn_db {
     char* procedure_name;
     int num_params;
     fp_type fnpoint;     
     struct fn_db* next ; 
};

typedef struct fn_db db; 
db* current;
db* head = NULL;

bool register_procedure(const char *procedure_name, const int nparams,
                        fp_type fnpointer) {
    printf("Register procedure: %s %d\n", procedure_name, nparams);
    if (head == NULL) {
        current = malloc(sizeof(db));
        current->procedure_name = (char *)procedure_name;
        current->fnpoint = fnpointer;
        current->num_params = nparams;
        current->next = NULL;;
        head = current;
        current = head;

        printf("registered function name: %s, regsitered params: %d\n",procedure_name,nparams);
        return true;
    }
    // find if function is already registered
    else {
         db* tmp = head;
        
         while (tmp->next != NULL) {
             if (strcmp(tmp->procedure_name, procedure_name) == 0) {
                   
                   fprintf(stderr, "possible duplicate entry\n");
                   return false;
             }
             tmp = tmp->next;
         }
         // the procedure is not to be found
         // register it in the dtabase
	 db* new_entry;
         new_entry = malloc(sizeof(db));
         new_entry->procedure_name = (char *)procedure_name;
         new_entry->fnpoint = fnpointer;
         new_entry->num_params = nparams;
         new_entry->next = NULL;
         current->next = new_entry;
         current = new_entry;
        
         return true;
    }

    return 0;
}

void launch_server() {
    int fd;
    struct ifreq ifr;

    // gets the ip address and the port the server connected to
    // the code is gathered from:
    // http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);
    close (fd);
    
    struct sockaddr_in server, client;
    int s, i, slen = sizeof(client);
    char buf[BUFLEN];

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
    }

    memset((char *)&server, 0, sizeof(server));
    mybind(s, &server);

    printf("%s %d\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), ntohs(server.sin_port));
    printf("start launch server()\n");
    // Server loops forever, waiting for UDP packets
    while (1) {
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&client, &slen)==-1) {
            perror("recvfrom()");
        }

        char* buf_ptr = buf;
        char* procedure_name;
        int nparams;
        printf("entered proc_name: %s, entered nparams: %d \n",procedure_name,nparams);
        /* Copy procedure name */
        // Get size of procedure name string
        int num_chars = strlen(buf_ptr) + 1;
        // Allocate memory for procedure name
        procedure_name = malloc(sizeof(char) * num_chars);
        // Copy procedure name from buffer
        strcpy(procedure_name, buf_ptr);

        buf_ptr += sizeof(procedure_name);
        // Copy params
        memcpy(&nparams, buf_ptr, sizeof(int));
        buf_ptr += sizeof(int);
        // generate a list of arg list
        arg_type* at, *tail;
        at = malloc(sizeof(arg_type)); 
        tail = at;

        printf("Received call to %s\n", procedure_name);

        // iterate through param numbers and build arglist
        int cnt;
        for (cnt = 0; cnt < nparams; cnt++) {
            //declate temp node for arg type
            arg_type* at_tmp = malloc(sizeof(arg_type));
            void* arg_v;
            int arg_s;

            memcpy(&arg_s, buf_ptr, sizeof(int));
            buf_ptr += sizeof(arg_s);

            arg_v = malloc(arg_s);
            printf("argv: %p, buf %p, args %d\n", arg_v, buf_ptr, arg_s);
            memcpy(arg_v, buf_ptr, arg_s);
            buf_ptr += arg_s;

            at_tmp->arg_val = arg_v;
            at_tmp->arg_size = arg_s; 
                       
            if (cnt == 0) {
                at = at_tmp;
                tail = at;
            }
            else {
                tail->next = at_tmp;
                tail = tail->next; 
            }
        }
       
        // lookup function and create return type 
        db* tmp;
        tmp = head;
        int registered_proc = 0; // if entry is not registered break
        while (tmp == head || tmp->next != NULL) {
            char* str = tmp->procedure_name;
            if (strcmp(str, procedure_name) == 0) {
              // check number of registered parameters:
              int registered_params = tmp->num_params;
              
              return_type r;
              const int params_num = nparams; 

              if (params_num != registered_params){
                 registered_proc = 0;
                 
                 printf("number of registered params do not match \n --  passed params: %i, registered params: %i \n",params_num, registered_params);
                 break;
              }

              printf("call made to : %s function with: %i  parameters\n",procedure_name,params_num);           
 
              // Call function
              r = (*(tmp->fnpoint))(params_num, at);

              // return r to client
              memset(buf, 0, BUFLEN);
              buf_ptr = buf;

              memcpy(buf_ptr, &r.return_size, sizeof(int));
              buf_ptr += sizeof(int);
              memcpy(buf_ptr, r.return_val, r.return_size);

              if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&client, slen) == -1){
                perror("sendto()");
              }
                   
              // successfully sent the return vlaue to clinet. Server process completed
              registered_proc = 1; 
              arg_type *node = at;
              while (node != NULL){
                arg_type *temp_arg = node;
                node = node->next; 
                free(temp_arg);
               
              }
            free(node);
            
            break;   
            
          }
          tmp = tmp->next;
       }
       // Procedure not registered
       if (registered_proc == 0){
           return_type r; 
           r.return_val = NULL;
           r.return_size = 0;

           // return r to client
           memset(buf, 0, BUFLEN);
           buf_ptr = buf;
           memcpy(buf_ptr, &r.return_size, sizeof(int));
           buf_ptr += sizeof(int);
           memcpy(buf_ptr, r.return_val, r.return_size);

           // send the struct
           if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&client, slen) == -1){
               perror("sendto()");
           }
       }
    }

    // should never reach here
    close(s);
}
