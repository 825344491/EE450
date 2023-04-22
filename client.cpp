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
#include <algorithm>

using namespace std;

const int max_buffer_size = 1000000;

/*
Function calls of socket programming are based on implementations from
1. [Socket Programming Tutorial In C For Beginners | Part 1 | Eduonix - YouTube](https://www.youtube.com/watch?v=LtXEMwSG5-8)
2. [C++ Network Programming Part 1: Sockets - YouTube](https://www.youtube.com/watch?v=gntyAFoZp-E)
3. [Socket Programming in C, C++ Part 2 (socket, bind, about sockaddr_in, listen etc) - YouTube](https://www.youtube.com/watch?v=W9b9SaGXIjA)
4. [Creating a TCP Server in C++ [Linux / Code Blocks] - YouTube](https://www.youtube.com/watch?v=cNdlrbZSkyQ).
*/

// Convert time slots from string to vector<vector<int>>
vector<vector<int>> string2vector(string input_line)
{
    vector<vector<int>> intervals;
    if (input_line.length() > 4)
    {
        input_line = input_line.substr(2, input_line.length() - 4);
        size_t start = 0, end;
        int left, right;
        while ((end = input_line.find("],[", start)) != string::npos)
        {
            sscanf(input_line.substr(start, end - start).c_str(), "%d,%d", &left, &right);
            intervals.push_back(vector<int>{left, right});
            start = end + 3;
        }
        sscanf(input_line.substr(start, end - start).c_str(), "%d,%d", &left, &right);
        intervals.push_back(vector<int>{left, right});
    }
    return intervals;
}

bool interval_validate(string schedule, vector<vector<int>> time_slots)
{
    // Input is not wrapped by "[]"
    if (schedule[0] != '[' || schedule[schedule.length() - 1] != ']')
        return false;

    // Input doesn't have ",", which is not an interval
    size_t position_to_split = schedule.find(",");
    if (position_to_split == string::npos)
        return false;

    // Either `left` or `right` is not a number (contains other characters, including spaces)
    string left = schedule.substr(1, position_to_split - 1);
    string right = schedule.substr(position_to_split + 1, schedule.length() - position_to_split - 2);
    if (left.find_first_not_of("0123456789") != string::npos || right.find_first_not_of("0123456789") != string::npos)
        return false;

    // Either `left` or `right` is not within the range of [0, 100], or `low >= high`
    int low = stoi(left), high = stoi(right);
    if (low < 0 || low > 100 || high < 0 || high > 100 || low >= high)
        return false;

    // Determine if the input interval is within one of the intervals in time_slots
    int i = 0;
    if (low < time_slots[i][0])
        return false;

    while (i < time_slots.size() && low >= time_slots[i][1])
        i++;

    if (i == time_slots.size())
        return false;

    if (low < time_slots[i][0] || high > time_slots[i][1])
        return false;

    return true;
}

bool isBothSpace(char const &left, char const &right)
{
    return left == right && left == ' ';
}

int main()
{
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Client is up and running." << endl;

    // Specify the address and port of the main server
    struct sockaddr_in main_server_address, client_address;
    main_server_address.sin_family = AF_INET;
    main_server_address.sin_port = htons(24089);
    main_server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t client_address_length = sizeof(client_address);

    // Connect to the main server
    connect(client_socket, (struct sockaddr *)&main_server_address, sizeof(main_server_address));
    getsockname(client_socket, (struct sockaddr *)&client_address, &client_address_length);
    unsigned short client_port = ntohs(client_address.sin_port);

    while (true)
    {
        cout << "Please enter the usernames to check schedule availability:" << endl;
        string username_line;
        getline(cin, username_line);
        // Replace consecutive space with a single space
        username_line.erase(unique(username_line.begin(), username_line.end(), isBothSpace), username_line.end());

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
        string usernamesB = combined_message.substr(position_to_split + 1);
        if (not_exist_usernames.length())
            cout << "Client received the reply from Main Server using TCP over port " + to_string(client_port) + ":\n" + not_exist_usernames + " do not exist." << endl;

        // Send an empty message to main server to prevent two messages sent by main server arrive at the same time and received together by the recv() above
        string empty = "[]";
        send(client_socket, empty.c_str(), empty.length(), 0);

        // Receive the result from the main server
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        recv_len = recv(client_socket, buffer, max_buffer_size, 0);
        string intervals = buffer;

        if (usernamesA.length() != 0 && usernamesB.length() != 0)
            cout << "Client received the reply from Main Server using TCP over port " + to_string(client_port) + ":\nTime intervals " + intervals + " works for " + usernamesA + ", " + usernamesB + "." << endl;
        else if (usernamesA.length() != 0)
            cout << "Client received the reply from Main Server using TCP over port " + to_string(client_port) + ":\nTime intervals " + intervals + " works for " + usernamesA + "." << endl;
        else if (usernamesB.length() != 0)
            cout << "Client received the reply from Main Server using TCP over port " + to_string(client_port) + ":\nTime intervals " + intervals + " works for " + usernamesB + "." << endl;

        if (intervals != "[]")
        {
            vector<vector<int>> time_slots = string2vector(intervals);
            string schedule;
            cout << "Please enter the final meeting time to register an meeting:" << endl;
            getline(cin, schedule);

            // Input validation
            while (!interval_validate(schedule, time_slots))
            {
                cout << "Time interval " + schedule + " is not valid. Please enter again:" << endl;
                getline(cin, schedule);
            }

            // Send the final schedule to the main server.
            send(client_socket, schedule.c_str(), schedule.length(), 0);
            if (usernamesA.length() != 0 && usernamesB.length() != 0)
                cout << "Sent the request to register " + schedule + " as the meeting time for " + usernamesA + ", " + usernamesB + "." << endl;
            else if (usernamesA.length() != 0)
                cout << "Sent the request to register " + schedule + " as the meeting time for " + usernamesA + "." << endl;
            else
                cout << "Sent the request to register " + schedule + " as the meeting time for " + usernamesB + "." << endl;

            // Receive the update confirmation from the main server
            memset(buffer, 0, sizeof(buffer)); // Clear buffer
            recv(client_socket, buffer, max_buffer_size, 0);
            cout << "Received the notification that registration has finished." << endl;
        }

        cout << "-----Start a new request-----" << endl;
    }

    close(client_socket);
    return 0;
}