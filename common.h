#pragma once

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);
int server_sock_addr_init(const char *protocolParam, const char *portParam, struct sockaddr_storage *storage);
void converterEnderecoEmString(const struct sockaddr *addr, char *str, size_t strsize);
