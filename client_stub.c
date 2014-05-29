#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "ece454rpc_types.h"

#define BUFLEN 512
#define SRV_IP "127.0.0.1"
#define PORT 9001

return_type make_remote_call(
    const char *servernameorip,
    const int serverportnumber,
    const char *procedure_name,
    const int nparams,
    ...)
{
    printf("%s to %s\n", "make call", procedure_name);

    struct sockaddr_in server;
    int s, i, slen=sizeof(server);
    char buf[BUFLEN] = "";
    char* buf_position = buf;

    va_list valist;
    arg_type args;

    // Copy procedure name to buf
    strcpy(buf_position, procedure_name);
    buf_position += sizeof(procedure_name);

    // Copy nparams to buf
    memcpy(buf_position, &nparams, sizeof(nparams));
    buf_position += sizeof(nparams);

    va_start(valist, nparams);

    /* access all the arguments assigned to valist */
    for (i = 0; i < nparams; i++)
    {
        // copy size of param to buf
        int size = va_arg(valist, int);
        memcpy(buf_position, &size, sizeof(int));
        buf_position += sizeof(int);

        // copy param to buf
        void* param = va_arg(valist, void*);
        memcpy(buf_position, param, size);
        buf_position += size;
    }

    /* clean memory reserved for valist */
    va_end(valist);

    // Print buf
    /*
    for (i = 0; i < BUFLEN; i++) {
        printf("%i ", buf[i]);
        if (i % 20 == 0 && i > 0) {
            printf("\n");
        }
    }*/
    
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
        perror("socket");
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_aton(SRV_IP, &server.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    // Send buf to server
    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&server, slen)==-1) {
        perror("sendto()");
    }

    /* Receive return value from server */
    memset(buf, 0, BUFLEN);
    buf_position = buf;

    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &server, &slen)==-1) {
        perror("recvfrom()");
    }

    return_type r;

    memcpy(&r.return_size, buf_position, sizeof(int));
    buf_position += sizeof(int);

    r.return_val = malloc(r.return_size);
    memcpy(r.return_val, buf_position, r.return_size);

    close(s);

    return r;
}
