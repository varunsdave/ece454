#include <stdio.h>
#include <stdlib.h>
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
    ...);

return_type make_remote_call(
    const char *servernameorip,
    const int serverportnumber,
    const char *procedure_name,
    const int nparams,
    ...)
{
    printf("%s\n", "make call");

    struct sockaddr_in server;
    int s, i, slen=sizeof(server);
    char buf[BUFLEN] = "test";

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

    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&server, slen)==-1) {
        perror("sendto()");
    }

    close(s);

    return_type r;
    return r;
}
