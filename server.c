#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <poll.h>

#define MAX 80
#define PORT 8082
#define SA struct sockaddr
#define TCP_PACKET_MAX_SIZE 65536
#define MESSAGE_LOG "message_log.txt"
#define SERVER_LOG "server_log.log"


static int loopCounter = 1;

void write_into_file(char msg[], char *file_name);

void handle_sigint(int sig);

static void demonizing();

void handling_signals_part();

void process_packet_read(int client_socket, char *text, size_t length) {
    size_t readed = read(client_socket, text, length);
    if (readed != length) {
        printf("read wrong number on bytes. Read: %s; Need %s", readed, length);
    };
}

void client_server_interaction(const int client_socket) {
    printf("client_server_interaction\n");
    int length;
    // read the length of the text from the socket. If read returns zero, the client closed the connection.
    if (read(client_socket, &length, sizeof(length)) == 0) {
        printf("packet size is zero - client closed the connection");
        exit(EXIT_SUCCESS);
    }

    // alocate memory for all text
    char *buff = malloc(sizeof(*buff) * (length + 1));

    if (length < TCP_PACKET_MAX_SIZE) { // if file text size < max tcp packet size
        process_packet_read(client_socket, buff, length);
    } else {
        size_t num_of_packets = length / TCP_PACKET_MAX_SIZE + 1;
        size_t last_pack_size = length - TCP_PACKET_MAX_SIZE * (num_of_packets - 1);

        for (int i = 0; i < num_of_packets; ++i) {
            if (i != num_of_packets - 1) { // if not the last packet
                char *text = malloc(sizeof(*text) * (TCP_PACKET_MAX_SIZE + 1));

                process_packet_read(client_socket, text, TCP_PACKET_MAX_SIZE);

                memcpy(buff + (i * TCP_PACKET_MAX_SIZE), text, TCP_PACKET_MAX_SIZE * sizeof(char));
                free(text);
            } else { // if the last packet
                char *text = malloc(sizeof(*text) * (last_pack_size + 1));

                process_packet_read(client_socket, text, last_pack_size);
                memcpy(buff + (i * TCP_PACKET_MAX_SIZE), text, last_pack_size * sizeof(char));
                free(text);
            }
        }
    } // end of - if file text size < max tcp packet size

    write_into_file(buff, MESSAGE_LOG);
    free(buff);
}

int main() {
    demonizing();

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    size_t msg_len;

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

    handling_signals_part();

    // Accept the data packet from client and verification
    while (loopCounter) {
        connfd = accept(sockfd, (SA *) &cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            break;
        } else
            printf("server accept the client...\n");

        // Function for interaction between client and server
        client_server_interaction(connfd);
    }

    printf("closing the socket");
    close(sockfd);
    return 0;
}

static void demonizing() {
    const int SIGNALS_TO_HOLD[] = {SIGINT, SIGTERM, SIGHUP, SIGCHLD};

    pid_t pid;

    //     Fork off the parent process
    pid = fork();
    //     An error occurred
    if (pid < 0) {
        write_into_file("An error occurred on creating fork", SERVER_LOG);
        exit(EXIT_FAILURE);
    }
    //     Success: Let the parent terminate
    if (pid > 0) exit(EXIT_SUCCESS);
    //     On success: The child process becomes session leader
    if (setsid() < 0) exit(EXIT_FAILURE);

    //     Fork off for the second time
    pid = fork();
    //     An error occurred
    if (pid < 0) exit(EXIT_FAILURE);
    //     Success: Let the parent terminate
    if (pid > 0) exit(EXIT_SUCCESS);

    //     Close all open file descriptors
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }
}


void handle_sigint(int sig) {
    printf("The signal was: %d\n", sig);
    loopCounter = 0;
}

void handling_signals_part() {
    const int SIGNALS_TO_HOLD[] = {SIGINT, SIGTERM, SIGHUP, SIGCHLD};
    for (int i = 0; i < sizeof(SIGNALS_TO_HOLD) / sizeof(SIGNALS_TO_HOLD[0]); ++i) {
        signal(SIGNALS_TO_HOLD[i], handle_sigint);
    }
}

void write_into_file(char *msg, char*file_name) {
    FILE *fp;

    fp = fopen(file_name, "a");
    fprintf(fp, msg);
    fclose(fp);
}
