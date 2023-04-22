# EE450 Socket Programming Project

## Full Name

Guodong Sun

## USCID

4400674089

## My Work

### Server M

#### Basic Part

##### Socket programming

Use functions from libary `<sys/socket.h>` to communicate with client and backend server A and B:

1. `socket()`: create TCP and UDP sockets for server M
2. `htons()`: convert human-readable port number into network format
3. `inet_addr()`: convert human-readable IP address into network format
4. `bind()`: bind the sockets with specified IP address and port number
5. `ntoh()`: convert port number from network format to human-readable format
6. `recvfrom()`: receive username lists and intersections of intervals from server A and B via UDP
7. `sendto()`: send requested username list to server A and B via UDP
8. `recv()`: receive requested username list from client via TCP connection
9. `send()`: send not exist usernames and final result to client via TCP connecion
10. `memset()`: clear buffer

##### Result processing

1. Use `unordered_map<string, unsignedshort>` to store usernames - server mapping, so that it only takes $O(1)$ to check which server a certain username is in
2. Use `stringstream` and `getline()` function to split each username from username list sent by server A and B, and to split each username from requested usernames from client

#### Extra Credit Part

##### Socket programming

Use functions from libary `<sys/socket.h>` to communicate with client and backend server A and B:

1. `recv()`: receive requested interval from client via TCP connection
2. `sendto()`: send requested interval to server A and B via UDP
3. `recvfrom()`: receive update confirmation from server A and B via UDP
4. `send()`: send update confirmation to server M via TCP connecion
5. `memset()`: clear buffer

### Server A / Server B

#### Basic Part

##### Implement `read_input()` function to read interval lists from input file

1. Use `remove()` and `erase()` function to remove all spaces in input files
2. Use `find()` and `substr()` functions to split username from interval list
3. Call `string2vector()` to convert interval list into `vector<vector<int>>`
4. Use `unordered_map<string, vector<vector<int>>>` to store username - interval list mapping, so that it only takes $O(1)$ to access a certain user's interval list

##### Socket programming

Use functions from libary `<sys/socket.h>` to communicate with server M:

1. `socket()`: create a UDP socket for server A and B
2. `htons()`: convert human-readable port number into network format
3. `inet_addr()`: convert human-readable IP address into network format
4. `bind()`: bind the sockets with specified IP address and port number
5. `sendto()`: send username lists and intersections of intervals to server M via UDP
6. `recvfrom()`: receive requested username list and intersections of intervals from server A and B via UDP
7. `memset()`: clear buffer

##### Result Processing

1. Use `find()` and `substr()` function to split each user from username list sent by server M

#### Extra Credit Part

##### Implement `update_intervals()` function to update interval lists for all involved users

For each involved user, we traverse their interval list to find which interval the requested one is in, and update that interval to zero or one or two new intervals according to the value of requested interval. For the result intervals and all the others, store them into a new `vector<vector<int>>`, and store it into the `unordered_map<string, vector<vector<int>>>`.

##### Socket programming

Use functions from libary `<sys/socket.h>` to communicate with server M:

1. `recvfrom()`: receive requested interval from server M via UDP
2. `sendto()`: send update confirmation to server M via UDP
3. `memset()`: clear buffer

### client

#### Basic Part

##### Socket programming

Use functions from libary `<sys/socket.h>` to communicate with server M:

1. `socket()`: create a TCP socket for client
2. `htons()`: convert human-readable port number into network format
3. `inet_addr()`: convert human-readable IP address into network format
4. `connect()`: set up connection between client and server M
5. `getsockname()`: get address information of client socket, including IP address and port number
6. `send()`: send requested username list to server M via TCP connecion
7. `recv()`: receive not exist usernames and final result from server M via TCP connection
8. `memset()`: clear buffer

##### Result Processing

1. Use `unique()` and `erase()` functions to replace consecutive space in user's input with a single space
2. Use `find()` and `substr()` functions to split not exist usernames, usernames in server A and usernames in server B from message sent by server M

#### Extra Credit Part

##### Implement `interval_validate()` function to validate the input interval from client

To be a valid interval, the input must meet all the situations below:

1. It is wrapped by "`[]`"
2. There is one and only one "`,`" between "`[]`"
3. Two strings between "`[`" and "`,`", and between "`,`" and "`]`", must be integers
4. Two integers must be within the range of "`[0, 100]`"
5. The left integer must be less than the right interger
6. The interval must within one and only one interval from the available intervals returned by server M

Repeat calling this function until user's input is valid.

##### Socket programming

Use functions from libary `<sys/socket.h>` to communicate with server M:

1. `send()`: send requested interval to server M via TCP connecion
2. `recv()`: receive update confirmation from server M via TCP connection
3. `memset()`: clear buffer

### Tool Functions (called by more than one hosts)

