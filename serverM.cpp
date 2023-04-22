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

/*
Function calls of socket programming are based on implementations from
1. [Socket Programming Tutorial In C For Beginners | Part 1 | Eduonix - YouTube](https://www.youtube.com/watch?v=LtXEMwSG5-8)
2. [C++ Network Programming Part 1: Sockets - YouTube](https://www.youtube.com/watch?v=gntyAFoZp-E)
3. [Socket Programming in C, C++ Part 2 (socket, bind, about sockaddr_in, listen etc) - YouTube](https://www.youtube.com/watch?v=W9b9SaGXIjA)
4. [Creating a TCP Server in C++ [Linux / Code Blocks] - YouTube](https://www.youtube.com/watch?v=cNdlrbZSkyQ).
*/

// Return the intersection of two interval lists
// This function is rewritten from my own solution for [LeetCode 986](https://leetcode.com/problems/interval-list-intersections/).
vector<vector<int>> intersection_of_2_intervals(vector<vector<int>> &interval_list1, vector<vector<int>> &interval_list2)
{
    int i = 0, j = 0;
    vector<vector<int>> result;
    while (i < interval_list1.size() && j < interval_list2.size())
    {
        int low = max(interval_list1[i][0], interval_list2[j][0]);
        int high = min(interval_list1[i][1], interval_list2[j][1]);
        if (low < high)
            result.push_back({low, high});
        if (interval_list1[i][1] < interval_list2[j][1])
            i++;
        else
            j++;
    }
    return result;
}

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

