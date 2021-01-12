Author:     Chase Dickerson
Program:    ftp_client.cpp & ftp_server.cpp

My solution to the problem of creating a simple ftp server is as follows.
    I first created a server to handle requests from a client and to serve the client.
    The server uses fork processing to handle multiple clients. When the server is running,
    it is always listening for requests from a client. I use sockets, bind, listen, and accept 
    in my server to make a connection with a client. The server is able to show all files on it,
    send files to clients, and able to accept files from client.

    I then created a client to send requests to the server. The client is able to show all files
    on the client, send files to the server, and download files from the server. The client connects
    to the server through sockets. 


Contents of directory: ReadMe.txt, Makefile, ftp_server.cpp, ftp_client.cpp