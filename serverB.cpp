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
#include <fstream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <unordered_map>

using namespace std;

const int max_buffer_size = 1000000;

int main()
{
    // Read input from a.txt
    ifstream input;
    input.open("b.txt");
    string input_line, username;
    size_t start, end;
    unordered_map<string, vector<vector<int>>> username_intervals_map;
    while (input >> input_line)
    {
        vector<vector<int>> intervals;
        // Extract username
        start = input_line.find(";[[");
        end = input_line.find("]]");
        username = input_line.substr(0, start);
        // cout << username << ": [";

        // Extract each interval and store in a vector
        input_line = input_line.substr(start + 3, end - start - 3); // Remove username, ";[[" and "]]"
        start = 0;
        int left, right;
        while ((end = input_line.find("],[", start)) != string::npos)
        {
            sscanf(input_line.substr(start, end - start).c_str(), "%d,%d", &left, &right);
            intervals.push_back(vector<int>{left, right});
            start = end + 3;
        }
        sscanf(input_line.substr(start, end - start).c_str(), "%d,%d", &left, &right);
        intervals.push_back(vector<int>{left, right});

        // for (vector<int> interval : intervals)
        // {
        //     cout << "[" << interval[0] << ", " << interval[1] << "], ";
        // }
        // cout << "\b\b]" << endl;
        username_intervals_map[username] = intervals;
    }
    // Print hashmap
    // for (auto x : username_intervals_map)
    // {
    //     cout << x.first << ": [";
    //     for (vector<int> interval : x.second)
    //     {
    //         cout << "[" << interval[0] << ", " << interval[1] << "], ";
    //     }
    //     cout << "\b\b]" << endl;
    // }

    // Retrieve username list in server B
    string user_list = "";
    for (auto x : username_intervals_map)
    {
        user_list += x.first + " ";
    }
    // cout << user_list << endl;

    // Create a socket
    int backend_B_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // Specify the address and port of the backend server B
    struct sockaddr_in backend_B_address;
    backend_B_address.sin_family = AF_INET;
    backend_B_address.sin_port = htons(22089);
    backend_B_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the udp socket to the server address and port
    bind(backend_B_socket, (struct sockaddr *)&backend_B_address, sizeof(backend_B_address));
    cout << "Server B is up and running using UDP on port 22089." << endl;

    // Specify the address and port of the main server
    struct sockaddr_in main_server_address;
    main_server_address.sin_family = AF_INET;
    main_server_address.sin_port = htons(23089);
    main_server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send usernames list to main server
    sendto(backend_B_socket, user_list.c_str(), user_list.length(), 0, (struct sockaddr *)&main_server_address, sizeof(main_server_address));
    cout << "Server B finished sending a list of usernames to Main Server." << endl;

    // char buffer[max_buffer_size] = {0};
    // struct sockaddr_in main_server_response_address;
    // socklen_t main_server_response_address_length = sizeof(main_server_response_address);
    // while (true)
    // {
    //     recvfrom(backend_B_socket, buffer, max_buffer_size, 0, (struct sockaddr *)&main_server_response_address, &main_server_response_address_length);
    //     string usernames_line = buffer;
    //     cout << usernames_line << endl;
    // }

    close(backend_B_socket);
}