// Convert time slots from vector<vector<int>> to string
string vector2string(vector<vector<int>> time_slots)
{
    string result = "[";
    for (auto time_slot : time_slots)
        result += "[" + to_string(time_slot[0]) + "," + to_string(time_slot[1]) + "],";
    if (result.length() > 1)
        result = result.substr(0, result.length() - 1);
    result += "]";
    return result;
}

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
    unsigned short port1;
    if (ntohs(backend_address.sin_port) == 21089)
    {
        cout << "Main Server received the username list from server A using UDP over port 23089" << endl;
        port1 = 21089;
        backend_A_address = backend_address;
    }
    else
    {
        cout << "Main Server received the username list from server B using UDP over port 23089" << endl;
        port1 = 22089;
        backend_B_address = backend_address;
    }
    string user_list1 = buffer1;

    // Receive users list from the other backend server
    char buffer2[max_buffer_size] = {0};
    recvfrom(udp_socket, buffer2, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
    unsigned short port2;
    if (ntohs(backend_address.sin_port) == 21089)
    {
        cout << "Main Server received the username list from server A using UDP over port 23089" << endl;
        port2 = 21089;
        backend_A_address = backend_address;
    }
    else
    {
        cout << "Main Server received the username list from server B using UDP over port 23089" << endl;
        port2 = 22089;
        backend_B_address = backend_address;
    }
    string user_list2 = buffer2;

    // Store usernames - server mapping into unordered_map
    unordered_map<string, unsigned short> usernames_server_map;
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
        // Delete ", " in the end
        not_exist_usernames = not_exist_usernames.substr(0, not_exist_usernames.length() - 2);
        usernamesA = usernamesA.substr(0, usernamesA.length() - 2);
        usernamesB = usernamesB.substr(0, usernamesB.length() - 2);

        // Main server reply the client with usernames that not exist
        string combined_message = not_exist_usernames + ";" + usernamesA + ";" + usernamesB;
        send(client_socket, combined_message.c_str(), combined_message.length(), 0);
        if (not_exist_usernames.length())
            cout << not_exist_usernames << " do not exist. Send a reply to the client." << endl;

        // Receive an empty message from the client to prevent two messages sent by main server arrive at the same time and received together by the client
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        recv(client_socket, buffer, max_buffer_size, 0);

        // Main server send the requested usernames to server A and B
        if (usernamesA.length() != 0)
        {
            sendto(udp_socket, usernamesA.c_str(), usernamesA.length(), 0, (struct sockaddr *)&backend_A_address, sizeof(backend_A_address));
            cout << "Found " + usernamesA + " located at Server A. Send to Server A." << endl;
        }
        if (usernamesB.length() != 0)
        {
            sendto(udp_socket, usernamesB.c_str(), usernamesB.length(), 0, (struct sockaddr *)&backend_B_address, sizeof(backend_B_address));
            cout << "Found " + usernamesB + " located at Server B. Send to Server B." << endl;
        }

        string result, result1, result2;
        if (usernamesA.length() != 0 && usernamesB.length() != 0)
        {
            // Main server reveive the time slots from backend server A and B
            memset(buffer1, 0, sizeof(buffer1)); // Clear buffer
            recvfrom(udp_socket, buffer1, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
            result1 = buffer1;
            if (ntohs(backend_address.sin_port) == 21089)
                cout << "Main Server received from server A the intersection result using UDP over port 23089:\n" + result1 + "." << endl;
            else
                cout << "Main Server received from server B the intersection result using UDP over port 23089:\n" + result1 + "." << endl;

            memset(buffer2, 0, sizeof(buffer2)); // Clear buffer
            recvfrom(udp_socket, buffer2, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
            result2 = buffer2;
            if (ntohs(backend_address.sin_port) == 21089)
                cout << "Main Server received from server A the intersection result using UDP over port 23089:\n" + result2 + "." << endl;
            else
                cout << "Main Server received from server B the intersection result using UDP over port 23089:\n" + result2 + "." << endl;

            // Main server decides the final time slots
            vector<vector<int>> time_slots1 = string2vector(result1);
            vector<vector<int>> time_slots2 = string2vector(result2);
            vector<vector<int>> final_time_slots = intersection_of_2_intervals(time_slots1, time_slots2);
            result = vector2string(final_time_slots);
        }
        else if (usernamesA.length() != 0)
        {
            // Main server reveive the time slots from backend server A
            memset(buffer1, 0, sizeof(buffer1)); // Clear buffer
            recvfrom(udp_socket, buffer1, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
            result = buffer1;
            cout << "Main Server received from server A the intersection result using UDP over port 23089:\n" + result + "." << endl;
        }
        else if (usernamesB.length() != 0)
        {
            // Main server reveive the time slots from backend server B
            memset(buffer2, 0, sizeof(buffer2)); // Clear buffer
            recvfrom(udp_socket, buffer2, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
            result = buffer2;
            cout << "Main Server received from server B the intersection result using UDP over port 23089:\n" + result + "." << endl;
        }
        else
            result = "[]";
        cout << "Found the intersection between the results from server A and B:\n" + result + "." << endl;

        // Main server sends the result to the client
        send(client_socket, result.c_str(), result.length(), 0);
        cout << "Main Server sent the result to the client." << endl;

        string schedule;
        if (result != "[]")
        {
            // Receive the final schedule from the client
            memset(buffer, 0, sizeof(buffer)); // Clear buffer
            recv(client_socket, buffer, max_buffer_size, 0);
            schedule = buffer;
            cout << "Main Server received the request from client using TCP over port 24089." << endl;
        }
        else
            schedule = "[]";
        // Send the final schedule to the backend servers
        if (usernamesA.length() != 0)
            sendto(udp_socket, schedule.c_str(), schedule.length(), 0, (struct sockaddr *)&backend_A_address, sizeof(backend_A_address));
        if (usernamesB.length() != 0)
            sendto(udp_socket, schedule.c_str(), schedule.length(), 0, (struct sockaddr *)&backend_B_address, sizeof(backend_B_address));

        // Receive the update confirmation from backend servers
        if (usernamesA.length() != 0 || usernamesB.length() != 0)
        {
            memset(buffer1, 0, sizeof(buffer1)); // Clear buffer
            recvfrom(udp_socket, buffer1, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
            result1 = buffer1;
        }

        if (usernamesA.length() != 0 && usernamesB.length() != 0)
        {
            memset(buffer2, 0, sizeof(buffer2)); // Clear buffer
            recvfrom(udp_socket, buffer2, max_buffer_size, 0, (struct sockaddr *)&backend_address, &backend_address_length);
            result2 = buffer2;
        }
        if (result != "[]")
        {
            // Send the update confirmation to the client
            string confirmation = "Main Server sent the result to the client.";
            send(client_socket, confirmation.c_str(), confirmation.length(), 0);
            cout << confirmation << endl;
        }

        memset(buffer, 0, sizeof(buffer));   // Clear buffer
        memset(buffer1, 0, sizeof(buffer1)); // Clear buffer
        memset(buffer2, 0, sizeof(buffer2)); // Clear buffer
    }
    close(udp_socket);
    close(tcp_socket);
    close(client_socket);
}