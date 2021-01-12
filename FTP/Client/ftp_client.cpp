//********************************************************************
//
// Chase Dickerson
// Computer Networks
// Programming Project #1: HW3
// File: ftp_client.cpp
// October 10th, 2019
// Instructor: Dr. Ajay K. Katangur
//
//********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h> 
#include <fcntl.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <fstream>
using namespace std;

//********************************************************************
//
// Main Function
//
// This function is the main function and holds all of the main compents
//      of the program. It creates the socket and then tries to connect 
//      to a server
//
// Parameters
// --------------------
// argc        int             Number of arguments passed from command line
// argv []     char*           Actual contents of command line
//
// Local Variables
// ---------------
// ifstream inStream;
// ofstream outStream;   
// int port_number;
// int sock;
// int con;
// struct sockaddr_in servAddr;
// DIR *d;
// struct dirent *dir;
// char *f;
// struct stat file_stat;
// char buffer[100];
// string userInput1 = "";
// string userInput2 = "";
// char readData[1024];
//
//*******************************************************************
int main(int argc, char* argv []) 
{
    // Ensuring correct number of arguments
    if (argc != 3)
    {
        printf("Enter only the filename, IP address and port number");
        return 0;
    }

    ifstream inStream;
    ofstream outStream;   
    int port_number;
    port_number = atoi(argv[2]);
    int sock;
    int con;
    struct sockaddr_in servAddr;
    DIR *d;
    struct dirent *dir;
    char *f;
    struct stat file_stat;

    // Creating socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Failed to create a socket\n");
        exit(1);
    }

    // Setting up server
    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = port_number;

    // Attemping to connect to server
    con = connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if(con < 0){
        printf("Failed to connect to server");
        exit(1);
    }

    printf("Connection successfully established with the ftp server\n");

    while(1){
        
        char buffer[100];
        string userInput1 = "";
        string userInput2 = "";
        char readData[1024];
        printf("\nPlease select one of the following: \nls = Show files on server\n \"d\" = followed by file number = downloads file from server\n \"u\" = followed by file numer = uploads file from server\n \"bye b\" = Close connection\n\n");
        cin >> userInput1 >> userInput2;

        // Checking if user wants to see server file list
        if((userInput1 == "ls") && (userInput2 == "server")) 
        {
            printf("You have selected to view all files on the server:\n");
            strcpy(buffer, "ls");
            if(sendto(sock, buffer, sizeof(buffer - 1), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
            {
                printf("Failed to send command");
                exit(1);
            }

            // Read file list from server
            int readBytes = read(sock, readData, sizeof(readData) - 1);
            printf("%s\n", readData);
            continue;
        }

        // Checking if user wants to see client file list
        if((userInput1 == "ls") && (userInput2 == "client")) 
        {
            printf("You have selected to view all files on the client:\n");
            string files;

            // Looping through client directory
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
            // Open file or create it 
            outStream.open("files.txt");
            if(outStream.fail())
            {
                inStream.open("files.txt");
                if(inStream.fail())
                {
                    cout << "ERROR";
                } else{
                    inStream >> files;
                }
            } else {
                outStream << files;
            }
            
            inStream.close();
            outStream.close();

            cout << "Files:\n" << files << endl;
            continue;
        }

        // Checking if user wants to download a file
        else if (userInput1 == "d")
        {
            char filename [100];
            int size;
            cout << "You have selected to download file number " << userInput2 << "\n";
            strcpy(buffer, "d ");
            char cstr[userInput2.size() + 1];
            strcpy(cstr, userInput2.c_str());
            strcat(buffer, cstr);

            // Send command
            if(sendto(sock, buffer, sizeof(buffer - 1), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
            {
                printf("Failed to send command");
                exit(1);
            }

            // Get file name
            int name = read(sock, filename, sizeof(filename) - 1);
            string fn(filename);
            if(fn == "")
            {
                cout << "File doesn't exist on server:\n";
                break;
            }

            // Get file size
            int fileSize = read(sock, &size, sizeof(size) - 1);

            // Open file 
            int file = open(filename, O_RDWR | O_TRUNC | O_CREAT);
            malloc(size);
            write(file, malloc(size), size);
            cout << "File " << filename << " downloaded successfully. " << fileSize << " bytes recieved.\n";
            close(file);
        } 

        // Checking if user wants to upload a file
        else if(userInput1 == "u")
        {
            cout << "You have selected to upload file number " << userInput2 << "\n";
            string uploadFile = "";
            // Looping through client directory
            if ((d = opendir(".")) != NULL)
            {
                int i = 1;
                while ((dir = readdir(d)) != NULL)
                {
                    if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )
                    {
                        // do nothing 
                    } else if (std::to_string(i) == userInput2) {
                        cout << std::to_string(i);
                        uploadFile.append(dir->d_name);
                        break;
                    } else {
                        i++;
                    }      
                }
                closedir(d);
            }
            strcpy(buffer, "u ");
            char cstr[uploadFile.size() + 1];
            strcpy(cstr, uploadFile.c_str());
            stat(cstr, &file_stat);
            strcat(buffer, cstr);
            // Send command with file name
            sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));

            char cstr2[uploadFile.size() + 1];
            strcpy(cstr2, uploadFile.c_str());
            int fd = open(cstr2, O_RDONLY);
            if(fd < 0)
            {
                cout << "ERROR";
                exit(1);
            }
            int fileSize = file_stat.st_size;
            
            // Send file size
            sendto(sock, &fileSize, sizeof(int), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
            // Send actual file
            sendfile(sock, fd, NULL, fileSize); 
            cout << "File " << uploadFile << " uploaded successfully. " << fileSize << " bytes sent. \n";
        }
        else if(userInput1 == "bye")
        {
            cout << "Bye!";
            close(sock);
            exit(1);
        }
        else
        {
            cout << "Invalid Input\n";
        }
    }

}