#### Implement `intersection_of_2_intervals()` function to compute the intersection of two interval lists

Use two-pointers to point the two intervals that are being processed. For each pair of intervals, we decide whether they have intersection or not. If they have, then put their intersection into result vector, and move the pointer that points the interval on the left side.

#### Implement `intersection_of_intervals()` function to compute the intersection of one or more interval lists

Implemented by calling intersection_of_2_intervals() iteratively

#### Implement `string2vector()` function to convert interval list from `string` to `vector<vector<int>>`.

For strings whose length less than or equal to 4, they are empty list, so return empty vector. For other strings, remove the first two characters ("`[[`") and last two characters ("`]]`"), then use "`],[`" as delimiter to seperate all intervals, and use `sscanf()` to extract left and right end of each interval. Finally, each interval is stored as `vector<int>`, and the list of intervals is stored as `vector<vector<int>>`.

#### Implement `vector2string()` function to convert interval list from `vector<vector<int>>` to `string`

Insert each interval in form of `vector<int>` into the string with the format `[%d,%d]`, and seperate each interval with "`,`". Finally wrap the result string with a pair of "`[]`".

## Project Structure and Workflow

### serverM.cpp

#### Basic Part

1. Create a datagram socket, and bind it with specified IP address and port number
2. Receive username lists from server A and B
3. Store usernames - server mapping into unordered_map
4. Create a stream socket, bind it with specified IP address and port number, listen and accept incomming connection requests from client
5. Receive requested username list from client
6. Extract usernames from the string, and seperate them into three catagories: not exist users, users in server A, users in server B
7. Send all three catagories of usernames to client
8. Receive an empty message from the client to prevent two messages sent by main server arrive at the same time and received together by the client
9. Send usernames in A and B to server A and B, respectively
10. Receive results from server A and B
11. Compute the intersection of two results if needed
12. Send the final result to client

#### Extra Credit Part

13. Receive requested interval from client
14. Send requested interval to server A and B
15. Receive update confirmation from server A and B
16. Send update confirmation to client

### serverA.cpp / serverB.cpp

#### Basic Part

1. Read input file and store username - interval list mapping into unordered_map
2. Retrive all the usernames in the current server
3. Create a datagram socket, and bind it with specified IP address and port number
4. Send username list to server M
5. Receive usernames from server M
6. Find the intersections among interval lists of all involved users
7. Send the result to server M

#### Extra Credit Part

8. Receive requested interval from server M
9. Register requested interval to all involved users
10. Send update confirmation to server M

### client.cpp

#### Basic Part

1. Create a stream socket
2. Set up connection with server M
3. Get usernames from input
4. Send requested username list to server M
5. Receive final result from server M

#### Extra Credit Part

6. Get requested interval from input
7. Validate input
8. Send requested interval to server M
9. Receive update confirmation from server M

## Messages Format

### Username list from server A and B to server M

Usernames are concatenated and delimited by " "

`Username1 Username2 ... UsernameN`

### Requested username list from client to server M

Usernames are concatenated and delimited by " "

`Username1 Username2 ... UsernameN`

### All three catagories of usernames from server M to client

Each catagory is concatenated and delimited by "`;`", and usernames within each catagory are concatenated and delimited by "`, `"

`Username1, Username2, ..., UsernameN;Username1, Username2, ..., UsernameN;Username1, Username2, ..., UsernameN`

### Usernames in A and B from server M to server A and B

usernames are concatenated and delimited by "`, `"

`Username1, Username2, ... UsernameN`

### Results from server A and B to server M, and from server M to client

Each interval is in the format of `[%d,%d]`. All intervals are concatenated and delimited by "`,`", and finally wrapped by "`[]`"

`[[low1,high1],[low2,high2],...,[lowN,highN]]`

### Requested interval from client to server M, and from server M to server A and B

The interval is in the format of `[%d,%d]`.

### Update confirmation from server A and B to server M, and from server M to client

Plain string.

## Idiosyncrasy


## References

1. Function calls of socket programming are based on implementations from [Socket Programming Tutorial In C For Beginners | Part 1 | Eduonix - YouTube](https://www.youtube.com/watch?v=LtXEMwSG5-8), [C++ Network Programming Part 1: Sockets - YouTube](https://www.youtube.com/watch?v=gntyAFoZp-E), [Socket Programming in C, C++ Part 2 (socket, bind, about sockaddr_in, listen etc) - YouTube](https://www.youtube.com/watch?v=W9b9SaGXIjA) and [Creating a TCP Server in C++ [Linux / Code Blocks] - YouTube](https://www.youtube.com/watch?v=cNdlrbZSkyQ).
2. Function `intersection_of_2_intervals()` is rewritten from my own solution for [LeetCode 986](https://leetcode.com/problems/interval-list-intersections/).
