#include <errno.h>
#include <fcntl.h>      // for opening socket
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // for closing socket
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdbool.h>
#define KGRN "\x1B[0;32m"    //Green
#define KYEL "\x1B[0;33m"    // Yellow
#define WHITE_L "\x1B[1;37m" // White
#define KRED "\x1B[0;31m"    // Red
#define TRUE                    1
#define FALSE                   0
#define MAX_PENDING_CONNECTIONS 4
#define MAX_CONNECTIONS         32
#define BUFFER_SIZE             1024

static inline int max(int a, int b) 
{
    return (a > b) ? a : b;
}

int acceptConnections(int tcpSocketFileDescriptor, int udpSocketFileDescriptor);
int registerNewSocket(int clientSocketFD, int* clientSocketFileDescriptors, int n);

bool check_per(char* str)
{
	int len = strlen(str);
	if(len==6)
	{
		bool flag = true;
		for(int i=0;i<6;i++)
		{
		   if(i%3==0) // read
		   {
			   if(!(str[i]=='r'||str[i]=='-'))
			   {
                    flag = false;
			   }
		   }
		   else if(i%3==1) // write 
		   {
			   if(!(str[i]=='w'||str[i]=='-'))
			   {
                    flag = false;
			   }
		   }
		   else if(i%3==2) // execute
		   {
			   if(!(str[i]=='x'||str[i]=='-'))
               {
                    flag = false;
               }
		   }
		}
		if(flag==true) return true;
		else return false;
	}
	else
	{
		return false;
	}
}


