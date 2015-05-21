
#include <time.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <gtk/gtk.h>

char * host;
char * user;
char * password;
char * sport;
int port;

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

int lastMessage = 0;

int open_client_socket(char * host, int port) {
    // Initialize socket address structure
    struct  sockaddr_in socketAddress;

    // Clear sockaddr structure
    memset((char *)&socketAddress,0,sizeof(socketAddress));

    // Set family to Internet 
    socketAddress.sin_family = AF_INET;

    // Set port
    socketAddress.sin_port = htons((u_short)port);

    // Get host table entry for this host
    struct  hostent  *ptrh = gethostbyname(host);
    if ( ptrh == NULL ) {
        perror("gethostbyname");
        exit(1);
    }

    // Copy the host ip address to socket address structure
    memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

    // Get TCP transport protocol entry
    struct  protoent *ptrp = getprotobyname("tcp");
    if ( ptrp == NULL ) {
        perror("getprotobyname");
        exit(1);
    }

    // Create a tcp socket
    int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    // Connect the socket to the specified server
    if (connect(sock, (struct sockaddr *)&socketAddress,
                sizeof(socketAddress)) < 0) {
        perror("connect");
        exit(1);
    }

    return sock;
}

int sendCommand(char * host, int port, char * command, char * user,
        char * password, char * args, char * response) {
    int sock = open_client_socket( host, port);

    // Send command
    write(sock, command, strlen(command));
    write(sock, " ", 1);
    write(sock, user, strlen(user));
    write(sock, " ", 1);
    write(sock, password, strlen(password));
    write(sock, " ", 1);
    write(sock, args, strlen(args));
    write(sock, "\r\n",2);

    // Keep reading until connection is closed or MAX_REPONSE
    int n = 0;
    int len = 0;
    while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
        len += n;
    }

    //printf("response:%s\n", response);

    close(sock);
}

void printUsage()
{
    printf("Usage: talk-client host port user password\n");
    exit(1);
}

void add_user() {
    // Try first to add user in case it does not exist.
    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "ADD-USER", user, password, "", response);

    if (!strcmp(response,"OK\r\n")) {
        printf("User %s added\n", user);
    }
}

void enter_room() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "ENTER-ROOM", user, password, "room", response);

    //printf("Response is %s\n", response);

    if(!strcmp(response, "OK\r\n")) {
        printf("User %s entered room %s\n", user, "room");
    }
    else if (!strcmp(response, "ERROR (Wrong password)\r\n")) {
        printf("Wrong password\n");
        exit(1);
    }
    else if (!strcmp(response, "ERROR (No room)\r\n")) {
        printf("No room\n");
        exit(1);
    }
    else {
        ; 
    }
}

void leave_room() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "LEAVE-ROOM", user, password, "", response);
}

void get_messages() {

}

void send_message(char * msg) {
}

void print_users_in_room() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "GET-USERS-IN-ROOM", user, password, "", response);
    char * s;
    s = strtok(response, "\r\n");
    while (s != NULL) {
        if (strlen(s) > 1) {
            printf("%s", s);
            printf("\n");
        }
        s = strtok(NULL, "\r\n");
    }
    memset(response, 0, 1000);
}

void print_users() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "GET-ALL-USERS", user, password, "", response);
    char * s;
    s = strtok(response, "\r\n");
    while(s != NULL) {
        printf("%s", s);
        printf("\n");
        s = strtok(NULL, "\r\n");
    }
}

void printPrompt() {
    printf("talk> ");
    fflush(stdout);
}

void printHelp() {
    printf("Commands:\n");
    printf(" -who   - Gets users in room\n");
    printf(" -users - Prints all registered users\n");
    printf(" -help  - Prints this help\n");
    printf(" -quit  - Leaves the room\n");
    printf("Anything that does not start with \"-\" will be a message to the chat room\n");
}

