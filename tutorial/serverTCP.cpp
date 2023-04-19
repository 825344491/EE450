#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>

int main()
{
    // create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify the server address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // bind the socket to the server address and port
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // listen for incoming connections
    listen(server_socket, 3);

    // accept incoming connections
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen);

    // send data to the client
    const char *message = "Hello, client!";
    send(client_socket, message, strlen(message), 0);

    // receive data from the client
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);
    std::cout << "Client message: " << buffer << std::endl;
    std::cout << "Received at: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;

    // close the socket
    close(server_socket);
    return 0;
}