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
#define PORT 8081
#define SA struct sockaddr

static int loopCounter = 1;

void write_into_file(char msg[]);


void handle_sigint(int sig);

static void demonizing();

void handling_signals_part();


char *read_from_socket(int sock) {
    char *buffer = malloc(1024);
    int len = 0;
    ioctl(sock, FIONREAD, &len);
    if (len > 0) {
        len = read(sock, buffer, len);
    }
    return buffer;
}

char *read_from_socket2(int sock) {
    char buffer[1024];
    int ptr = 0;
    ssize_t rc;

    struct pollfd fd = {
            .fd = sock,
            .events = POLLIN
    };

    poll(&fd, 1, 0); // Doesn't wait for data to arrive.
    while (fd.revents & POLLIN) {
        rc = read(sock, buffer + ptr, sizeof(buffer) - ptr);

        if (rc <= 0)
            break;

        ptr += rc;
        poll(&fd, 1, 0);
    }

    printf("Read %d bytes from sock.\n", ptr);
}

char *read_from_socket3(int client_socket) {
    //     Read text from the socket and print it out. Continue until the socket closes.
    int length;
    char *text;
    //First, read the length of the text message from the socket. If read returns zero, the client closed the connection.
    if (read(client_socket, &length, sizeof(length)) == 0)
        return 0;
     // Allocate a buffer to hold the text.
    text = (char *) malloc(length);
    //Read the text itself, and print it.
    read(client_socket, text, length);
    printf("%s\n", text);
    // Free the buffer.
    return text;
}

char *read_from_socket0(int _sockfd) {
    char buff[MAX];
    bzero(buff, MAX);
    if (read(_sockfd, buff, sizeof(buff)) != sizeof(buff)) {
        printf("read a different number of bytes than expected");
        exit(EXIT_FAILURE);
    };

    return buff;
}


void client_server_interaction(const int *_sockfd) {
    char buff[MAX];
    bzero(buff, MAX);
    read(_sockfd, buff, sizeof(buff));

    // read the message from client and copy it in buffer
//    char * buff = read_from_socket(_sockfd);
//    char *buff = read_from_socket2(_sockfd);
//    char *buff = read_from_socket2(_sockfd);
//    char *buff = read_by_fd(_sockfd);
    printf("Client message: %s\n\n", buff);

    write_into_file(buff);
    // print buffer which contains the client contents
    bzero(buff, MAX);
}

int main() {
//    demonizing();

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

    // ----------------------
    handling_signals_part();


    char buff[MAX];
    int n;

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
    if (pid < 0) exit(EXIT_FAILURE);
    //     Success: Let the parent terminate
    if (pid > 0) exit(EXIT_SUCCESS);
    //     On success: The child process becomes session leader
    if (setsid() < 0) exit(EXIT_FAILURE);

    //    handle signals
//    for (int i = 0; i < sizeof(SIGNALS_TO_HOLD) / sizeof(SIGNALS_TO_HOLD[0]); ++i) {
//        int ans;
//        ans = signal(SIGNALS_TO_HOLD[i], handle_sigint);
//    }

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

void write_into_file(char msg[]) {
    FILE *fp;

    fp = fopen("message_log.txt", "a");
    fprintf(fp, msg);
    fputs("\n", fp);
    fclose(fp);
}
