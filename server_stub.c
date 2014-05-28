#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "ece454rpc_types.h"

#define BUFLEN 512
#define PORT 9001

bool register_procedure(const char *procedure_name,
    const int nparams,
    fp_type fnpointer)
{
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
    }

    // should never reach here
    close(s);
}
