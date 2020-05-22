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

static int loopCounter = 1;

void write_into_file(char msg[]);
void client_server_interaction(int _sockfd);
void handle_sigint(int sig);
static void demonizing();

/* Exit handler function called by sigaction */
void exit_handler(int sig, siginfo_t *siginfo, void *ignore) {
    printf("*** Got %d signal from %d\n", siginfo->si_signo, siginfo->si_pid);
    loopCounter = 0;

    return;
}

void handling_signals_part() {
    struct sigaction sig_action;

    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = exit_handler;

    const int SIGNALS_TO_HOLD[] = {SIGINT, SIGTERM, SIGHUP, SIGCHLD};
    for (int i = 0; i < sizeof(SIGNALS_TO_HOLD) / sizeof(SIGNALS_TO_HOLD[0]); ++i) {
        sigaction(SIGNALS_TO_HOLD[i], &sig_action, 0);
    }
    signal(SIGTERM, handle_sigint);
}

void handling_signals_part2() {
    const int SIGNALS_TO_HOLD[] = {SIGINT, SIGTERM, SIGHUP, SIGCHLD};
    for (int i = 0; i < sizeof(SIGNALS_TO_HOLD) / sizeof(SIGNALS_TO_HOLD[0]); ++i) {
        signal(SIGNALS_TO_HOLD[i], handle_sigint);
    }
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
//    handling_signals_part();
    handling_signals_part2();

    // Accept the data packet from client and verification
    while(loopCounter) {
        printf(".");
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

void client_server_interaction(int _sockfd) {
    char buff[MAX];
    // infinite loop for chat
    bzero(buff, MAX);

    // read the message from client and copy it in buffer
    read(_sockfd, buff, sizeof(buff));
    printf("Client message: %s\n\n", buff);
    write_into_file(buff);
    // print buffer which contains the client contents
    bzero(buff, MAX);
}
void write_into_file(char msg[]) {
    FILE *fp;

    fp = fopen("message_log.txt", "a");
    fprintf(fp, msg);
    fputs("\n", fp);
    fclose(fp);
}
void handle_sigint(int sig) {
    printf("The signal was: %d\n", sig);
    loopCounter = 0;
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
