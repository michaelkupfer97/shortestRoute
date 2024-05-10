#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> 
#include "std_lib_facilities.h"
#include <iostream>
//HOW TO RUN: g++ client.cpp 
// ./a.out <IP> <port> <V1> <V2>

int main(int argc, char **argv) {

    // Creating a new socket.
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2])); 
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    // Connect each client with the address info of the server.
    connect(client_sock, ( sockaddr *)&addr, sizeof(addr)); 
    
    // Handling the message to send to the server.
    string massage1;
    massage1 = to_string(argv[3]) + " " + to_string(argv[4]);
    const char *message = massage1.c_str() ;
    send(client_sock, message, strlen(message), 0);
    char buffer[256];
    recv(client_sock, buffer, sizeof(buffer), 0);
    cout << buffer << endl; // The actual printing to the terminal 
    close(client_sock);
    return 0;
}