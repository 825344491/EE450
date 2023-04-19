#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <thread>
#include <algorithm>

void handle_client(int client_socket, std::vector<int> &client_sockets)
{
    char buffer[1024] = {0};
    while (true)
    {
        // receive message from client
        int recv_len = recv(client_socket, buffer, 1024, 0);
        if (recv_len == -1)
        {
            std::cerr << "Failed to receive message from client." << std::endl;
            break;
        }
        std::string message = buffer;

        // relay message to all clients
        for (auto it = client_sockets.begin(); it != client_sockets.end(); ++it)
        {
            if (*it != client_socket)
            {
                int send_len = send(*it, message.c_str(), message.length(), 0);
                if (send_len == -1)
                {
                    std::cerr << "Failed to send message to client." << std::endl;
                    break;
                }
            }
        }

        // clear buffer
        memset(buffer, 0, sizeof(buffer));
    }

    // remove client socket from vector and close socket
    auto it = std::find(client_sockets.begin(), client_sockets.end(), client_socket);
    if (it != client_sockets.end())
    {
        client_sockets.erase(it);
    }
    close(client_socket);
}

int main()
{
    // create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // bind the socket to an address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // use any available interface
    server_address.sin_port = htons(8080);       // bind to port 8080
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        close(server_socket);
        return 1;
    }

    // listen for incoming connections
    if (listen(server_socket, 3) == -1)
    {
        std::cerr << "Failed to listen for incoming connections." << std::endl;
        close(server_socket);
        return 1;
    }

    // handle incoming connections
    std::vector<int> client_sockets;
    while (true)
    {
        // accept incoming connection
        struct sockaddr_in client_address;
        socklen_t addrlen = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen);
        if (client_socket == -1)
        {
            std::cerr << "Failed to accept incoming connection." << std::endl;
            continue;
        }

        // add client socket to vector
        client_sockets.push_back(client_socket);

        // start thread to handle client
        std::thread t(handle_client, client_socket, std::ref(client_sockets));
        t.detach();
    }
    // close the server socket
    close(server_socket);
    return 0;
}