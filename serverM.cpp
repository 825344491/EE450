#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <unordered_map>

using namespace std;

const int max_buffer_size = 1000000;

int main()
{
    // Create a datagram socket
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // Specify the address and port of the main server
    struct sockaddr_in udp_address;
    udp_address.sin_family = AF_INET;
    udp_address.sin_port = htons(23089);
    udp_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the udp socket to the server address and port
    bind(udp_socket, (struct sockaddr *)&udp_address, sizeof(udp_address));
    cout << "Main Server is up and running." << endl;

    // Receive users list from a backend server
    char buffer1[max_buffer_size] = {0};
    struct sockaddr_in backend_address, backend_A_address, backend_B_address;
    socklen_t backend_address_length = sizeof(backend_address);
    recvfrom(udp_socket, buffer1, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
    short port1;
    if (ntohs(backend_address.sin_port) == 21089)
    {
        cout << "Main Server received the username list from server A using UDP over port 21089" << endl;
        port1 = 21089;
        backend_A_address = backend_address;
    }
    else
    {
        cout << "Main Server received the username list from server B using UDP over port 22089" << endl;
        port1 = 22089;
        backend_B_address = backend_address;
    }
    string user_list1 = buffer1;

    // Receive users list from the other backend server
    char buffer2[max_buffer_size] = {0};
    recvfrom(udp_socket, buffer2, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
    short port2;
    if (ntohs(backend_address.sin_port) == 21089)
    {
        cout << "Main Server received the username list from server A using UDP over port 21089" << endl;
        port2 = 21089;
        backend_A_address = backend_address;
    }
    else
    {
        cout << "Main Server received the username list from server B using UDP over port 22089" << endl;
        port2 = 22089;
        backend_B_address = backend_address;
    }
    string user_list2 = buffer2;

    // Store usernames - server mapping into unordered_map
    unordered_map<string, short> usernames_server_map;
    stringstream userlist1_stringstream(user_list1), userlist2_stringstream(user_list2);
    string current_username;
    while (getline(userlist1_stringstream, current_username, ' '))
    {
        usernames_server_map[current_username] = port1;
    }
    while (getline(userlist2_stringstream, current_username, ' '))
    {
        usernames_server_map[current_username] = port2;
    }
    // for (auto x : usernames_server_map)
    // {
    //     cout << x.first << ": " << x.second << endl;
    // }

    // Create a stream socket
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Specify the address and port of the main server
    struct sockaddr_in tcp_address;
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_port = htons(24089);
    tcp_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the TCP socket to the server address and port
    bind(tcp_socket, (struct sockaddr *)&tcp_address, sizeof(tcp_address));

    // Listen for incomming connection requests from client
    listen(tcp_socket, 3);

    // Accept incomming connection requests from client
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    int client_socket = accept(tcp_socket, (struct sockaddr *)&client_address, &client_address_length);

    char buffer[max_buffer_size] = {0};
    while (true)
    {
        // Receive username list from the client
        recv(client_socket, buffer, max_buffer_size, 0);
        // This might be the part that the requirements said not to hardcode
        cout << "Main Server received the request from client using TCP over port 24089." << endl;
        string username_line = buffer;

        // Extract usernames from the string, detect errors and store them into a vector
        string usernamesA;
        string usernamesB;
        string not_exist_usernames = "";
        stringstream username_stringstream(username_line);
        while (getline(username_stringstream, current_username, ' '))
        {
            auto item = usernames_server_map.find(current_username);
            if (item == usernames_server_map.end())
                not_exist_usernames += current_username + ", ";
            else if (item->second == 21089)
                usernamesA += current_username + ", ";
            else
                usernamesB += current_username + ", ";
        }
        // Add delimiter to the end of each list
        not_exist_usernames += "\b\b";
        usernamesA += "\b\b";
        usernamesB += "\b\b";

        // cout << "Not exist: " << not_exist_usernames << endl;
        // cout << "A: ";
        // for (string username : usernamesA)
        // {
        //     cout << username << ", ";
        // }
        // cout << "\b\b." << endl;
        // cout << "B: ";
        // for (string username : usernamesB)
        // {
        //     cout << username << ", ";
        // }
        // cout << "\b\b." << endl;

        // Main server reply the client with usernames that not exist
        string combined_message = not_exist_usernames + ";" + usernamesA + ";" + usernamesB + ";";
        send(client_socket, combined_message.c_str(), combined_message.length(), 0);
        if (not_exist_usernames.length())
            cout << "<" << not_exist_usernames << "> do not exist. Send a reply to the client." << endl;

        // // Main server send the requested usernames to server A and B
        // sendto(udp_socket, usernamesA.c_str(), usernamesA.length(), 0, (struct sockaddr *)&backend_A_address, sizeof(backend_A_address));
        // cout << "Found " + usernamesA + " located at Server A. Send to Server A." << endl;
        // sendto(udp_socket, usernamesB.c_str(), usernamesB.length(), 0, (struct sockaddr *)&backend_B_address, sizeof(backend_B_address));
        // cout << "Found " + usernamesB + " located at Server B. Send to Server B." << endl;

        //

        // Main server decides the final time slots
        cout << "Found the intersection between the results from server A and B:" << endl;
        string result = "All are busy. Cancel the meeting!";
        cout << result << endl;

        // Main server sends the result to the client
        send(client_socket, result.c_str(), result.length(), 0);
        cout << "Main Server sent the result to the client." << endl;

        // Receive the final schedule from the client
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        recv(client_socket, buffer, max_buffer_size, 0);
        string schedule = buffer;
        cout << "Receive the request to register " + schedule + " as the meeting time for " + usernamesA + ", " + usernamesB + "." << endl;

        // for (string username : usernamesA)
        // {
        //     cout << username << ", ";
        // }
        // for (string username : usernamesB)
        // {
        //     cout << username << ", ";
        // }
        // cout << "\b\b." << endl;

        // Send the final schedule to the backend servers

        // Receive the update confirmation from backend servers

        // Send the update confirmation to the client
        string confirmation = "Notified Client that registration has finished.";
        send(client_socket, confirmation.c_str(), confirmation.length(), 0);
        cout << confirmation << endl;
    }
    close(udp_socket);
    close(tcp_socket);
    close(client_socket);
}