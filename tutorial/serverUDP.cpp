#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main()
{
    // create a socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // specify the server address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // bind the socket to the server address and port
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // receive data from the client
    char buffer[1024] = {0};
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    int recv_len = recvfrom(server_socket, buffer, 1024, 0, (struct sockaddr *)&client_address, &addrlen);
    std::cout << "Client message: " << buffer << std::endl;

    // send data to the client
    const char *message = "Hello, client!";
    sendto(server_socket, message, strlen(message), 0, (struct sockaddr *)&client_address, addrlen);

    // close the socket
    close(server_socket);
    return 0;
}