int main(int argc, char* argv[]) {
    if ( argc != 2 ) {
        fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
        return EXIT_FAILURE;
    } 

    int portNumber = atoi(argv[1]);
    if ( portNumber <= 0 ) {
        fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
        return EXIT_FAILURE;
    }


    int tcpSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int udpSocketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if ( tcpSocketFileDescriptor == -1 || udpSocketFileDescriptor == -1 ) {
        fprintf(stderr, "[ERROR] Failed to create socket: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }


    socklen_t sockaddrSize = sizeof(struct sockaddr);
    struct sockaddr_in serverSocketAddress;
    bzero(&serverSocketAddress, sockaddrSize);
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverSocketAddress.sin_port = htons(portNumber);


    int optionValue = 1;
    setsockopt(tcpSocketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));

    

    if ( bind(tcpSocketFileDescriptor, (struct sockaddr*)(&serverSocketAddress), sockaddrSize) == -1) {
        fprintf(stderr, "[ERROR] Failed to bind TCP socket file descriptor to specified address: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if ( bind(udpSocketFileDescriptor, (struct sockaddr*)(&serverSocketAddress), sockaddrSize) == -1) {
        fprintf(stderr, "[ERROR] Failed to bind UDP socket file descriptor to specified address: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }


    if ( listen(tcpSocketFileDescriptor, MAX_PENDING_CONNECTIONS) == -1 ) {
        fprintf(stderr, "[ERROR] Failed to listen to the TCP socket: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    /*
     * Prepare for handling TCP and UDP connections using select.
     */
    int exitCode = acceptConnections(tcpSocketFileDescriptor, udpSocketFileDescriptor);
    if ( exitCode == -1 ) {
        fprintf(stderr, "[ERROR] Server exit with an error: %s\n", strerror(errno));
    }

    /*
     * Close Sockets.
     */
    close(tcpSocketFileDescriptor);
    close(udpSocketFileDescriptor);

    return EXIT_SUCCESS;
}


int acceptConnections(int tcpSocketFileDescriptor, int udpSocketFileDescriptor) {
    /**
     * The file descriptor used to check readability, if characters become available for reading.
     */
    fd_set readFileDescriptorSet;

    /**
     * Buffers for sending and receiving data.
     */
    char inputBuffer[BUFFER_SIZE] = {0};
    char outputBuffer[BUFFER_SIZE] = {0};

    /**
     * Client sockets.
     */
    socklen_t sockaddrSize = sizeof(struct sockaddr);
    struct sockaddr_in clientSocketAddress;
    int clientSocketFileDescriptors[MAX_CONNECTIONS] = {0};

    /**
     * Handle TCP and UDP connections.
     */
    while ( TRUE ) {
        /**
         * Clear the read file descriptor set.
         */
        FD_ZERO(&readFileDescriptorSet);

        /**
         * Add file descriptor of TCP and UDP to the set.
         */
        FD_SET(tcpSocketFileDescriptor, &readFileDescriptorSet);
        FD_SET(udpSocketFileDescriptor, &readFileDescriptorSet);
        int maxFileDescriptor = max(tcpSocketFileDescriptor, udpSocketFileDescriptor);

        /**
         * Add valid socket file descriptor to the read set.
         */
        int i = 0;
        for ( i = 0; i < MAX_CONNECTIONS; ++ i ) {
            int clientSocketFD = clientSocketFileDescriptors[i];

            if ( clientSocketFD > 0 ) {
                FD_SET(clientSocketFD, &readFileDescriptorSet);
            }
            if ( clientSocketFD > maxFileDescriptor ) {
                maxFileDescriptor = clientSocketFD;
            }
        }


        int events = select(maxFileDescriptor + 1, &readFileDescriptorSet, NULL, NULL, NULL);
        if ( events == -1 && errno != EINTR ) {
            fprintf(stderr, "[ERROR] An error occurred while monitoring sockets: %s\n", strerror(errno));
        }

        // New incoming TCP connection
        if ( FD_ISSET(tcpSocketFileDescriptor, &readFileDescriptorSet) ) {
            // Establish connection with client
            int clientSocketFD = accept(tcpSocketFileDescriptor, (struct sockaddr *)(&clientSocketAddress), &sockaddrSize);

            if ( clientSocketFD == -1 ) {
                fprintf(stderr, "[WARN][TCP] Failed to accpet a socket from client: %s\n", strerror(errno));
                continue;
            }
            fprintf(stderr, KGRN"[INFO][TCP] Connection established with %s:%d\n", 
                inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port));

            // Register file descriptors for sockets
            int clientSocketFDIndex = registerNewSocket(clientSocketFD, clientSocketFileDescriptors, MAX_CONNECTIONS);
            if ( clientSocketFDIndex == -1 ) {
                fprintf(stderr, "[WARN][TCP] Failed to register the socket for client: %s:%d\n", 
                    inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port));
            } else {
                fprintf(stderr, "[INFO][TCP] Socket #%d registered for the client: %s:%d\n", 
                    clientSocketFDIndex, inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port));
            }
        }
        
        // New incoming UDP connection
        if ( FD_ISSET(udpSocketFileDescriptor, &readFileDescriptorSet) ) {
            // Receive a message from client
            int readBytes = recvfrom(udpSocketFileDescriptor, inputBuffer, BUFFER_SIZE, 0, (struct sockaddr *)(&clientSocketAddress), &sockaddrSize);

            if ( readBytes < 0 ) {
                fprintf(stderr, "[ERROR][UDP] An error occurred while sending message to the client %s:%d: %s\nThe connection is going to close.\n", 
                    inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port), strerror(errno));
                continue;
            }
            fprintf(stderr,WHITE_L "[INFO][UDP] Received a message from client %s:%d: %s\n", 
                inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port), inputBuffer);

            // Send a message to client
            
            // send cookie back to client
            if ( sendto(udpSocketFileDescriptor, inputBuffer, strlen(inputBuffer), 0, (struct sockaddr *)(&clientSocketAddress), sockaddrSize) == -1 ) {
                fprintf(stderr, "[ERROR][UDP] An error occurred while sending message to the client %s:%d: %s\nThe connection is going to close.\n", 
                    inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port), strerror(errno));
            }
        }

        // IO operation on some other sockets
        for ( i = 0; i < MAX_CONNECTIONS; ++ i ) {
            int clientSocketFD = clientSocketFileDescriptors[i];

            if ( FD_ISSET(clientSocketFD, &readFileDescriptorSet) ) {
                // Receive a message from client
                int readBytes = recv(clientSocketFD, inputBuffer, BUFFER_SIZE, 0);
                
                if ( readBytes < 0 ) {
                    fprintf(stderr, "[ERROR][TCP] An error occurred while sending message to the client %s:%d: %s\nThe connection is going to close.\n", 
                        inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port), strerror(errno));
                    continue;
                }
                fprintf(stderr, WHITE_L "[INFO][TCP] Received a message from client %s:%d: %s\n", 
                    inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port), inputBuffer);

                // Handler for TCP messages
                if ( readBytes == 0 || strcmp("BYE", inputBuffer) == 0 ) {
                    // Complete receiving message from client
                    close(clientSocketFD);
                    clientSocketFileDescriptors[i] = 0;
                    fprintf(stderr,KRED  "[INFO][TCP] Client %s:%d disconnected.\n", 
                        inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port));

                    continue;
                } else if ( strncmp("GET", inputBuffer, 3) ==0 ) {
                    // Send file stream to the client
                    char filePath[BUFFER_SIZE] = {0};
                    memcpy(filePath, &inputBuffer[4], strlen(inputBuffer) - 4);
                    FILE* inputFile = fopen(filePath, "rb");

                    char* pMessage = "ACCEPT";
                    if ( inputFile == NULL ) {
                        pMessage = "REJECT";
                    }
                    send(clientSocketFD, pMessage, strlen(pMessage), 0);

                    // Send file stream
                    int readBytes = 0;
                    while ( (readBytes = fread(outputBuffer, sizeof(char), BUFFER_SIZE, inputFile)) > 0 ) {
                        send(clientSocketFD, outputBuffer, readBytes, 0);
                        fprintf(stderr, "[INFO] Sent %lu bytes\n", strlen(outputBuffer));
                        memset(outputBuffer, 0, BUFFER_SIZE);
                    }
                    fclose(inputFile);
                    fprintf(stderr, "[INFO][TCP] Send file stream to client %s:%d: %s\n", 
                        inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port), inputBuffer);
                } else 
                {
                    // Send a message to client
                    // send back what unique message (cookie) which client send to server

                    /* check inputBuffer type -> str */
                    char *delim = " ";
                    char *pch;


                    int cnt = 0; // counter for token number // 

                    pch = strtok(inputBuffer,delim);


                    char arg1[80],arg2[80],arg3[80];

                    while(pch!=NULL)
                    {

                        if(cnt==0)
                        {
                            strcpy(arg1,pch);
                        }
                        else if(cnt==1)
                        {
                            strcpy(arg2,pch);
                        }
                        else if(cnt==2)
                        {
                            strcpy(arg3,pch);
                        }   
                        pch = strtok(NULL,delim);
                        cnt++;

                    }
                    if(cnt==3)
                    {
                        printf("Right argc.\n");

                        if(strcmp(arg1,"Create")==0)
                        {
                            printf(KYEL "owner create file and upload file to server side\n");
                            // Check permission format // 
                            if(check_per(arg3)==true)
                            {
                                printf(KGRN "Ok.\n");
                            }
                            else
                            {
                                printf(KRED "Not Ok.\n");
                            }
                        }
                        else if(strcmp(arg1,"Write")==0)
                        {
                           printf(KYEL "Check whether client has write permission.\n");
                           

                           if(strcmp(arg3,"o")==0||strcmp(arg3,"a")==0)
                           {
                                fprintf(stderr,KGRN  "Write/Upload\n");
                           }
                           else
                           {
                                fprintf(stderr,KRED "o/a\n");
                           }

                        }
                        else if(strcmp(arg1,"Changemode")==0)
                        {
                            printf(KYEL "Check whether client is an owerner.\n");
                            
                            if(check_per(arg3)==true)
                            {
                                printf(KGRN "Ok.\n");
                            }
                            else
                            {
                                printf(KRED "Not Ok.\n");
                            }
                        }
                        else
                        {
                            printf(KRED "Error Command\n");
                            printf(KYEL "[Usage]\n1)Create\n2)Read\n3)Write\n4)Changemode\n"); 
                        }
                    }
                    else if(cnt==2) // Read
                    {   
                        if(strcmp(arg1,"Read")==0)
                        {
                            fprintf(stderr,KYEL "Check whether client has read permission.\n");
                        }
                    }
                    else
                    {
                        printf(KRED "Error argc.\n");
                    }
                    if ( send(clientSocketFD, inputBuffer, strlen(inputBuffer), 0) == -1 ) 
                    {
                                clientSocketFileDescriptors[i] = 0;
                                fprintf(stderr,KRED "[ERROR] An error occurred while sending message to the client %s:%d: %s\nThe connection is going to close.\n", inet_ntoa(clientSocketAddress.sin_addr), ntohs(clientSocketAddress.sin_port), strerror(errno));
                    }
                }
            }
        }
    }
}

int registerNewSocket(int clientSocketFD, int* clientSocketFileDescriptors, int n) {
    int i = 0;
    for ( i = 0; i < n; ++ i ) {
        if ( clientSocketFileDescriptors[i] == 0 ) {
            clientSocketFileDescriptors[i] = clientSocketFD;
            
            return i;
        }
    }
    return -1;
}


