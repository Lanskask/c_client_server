#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 8081
#define SA struct sockaddr


char *read_file(char file_name[]) {
    FILE *f = fopen("input_file.txt", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

void write_text(int socket_fd, const char *text) {
    /* Write the number of bytes in the string, including
    NUL-termination. */
    int length = strlen(text) + 1;
    //     Write the length
    if (write(socket_fd, &length, sizeof(length)) != sizeof(length)) {
        printf("send a different number of bytes than expected");
        exit(EXIT_FAILURE);
    };

    //     Write the message
    if (write(socket_fd, text, length)  != length) {
        printf("send a different number of bytes than expected");
        exit(EXIT_FAILURE);
    };
}


void client_server_interaction(int sockfd) {
    char *buff = read_file("input_file.txt");

    write_text(sockfd, buff);
    free(buff);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else
        printf("connected to the server..\n");

    // function for chat
    client_server_interaction(sockfd);

    // close the socket
    close(sockfd);
}
