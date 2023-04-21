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
#include <algorithm>

using namespace std;

const int max_buffer_size = 1000000;

// Return the intersection of two interval lists
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

// Return the intersection of one or more interval lists by calling intersection_of_2_intervals() iteratively
vector<vector<int>> intersection_of_intervals(unordered_map<string, vector<vector<int>>> username_intervals_map, vector<string> usernames)
{
    vector<vector<int>> result = username_intervals_map[usernames[0]];
    for (int i = 1; i < usernames.size(); i++)
        result = intersection_of_2_intervals(result, username_intervals_map[usernames[i]]);
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

// Read input from b.txt
unordered_map<string, vector<vector<int>>> read_input(string file_name)
{
    ifstream input;
    input.open(file_name);
    string input_line, username;
    size_t position_to_split;
    unordered_map<string, vector<vector<int>>> username_intervals_map;
    while (input >> input_line)
    {
        // Remove all spaces
        input_line.erase(remove(input_line.begin(), input_line.end(), ' '), input_line.end());
        // Extract username
        position_to_split = input_line.find(";");
        username = input_line.substr(0, position_to_split);
        // cout << username << ": [";

        // Extract each interval and store in a vector
        input_line = input_line.substr(position_to_split + 1, input_line.length() - position_to_split - 1); // Remove username
        vector<vector<int>> intervals = string2vector(input_line);

        // for (vector<int> interval : intervals)
        // {
        //     cout << "[" << interval[0] << ", " << interval[1] << "], ";
        // }
        // cout << "\b\b]" << endl;
        username_intervals_map[username] = intervals;
    }
    return username_intervals_map;
}

// Update interval lists for all involved users
void update_intervals(unordered_map<string, vector<vector<int>>> &username_intervals_map, vector<string> usernames, string schedule)
{
    size_t position_to_split = schedule.find(",");
    int low = stoi(schedule.substr(1, position_to_split - 1));
    int high = stoi(schedule.substr(position_to_split + 1, schedule.length() - position_to_split - 2));
    for (int i = 0; i < usernames.size(); i++)
    {
        vector<vector<int>> intervals;
        int j = 0;
        while (j < username_intervals_map[usernames[i]].size() && low >= username_intervals_map[usernames[i]][j][1])
        {
            intervals.push_back(username_intervals_map[usernames[i]][j]);
            j++;
        }

        if (low > username_intervals_map[usernames[i]][j][0])
            intervals.push_back({username_intervals_map[usernames[i]][j][0], low});

        if (high < username_intervals_map[usernames[i]][j][1])
            intervals.push_back({high, username_intervals_map[usernames[i]][j][1]});

        j++;
        while (j < username_intervals_map[usernames[i]].size())
        {
            intervals.push_back(username_intervals_map[usernames[i]][j]);
            j++;
        }

        cout << usernames[i] + ": updated from " + vector2string(username_intervals_map[usernames[i]]) + " to " + vector2string(intervals) << endl;
        username_intervals_map[usernames[i]] = intervals;
    }
}

int main()
{
    unordered_map<string, vector<vector<int>>> username_intervals_map = read_input("b.txt");
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

    char buffer[max_buffer_size] = {0};
    struct sockaddr_in main_server_response_address;
    socklen_t main_server_response_address_length = sizeof(main_server_response_address);
    while (true)
    {
        // Receive usernames from main server
        recvfrom(backend_B_socket, buffer, max_buffer_size, 0, (struct sockaddr *)&main_server_response_address, &main_server_response_address_length);
        string usernames_line = buffer;
        cout << "Server B received the usernames from Main Server using UDP over port 22089." << endl;

        // Store usernames into a vector (edge case: size = 0!!!)
        vector<string> usernames;
        size_t start = 0, end;
        while ((end = usernames_line.find(", ", start)) != string::npos)
        {
            usernames.push_back(usernames_line.substr(start, end - start));
            start = end + 2;
        }
        usernames.push_back(usernames_line.substr(start, end - start));

        // Find the intersections among all users' interval lists
        vector<vector<int>> time_slots = intersection_of_intervals(username_intervals_map, usernames);

        // Convert result from vector<vector<int>> to string
        string result = vector2string(time_slots);
        cout << "Found the intersection result: " + result + " for " + usernames_line + "." << endl;

        // Send the time slots to the main server
        sendto(backend_B_socket, result.c_str(), result.length(), 0, (struct sockaddr *)&main_server_address, sizeof(main_server_address));
        cout << "Server B finished sending the response to Main Server." << endl;

        // Receive the final schedule from main server
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        recvfrom(backend_B_socket, buffer, max_buffer_size, 0, (struct sockaddr *)&main_server_response_address, &main_server_response_address_length);
        string schedule = buffer;
        cout << "Register a meeting at " + schedule + " and update the availability for the following users:" << endl;

        // Update interval lists for all involved users
        update_intervals(username_intervals_map, usernames, schedule);

        // Send the update confirmation to the main server
        string confirmation = "Notified Main Server that registration has finished.";
        sendto(backend_B_socket, confirmation.c_str(), confirmation.length(), 0, (struct sockaddr *)&main_server_address, sizeof(main_server_address));
        cout << confirmation << endl;

        memset(buffer, 0, sizeof(buffer)); // Clear buffer
    }

    close(backend_B_socket);
}