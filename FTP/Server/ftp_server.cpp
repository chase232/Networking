//********************************************************************
//
// Chase Dickerson
// Computer Networks
// Programming Project #1: HW3
// File: ftp_server.cpp
// October 10th, 2019
// Instructor: Dr. Ajay K. Katangur
//
//********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <fstream>
using namespace std;

struct sockaddr_in servAddr;
struct sockaddr_in clientAddr;

ifstream inStream;
ofstream outStream;

void ftp_function(int accept_1);

//********************************************************************
//
// Main Function
//
// This function is the main function and holds all of the main compents
//      of the program. It creates the socket, binds the socket, listens
//      any incoming requests, and then accepts.
//
// Parameters
// --------------------
// argc        int             Number of arguments passed from command line
// argv []     char*           Actual contents of command line
//
// Local Variables
// ---------------
// int port_number;
// port_number = atoi(argv[1]);
// int sock;
// int bind_1;
// int listen_1;
// int accept_1;
// int pid;  
//
//*******************************************************************
int main(int argc, char* argv []) 
{
    // Ensuring correct amount of command line arguments 
    if (argc < 1 || argc > 2)
    {
        printf("Enter only the filename and portnumber eg.\"filename portnumber\"");
        return 0;
    }

    int port_number;
    port_number = atoi(argv[1]);
    int sock;
    int bind_1;
    int listen_1;
    int accept_1;
    int pid;  

    // Getting socket using TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Failed to create a socket\n");
        exit(1);
    }

    // Setting up server
    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = port_number;

    // Binding socket
    bind_1 = bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if(bind < 0)
    {
        printf("Failed to bind\n");
        exit(1);
    }

    // Listening for new sockets
    listen_1 = listen(sock, 5);
    if(listen < 0)
    {
        printf("Failed to listen\n");
        exit(1);
    }

    socklen_t addrLen = sizeof(clientAddr);
    
    while(1)
    {  
        // Accept socket
        accept_1 = accept(sock, (struct sockaddr *)&clientAddr, &addrLen);
        
        if(accept_1 < 0){
            printf("Failed to accept connection\n");
            exit(1);
        }
        
        // Used to serve multiple clients concurrently 
        if((pid = fork()) == 0)
        {
            close(sock);
            ftp_function(accept_1);
            close(accept_1);
            exit(0);
        }
        close(accept_1);
    }

    return 0;
}

//********************************************************************
//
// ftp_function Function
//
// This function handles all requests from the client. It shows all of the
//  files on the server, is able to send files to client, and is able to 
//  accept files from client. 
//
// Parameters
// --------------------
// accept_1        int            Accept socket
//
// Local Variables
// ---------------
// DIR *d;
// struct dirent *dir;
// struct stat file_stat;
// char filename [100];
// char buffer[100];
// char command1[15];
// char command2[15]; 
//
//*******************************************************************
void ftp_function(int accept_1)
{
    DIR *d;
    struct dirent *dir;
    struct stat file_stat;

    while(1)
    {
        char filename [100];
        char buffer[100];
        char command1[15];
        char command2[15];

        // Start recieving from client
        int bytes = read(accept_1, buffer, 100);
        sscanf(buffer, "%s%s", command1, command2);

        // Checking if the client wants to view all files on server
        if(!strcmp(command1, "ls"))
        {
            string files = "";
            // Getting all server file names
            if ((d = opendir(".")) != NULL)
            {
                int i = 1;
                while ((dir = readdir(d)) != NULL)
                {
                    if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )
                    {
                        // do nothing 
                    } else {
                        files.append(std::to_string(i));
                        files.append(" "); 
                        files.append(dir->d_name);
                        files.append("\n");
                        i++;
                    }               
                }
                closedir(d);
            }
            // Either open file or create it 
            outStream.open("serverfiles.txt");
            if(outStream.fail())
            {
                inStream.open("serverfiles.txt");
                if(inStream.fail())
                {
                    cout << "ERROR";
                } else{
                    inStream >> files;
                }
            } 
            else 
            {
                outStream << files;
            }

            inStream.close();
            outStream.close();
            char cstr[files.size() + 1];
            strcpy(cstr, files.c_str());

            // Send file list to client
            sendto(accept_1, cstr, sizeof(cstr), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
            continue;
        }

        // Checking if client wants to download a file
        else if(!strcmp(command1, "d"))
        {
            string downloadFile = "";
            if ((d = opendir(".")) != NULL)
            {
                int i = 1;
                // Getting the name of the desired file
                while ((dir = readdir(d)) != NULL)
                {
                    if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )
                    {
                        // do nothing 
                    } else if (std::to_string(i) == command2) {
                        cout << std::to_string(i);
                        downloadFile.append(dir->d_name);
                        break;
                    } else {
                        i++;
                    }      
                }
                closedir(d);
            }
            char cstr[downloadFile.size() + 1];
            strcpy(cstr, downloadFile.c_str());
            stat(cstr, &file_stat);

            // Send name of file
            sendto(accept_1, cstr, sizeof(cstr), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));

            int fd = open(cstr, O_RDONLY);
            if(fd < 0)
            {
                cout << "ERROR";
                exit(1);
            }
            int fileSize = file_stat.st_size;

            // Send file size
            sendto(accept_1, &fileSize, sizeof(int), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
            // Send file
            sendfile(accept_1, fd, NULL, fileSize); 
            break;
        }

        // Checking if client wants to upload a file
        else if(!strcmp(command1, "u"))
        {
            int size;
            int fileSize = read(accept_1, &size, sizeof(size) - 1);
            int file = open(command2, O_RDWR | O_TRUNC | O_CREAT);
            malloc(size);
            write(file, malloc(size), size);
            close(file);
            break;
        }
        else 
        {
            cout << "Invalid Input\n";
            break;
        }
    }
}