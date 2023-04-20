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

using namespace std;

const int max_buffer_size = 1000000;

int main()
{
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Client is up and running." << endl;

    // Specify the address and port of the main server
    struct sockaddr_in main_server_address;
    main_server_address.sin_family = AF_INET;
    main_server_address.sin_port = htons(24089);
    main_server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the main server
    connect(client_socket, (struct sockaddr *)&main_server_address, sizeof(main_server_address));

    while (true)
    {
        cout << "Please enter the usernames to check schedule availability:" << endl;
        string username_line;
        getline(cin, username_line);
        // cout << username_line.c_str() << endl;
        vector<string> usernames;
        string current_username;
        stringstream username_stringstream(username_line);
        while (getline(username_stringstream, current_username, ' '))
        {
            usernames.push_back(current_username);
        }

        // Send username list to the main server
        send(client_socket, username_line.c_str(), username_line.length(), 0);
        cout << "Client finished sending the usernames to Main Server." << endl;

        char buffer[max_buffer_size] = {0};
        // Receive the error message and result from the main server
        int recv_len = recv(client_socket, buffer, max_buffer_size, 0);
        string combined_message = buffer;
        size_t position_to_split = combined_message.find(";");
        string not_exist_usernames = combined_message.substr(0, position_to_split);
        combined_message = combined_message.substr(position_to_split + 1);
        position_to_split = combined_message.find(";");
        string usernamesA = combined_message.substr(0, position_to_split);
        // combined_message = combined_message.substr(position_to_split + 1);
        // position_to_split = combined_message.find(";");
        // string usernamesB = combined_message.substr(0, position_to_split);
        // string intervals = combined_message.substr(position_to_split + 1);
        string usernamesB = combined_message.substr(position_to_split + 1);
        // Correct port number!!!
        if (not_exist_usernames.length())
            cout << "Client received the reply from Main Server using TCP over port <port number>:\n" + not_exist_usernames + " do not exist." << endl;

        // // Receive the result from the main server
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        recv(client_socket, buffer, max_buffer_size, 0);
        string intervals = buffer;

        // Correct port number!!!
        cout << "Client received the reply from Main Server using TCP over port <port number>:\nTime intervals " + intervals + " works for " + usernamesA + ", " + usernamesB + "." << endl;

        cout << "Please enter the final meeting time to register an meeting:" << endl;
        string schedule;
        getline(cin, schedule);

        // Send the final schedule to the main server.
        send(client_socket, schedule.c_str(), schedule.length(), 0);
        cout << "Sent the request to register " + schedule + " as the meeting time for " + usernamesA + ", " + usernamesB + "." << endl;

        // Receive the update confirmation from the main server
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        recv(client_socket, buffer, max_buffer_size, 0);
        cout << "Received the notification that registration has finished." << endl;

        cout << "-----Start a new request-----" << endl;
    }

    close(client_socket);
    return 0;
}