#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 8082
#define SA struct sockaddr
//"input_file_little.txt"
#define INPUT_FILE_NAME "input_file.txt"
//#define INPUT_FILE_NAME "input_file_little.txt"
#define TCP_PACKET_MAX_SIZE 65536

char *read_file(char file_name[]) {
    FILE *f = fopen(file_name, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

void process_packet_length(int socket_fd, int length) {
    if (write(socket_fd, &length, sizeof(length)) != sizeof(length)) {
        printf("send a different number of bytes than expected");
        exit(EXIT_FAILURE);
    };
}

void process_packet(int socket_fd, const char *text, size_t length) {
    if (write(socket_fd, text, length) != length) {
        printf("send a different number of bytes than expected");
        exit(EXIT_FAILURE);
    };
}

void write_text(int socket_fd, const char *text) {
    //     Write the number of bytes in the string, including NUL-termination.
    int length = strlen(text) + 1;
    //     Write the length
    process_packet_length(socket_fd, length);

    //     Write the message
    if (strlen(text) < TCP_PACKET_MAX_SIZE) { // if file text size < max tcp packet size
        process_packet(socket_fd, text, length);
    } else {
        size_t num_of_packets = length / TCP_PACKET_MAX_SIZE + 1;
        size_t last_pack_size = length - TCP_PACKET_MAX_SIZE * (num_of_packets - 1);

        for (int i = 0; i < num_of_packets; ++i) {
            if (i != num_of_packets - 1) { // if not the last packet
                char packet[TCP_PACKET_MAX_SIZE];
                bzero(packet, TCP_PACKET_MAX_SIZE);
                // copy part of main array
                memcpy(packet, text + (i * TCP_PACKET_MAX_SIZE), TCP_PACKET_MAX_SIZE * sizeof(char));

                process_packet(socket_fd, packet, TCP_PACKET_MAX_SIZE);
            } else { // if the last packet
                char packet[last_pack_size];
                bzero(packet, last_pack_size);
                // copy part of main array
                memcpy(packet, text + (i * TCP_PACKET_MAX_SIZE), last_pack_size * sizeof(char));

                process_packet(socket_fd, packet, last_pack_size);
            }
        }
    }
}

void client_server_interaction(int sockfd, char *input_file_name) {
    char *buff = read_file(input_file_name);

    write_text(sockfd, buff);
    free(buff);
}

void handle_input(int argc, char *argv[], char **input_file_name) {
    if (argc < 2) {
        fprintf(stderr, "You forget to set input_file_name");
        exit(EXIT_FAILURE);
    } else {
        *input_file_name = argv[1];
    }
}

int main(int argc, char *argv[]) {
    // setting input file name
    char *input_file_name = NULL;
    handle_input(argc, argv, &input_file_name);

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
    client_server_interaction(sockfd, input_file_name);

    // close the socket
    close(sockfd);
}
