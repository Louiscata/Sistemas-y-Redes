#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, const char *argv[]){

    //Ejercicio 1

    //1
    const char* ip_texto = "193.110.128.200";
    struct in_addr ip_binario;

    inet_pton(AF_INET, ip_texto, (void*) &ip_binario);
    printf("IPv4 en binario: %x\n", ip_binario.s_addr);

    //2
    char ip_texto_2[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, (const void*) &ip_binario, ip_texto_2, INET_ADDRSTRLEN);
    printf("IPv4 en texto: %s\n", ip_texto_2);

    //3
    const char* ipv6_texto = "1080:0:0:0:8:800:200C:417A";
    struct in6_addr ipv6_binario;

    inet_pton(AF_INET6, ipv6_texto, (void*) &ipv6_binario);
    printf("IPv6 en binario: ");
    for (int i = 0; i < 4; i++){
        printf("%x", ipv6_binario.__in6_u.__u6_addr32[i]);
    }
    printf("\n");

    //4
    char ipv6_texto_2[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, (const void*) &ipv6_binario, ipv6_texto_2, INET6_ADDRSTRLEN);
    printf("IPv6 en texto: %s\n", ipv6_texto_2);

    //Ejercicio 2

    //1

    uint16_t puerto = 0x6942;
    printf("Puerto: %x\n", puerto);
    uint16_t puerto_inv = htons(puerto);
    printf("Puerto inverso: %x\n", puerto_inv);
    uint16_t puerto_dobleinv = ntohs(puerto_inv);
    printf("Puerto inverso doble: %x\n", puerto_dobleinv);

    //1

    uint32_t puerto32 = 0x12345678;
    printf("Puerto 32 bits: %x\n", puerto32);
    uint32_t puerto32_inv = htonl(puerto32);
    printf("Puerto inverso 32: %x\n", puerto32_inv);
    uint32_t puerto32_dobleinv = ntohl(puerto32_inv);
    printf("Puerto inverso doble 32: %x\n", puerto32_dobleinv);

    return EXIT_SUCCESS;
}