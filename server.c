#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include<signal.h>

#define MAX 80
#define PORT 8081
#define SA struct sockaddr

void write_into_file(char msg[]) {
    FILE *fp;

    fp = fopen("message_log.txt", "a");
    fprintf(fp, msg);
    fputs("\n", fp);
    fclose(fp);
}

void client_server_interaction(int sockfd) {
    char buff[MAX];
    // infinite loop for chat
//    for (;;) {
    bzero(buff, MAX);

    // read the message from client and copy it in buffer
    read(sockfd, buff, sizeof(buff));
    printf("Client message: %s\n\n", buff);
    write_into_file(buff);
    // print buffer which contains the client contents
    bzero(buff, MAX);

    // if msg contains "Exit" then server exit and chat ended.
//    }
}

void handle_sigint(int sig, int socket) {
    printf("The signal was: %d\n", sig);
    close(socket);
    exit(sig);
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else
        printf("Server listening..\n");
    len = sizeof(cli);


    // signal handling
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
    signal(SIGHUP, handle_sigint);

    // Accept the data packet from client and verification
    for(;;) {
        connfd = accept(sockfd, (SA *) &cli, &len);
        if (connfd < 0) {
            printf("server acccept failed...\n");
            break;
        } else
            printf("server acccept the client...\n");

        // Function for interaction between client and server
        client_server_interaction(connfd);
    }

    // closing the socket
    close(sockfd);
    return 0;
}