void * getMessagesThread(void * arg) {
    // This code will be executed simultaneously with main()
    // Get messages to get last message number. Discard the initial Messages

    printf("\ngetMessagesThread GO!\n");

    char i[10];
    sprintf(i, "%d", lastMessage);

    strcat(i," room");

    char response[MAX_RESPONSE];
    sendCommand(host, port, "GET-MESSAGES", user, password, i, response);

    //printf("\nCommand has been sent\n");

    char * s;
    s = strtok(response, "\r\n");
    while (s != NULL) {
        lastMessage++;
        int count = 0; //char count
        int s_count = 0; //space count

        while (s[count] != '\0') {
            if (s[count] == ' ' && s_count == 0) { //first space
                printf(": ");
                s_count++;
            }
            else if (s[count] == ' ' && s_count == 1) { //second space
                printf(" - ");
                s_count++;
            }
            else {
                printf("%c", s[count]); //word
            }
            count++;
        }
        //printf("%s", s);
        printf("\n");
        s = strtok(NULL, "\r\n");

        //lastMessage++;
        //s = strtok(NULL, "\r\n");
    }

    sprintf(i, "%d", lastMessage);

    while (1) {
        memset(response, 0, 1000);
        // Get messages after last message number received.

        strcat(i, " room");

        sendCommand(host, port, "GET-MESSAGES", user, password, i, response);

        if(strcmp(response, "NO-NEW-MESSAGES\r\n") != 0) { //if different
            //Print messages
            char * s;
            s = strtok(response, "\r\n");
            while (s != NULL) {
                lastMessage++;
                int count = 0; //char count
                int s_count = 0; //space count

                while (s[count] != '\0') {
                    if (s[count] == ' ' && s_count == 0) { //first space
                        printf(": ");
                        s_count++;
                    }
                    else if (s[count] == ' ' && s_count == 1) { //second space
                        printf(" - ");
                        s_count++;
                    }
                    else {
                        printf("%c", s[count]); //word
                    }
                    count++;
                }
                //printf("%s", s);
                printf("\n");
                s = strtok(NULL, "\r\n");
            }
        }

        sprintf(i, "%d", lastMessage);

        // Sleep for ten seconds
        printf("...\n");
        usleep(2*1000*1000);
    }
}

void startGetMessageThread()
{
    printf("\nCreating PThread\n");
    //seg fault here  ??

    pthread_t thread1, thread2; //thread variable

    pthread_create(&thread1, NULL, getMessagesThread, NULL);

    pthread_join(thread1, NULL);
}


int main(int argc, char **argv) {

    char line[MAX_MESSAGE_LEN+1];

    if (argc < 5) {
        printUsage();
    }

    host = argv[1];
    sport = argv[2];
    user = argv[3];
    password = argv[4];

    printf("\nStarting talk-client %s %s %s %s\n", host, sport, user, password);

    // Convert port to number
    sscanf(sport, "%d", &port);

    //create gui
    //
    //entry.c - text fields, messages, password
    //panned.c - split pane
    //radio.c - radio buttons
    //timer.c - time and countdown






    add_user();

    // Enter room
    enter_room();

    printf("\nEntered room and starting message thread\n");

    // Start message thread
    startGetMessageThread();

    while (1) {
        printPrompt();

        char * s = fgets(line,MAX_MESSAGE_LEN, stdin);
        if (s==NULL) {
            leave_room();
            printf("talk-client exiting...\n");
            exit(1);
        }

        if (!isatty(0)) {
            // If it is not a terminal, echo command as well
            printf("%s\n", line);
        }

        if (line[0]=='-') {
            // This is a command process it
            if (!strcmp(line,"-help")) {
                printHelp();
            }
            else if (!strcmp(line,"-quit")) {
                printf("talk-client exiting...\n");
                exit(1);
            }
            // Put other commands here
            else if (!strcmp(line,"-users")) {
                print_users();
            }
            else if (!strcmp(line,"-who")) {
                print_users_in_room();
            }
            else {
                printf("Wrong command\n");
            }
        }
        else if (line[0]==0) {
            // Empty line. Print help
            printf("Type -help to print the available commands\n");
        }
        else {
            // Send message
            char response[MAX_RESPONSE];
            sendCommand(host,port,"SEND-MESSAGE", user, password, line, response);
        }
    }

    printf("TEST ENDS\n");
    return 0;
}
