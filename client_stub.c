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

return_type make_remote_call(const char *servernameorip, const int serverportnumber,
                             const char *procedure_name, const int nparams, ...) {
    /* Convert server name to ip address */
    // http://stackoverflow.com/questions/7725498/getaddrinfo-function-returns-the-wrong-ip-address
    struct addrinfo hints, *res;
    struct in_addr addr;
    int err;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    if ((err = getaddrinfo(servernameorip, NULL, &hints, &res)) != 0) {
        printf("getaddrinfo(): %s\n", gai_strerror(err));
        exit(1);
    }
    addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
    char* server_ip = inet_ntoa(addr);
    freeaddrinfo(res);

    // Allocate UDP buffer
    char buf[BUFLEN] = "";
    char* buf_position = buf;

    // Copy procedure name to buf
    strcpy(buf_position, procedure_name);
    buf_position += sizeof(procedure_name);

    // Copy nparams to buf
    memcpy(buf_position, &nparams, sizeof(nparams));
    buf_position += sizeof(nparams);

    /* Access params based on nparams */
    va_list valist;
    va_start(valist, nparams);

    int i;
    for (i = 0; i < nparams; i++) {
        // Copy size of param to buf
        int size = va_arg(valist, int);
        memcpy(buf_position, &size, sizeof(int));
        buf_position += sizeof(int);

        // Copy param to buf
        void* param = va_arg(valist, void*);
        memcpy(buf_position, param, size);
        buf_position += size;
    }

    // Clean memory reserved for valist
    va_end(valist);

    // Create socket
    int s;
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket()");
    }

    // Build sockaddr_in (http://www.abc.se/~m6695/udp.html)
    struct sockaddr_in server;
    int slen = sizeof(server);
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverportnumber);
    if (inet_aton(server_ip, &server.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    // Send buf to server
    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&server, slen) == -1) {
        perror("sendto()");
    }

    /* Receive return value from server */
    memset(buf, 0, BUFLEN);
    buf_position = buf;

    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&server, &slen) == -1) {
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
