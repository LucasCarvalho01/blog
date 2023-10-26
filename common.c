#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage) {
    if(addrstr == NULL || portstr == NULL)
        return -1;
    
    uint16_t port = (uint16_t) atoi(portstr); 
    if(port == 0)
        return -1;
    port = htons(port); 

    struct in_addr inaddr4; 
    if(inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; 
    if(inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }
    return -1;
}

int server_sock_addr_init(const char *protocolParam, const char *portParam, struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t)atoi(portParam); 
    if (port == 0) {
        return -1;
    }
    port = htons(port); 

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(protocolParam, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(protocolParam, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
}
