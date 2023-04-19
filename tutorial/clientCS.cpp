#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>

void receive_messages(int client_socket)
{
    char buffer[1024] = {0};
    while (true)
    {
        // receive message from server
        int recv_len = recv(client_socket, buffer, 1024, 0);
        if (recv_len == -1)
        {
            std::cerr << "Failed to receive message from server." << std::endl;
            break;
        }
        std::cout << "Server message: " << buffer << std::endl;

        // clear buffer
        memset(buffer, 0, sizeof(buffer));
    }
}

int main()
{
    // create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // connect to server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // connect to local host
    server_address.sin_port = htons(8080);                   // connect to port 8080
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Failed to connect to server." << std::endl;
        close(client_socket);
        return 1;
    }

    // start thread to receive messages from server
    std::thread t(receive_messages, client_socket);
    t.detach();

    // send messages to server
    std::string message;
    while (true)
    {
        std::getline(std::cin, message);
        int send_len = send(client_socket, message.c_str(), message.length(), 0);
        if (send_len == -1)
        {
            std::cerr << "Failed to send message to server." << std::endl;
            break;
        }
    }

    // close the socket
    close(client_socket);
    return 0;
}