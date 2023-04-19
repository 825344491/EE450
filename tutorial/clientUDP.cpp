#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main()
{
    // create a socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // specify the server address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    // send data to the server
    const char *message = "Hello, server!";
    sendto(client_socket, message, strlen(message), 0, (struct sockaddr *)&server_address, sizeof(server_address));

    // receive data from the server
    char buffer[1024] = {0};
    struct sockaddr_in server_response_address;
    socklen_t addrlen = sizeof(server_response_address);
    int recv_len = recvfrom(client_socket, buffer, 1024, 0, (struct sockaddr *)&server_response_address, &addrlen);
    std::cout << "Server message: " << buffer << std::endl;

    // close the socket
    close(client_socket);
    return 0;
}