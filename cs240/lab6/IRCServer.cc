
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sstream>

#include "IRCServer.h"
#include "linked_list.h"

char * user;
char * password;
char * host;
char * args;

#define PASSWORD_FILE "password.txt"
LINKED_LIST * users;
//LINKED_LIST * usersInRoom;

typedef struct MESSAGE {
	char * user;
	char * message;
	int secs;
} MESSAGE;

typedef struct ROOM_DATA {
	MESSAGE messages[100];
	//LINKED_LIST * usersInRoom;
	int nmessages;
	int lastMessage;
	int nMessagesOverall;
} ROOM_DATA;

#define MAX_MESSAGES 100 
//int nmessages;
//int lastMessage;
//int nMessagesOverall;
MESSAGE messages[MAX_MESSAGES];


int nRooms;
int maxRooms = 50;
ROOM_DATA roomData[50];
LINKED_LIST * usersInRoom[50];
char * roomList[50];

int QueueLength = 5;

int
IRCServer::open_server_socket(int port) {

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);

	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			(char *) &optval, sizeof( int ) );

	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			(struct sockaddr *)&serverIPAddress,
			sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}

	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	return masterSocket;
}

	void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);

	initialize();

	while ( 1 ) {

		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,
				(struct sockaddr *)&clientIPAddress,
				(socklen_t*)&alen);

		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}

		// Process request.
		processRequest( slaveSocket );		
	}
}

	int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}

	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);

}

//
// Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//

	void
IRCServer::processRequest( int fd )
{
	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;

	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;

	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
			read( fd, &newChar, 1) > 0 ) {

		if (newChar == '\n' && prevChar == '\r') {
			break;
		}

		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}

	// Add null character at the end of the string
	// Eliminate last \r
	commandLineLength--;
	commandLine[ commandLineLength ] = 0;

	printf("RECEIVED: %s\n", commandLine);

	char* command = strtok(commandLine, " ");
	user = strtok(NULL, " ");
	password = strtok(NULL, " ");
	args = strtok(NULL, "");

	printf("command=%s\n", command);
	printf("user=%s\n", user);
	printf("password=%s\n", password );
	printf("args=%s\n", args);

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		getMessages(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else if(!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, args);
	}
    else if(!strcmp(command, "LIST-ROOMS")) {
        listRooms(fd, user, password, args);
    }
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}

	// Send OK answer
	//const char * msg =  "\n";
	//write(fd, msg, strlen(msg));

	close(fd);	
}

	void
IRCServer::initialize()
{
	FILE *file;
	file = fopen(PASSWORD_FILE, "a");

	//init users
	//usersInRoom = llist_create();
	users = llist_create();
	llist_read(users, PASSWORD_FILE);

	//init messages list
	for(int i = 0; i < maxRooms; i++) {
		//roomData[i].usersInRoom = llist_create();
		roomData[i].nmessages = 0;
		for(int j = 0; j < 100; j++) {
			roomData[i].messages[j].user = (char*)malloc(50*sizeof(char));
			roomData[i].messages[j].message = (char*)malloc(50*sizeof(char));
		}
		//roomData[i].nmessages = 1;
	}

	//init room list and userList
	for(int i = 0; i < maxRooms; i++) {
		usersInRoom[i] = llist_create();
		roomList[i] = (char*)malloc(50*sizeof(char));
	}

	nRooms = 0;
	/*	
		nmessages = 0;
		for (; nmessages < 100; nmessages++) {
		messages[nmessages].user = (char*)malloc(50*sizeof(char));
		messages[nmessages].message = (char*)malloc(50*sizeof(char));
		}
		nmessages = 1;
		*/
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {
	//check
	char test[100];
	if ((llist_lookup(users,(char*)user)) == NULL) {
		printf("password check NULL\r\n");
		return 0;
	}
	else {
		strcpy(test, llist_lookup(users, (char*)user));
		if ((strcmp(test, (char*)password)) == 0) {
			printf("password check YES\r\n");
			return 1;
		} else {
			printf("password check NO\r\n");
			return 0;
		}
	}
}

	void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args)
{
	llist_add(users, (char*)user, (char*)password);
	llist_sort(users, 1);
	llist_save(users, PASSWORD_FILE);
	const char * msg = "OK\r\n";
	write(fd, msg, strlen(msg));

	return;
}

	void
IRCServer::createRoom(int fd, const char * user, const char * password, const char * args)
{

	if ((checkPassword(fd, user, password)) == 1) { //check password
		//check if exists
		for(int i = 0; i < nRooms; i++) {
			if (!strcmp(roomList[i],args)) { //if room found
				const char * msg = "DENIED (room exists)\r\n";
				write(fd, msg, strlen(msg));
				return;
			}
		}

		//does not exist so create room and increment nRooms by 1
		roomList[nRooms] = strdup((char*)args);
		printf("%s\n", (char*)args);
		nRooms++;
		const char * msg = "OK\r\n";
		write(fd, msg, strlen(msg));
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
	}
	return;
}

void IRCServer::listRooms(int fd, const char * user, const char * password, const char * args) {
    if ((checkPassword(fd, user, password)) == 1) {
        for (int i = 0; i < nRooms; i++) {
            const char *msg = roomList[i];
            write(fd, msg, strlen(msg));
            write(fd, "\r\n", strlen("\r\n"));
        }
        write(fd, "\r\n", strlen("\r\n"));
        return;
    }
   /* 
    else {
        const char * msg = "ERROR (Wrong password)\r\n";
        write(fd, msg, strlen(msg));
    }
    */
    return;
}

	void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args)
{	
	if ((checkPassword(fd, user, password)) == 1) { //check password
		//get room number
		int rNum = -1;
		for(int i = 0; i < nRooms; i++) {
			printf("%s\n",roomList[i]);
			if(!strcmp(roomList[i],args)) { //if room is found
				rNum = i;	
				break;
			}
		}

		//if not found
		if (rNum == -1) {
			const char * msg = "ERROR (No room)\r\n";
			write (fd, msg, strlen(msg));
			return;
		}

		//else
		if(llist_lookup(usersInRoom[rNum],(char*)user) == NULL) { //if not in room
			if(!llist_add(usersInRoom[rNum],(char*)user, (char*)password)) { //add to usersInRoom
				//add to roomData	
				//int nm = roomData[rNum].nmessages;
				//ROOM_DATA r = roomData[rNum];
				//strcpy(r.messages[nm].user, user);
				//strcpy(r.messages[nm].message, "entered the room.\r\n");
				//r.messages[nm].secs = nm;
				//roomData[rNum].nmessages++;

				const char * msg = "OK\r\n";
				write(fd, msg, strlen(msg));
			}
			else {
				const char * msg = "DENIED\r\n";
				write(fd, msg, strlen(msg));
			}
		}
		else { //if found in room
			const char *msg = "OK\r\n";
			write(fd, msg, strlen(msg));
		}
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
	}
	return;
}

	void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{

	if ((checkPassword(fd, user, password)) == 1) { //check password
		//get room number
		int rNum = -1;
		for(int i = 0; i < nRooms; i++) {
			if(!strcmp(roomList[i],args)) { //if room is found
				rNum = i;	
				break;
			}
		}

		//if not found
		if (rNum == -1) {
			const char * msg = "DENIED (room does not exist)\r\n";
			write (fd, msg, strlen(msg));
			return;
		}

		//else 
		if(llist_lookup(usersInRoom[rNum],(char*)user) != NULL) { //if in room
			if((llist_remove(usersInRoom[rNum],(char*)user)) == 1) { //removed

				/*
				   int nm = roomData[rNum].nmessages;
				   ROOM_DATA r = roomData[rNum];
				   strcpy(r.messages[nm].user, user);
				   strcpy(r.messages[nm].message, "left the room\r\n");
				   r.messages[nm].secs = nm;
				   roomData[rNum].nmessages++;
				   */
				const char * msg = "OK\r\n";
				write(fd, msg, strlen(msg));
			}
		} else {
			const char * msg = "ERROR (No user in room)\r\n";
			write(fd, msg, strlen(msg));
		}
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
	}
	return;
}

	void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args)
{
	if ((checkPassword(fd, user, password)) == 1) { //check password
		//get room number and message
		char * room  = strtok((char*)args, " ");
		char * mess = strtok(NULL, "");

		//get room number
		int rNum = -1;
		for(int i = 0; i < nRooms; i++) {
			if(!strcmp(roomList[i],room)) { //if room is found
				rNum = i;	
				break;
			}
		}

		//if not found
		if (rNum == -1) {
			const char * msg = "DENIED (room does not exist)\r\n";
			write (fd, msg, strlen(msg));
			return;
		}

		if ((llist_lookup(usersInRoom[rNum], (char*)user)) != NULL) { //if found

			int nm = roomData[rNum].nmessages;
			ROOM_DATA r = roomData[rNum];
			strcpy(roomData[rNum].messages[nm].user, user);

			strcat(mess, "\r\n");
			//strcpy(r.messages[rNum].message, mess);
			roomData[rNum].messages[nm].message = strdup(mess);
			roomData[rNum].messages[nm].secs = nm;
			roomData[rNum].nmessages++;

			const char * msg = "OK\r\n";
			write(fd, msg, strlen(msg));

		} else {
			const char * msg = "ERROR (user not in room)\r\n";
			write (fd, msg, strlen(msg));
		}
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write(fd, msg, strlen(msg));
	}
	return;
}

	void
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args)
{

	if ((checkPassword(fd, user, password)) == 1) { //check password

		//get room number and message
		char * mess  = strtok((char*)args, " ");
		char * room = strtok(NULL, "");

		//get room number
		int rNum = -1;
		for(int i = 0; i < nRooms; i++) {
			if(!strcmp(roomList[i],room)) { //if room is found
				rNum = i;	
				break;
			}
		}

		//if not found
		if (rNum == -1) {
			const char * msg = "DENIED (room does not exist)\r\n";
			write (fd, msg, strlen(msg));
			return;
		}

		if ((llist_lookup(usersInRoom[rNum], (char*)user)) != NULL) { //if found

			char number[10];
			int i = 0;
			char c;
			int argy = atoi(mess);
			argy++;
			char * test;

			int nm = roomData[rNum].nmessages;
			ROOM_DATA r = roomData[rNum];

			if(argy > nm) {
				const char * msg = "NO-NEW-MESSAGES\r\n";
				write(fd, msg, strlen(msg));
			}
			else {
				for (i = argy-1; i < nm; i++) {
					snprintf(number, sizeof(number), "%d", i);
					if (i < 10)		write (fd, number, 1);
					else			write (fd, number, 2);
					write (fd, " ", 1);
					write (fd, r.messages[i].user, strlen(r.messages[i].user));
					write (fd, " ", 1);
					write (fd, r.messages[i].message, strlen(r.messages[i].message));
				}
				write (fd, "\r\n", strlen("\r\n"));
			}

		} else {
			const char * msg = "ERROR (User not in room)\r\n";
			write (fd, msg, strlen(msg));
		}
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write (fd, msg, strlen(msg));
	}
	return;
}

	void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
	if ((checkPassword(fd, user, password)) == 1) { //check password
		//get room number
		int rNum = -1;
		for(int i = 0; i < nRooms; i++) {
			if(!strcmp(roomList[i],(char*)args)) { //if room is found
				rNum = i;	
				break;
			}
		}

		//if not found
		if (rNum == -1) {
			const char * msg = "DENIED (room does not exist)\r\n";
			write (fd, msg, strlen(msg));
			return;
		}

		llist_sort(usersInRoom[rNum], 1);
		LINKED_LIST_ENTRY * e;
		e = usersInRoom[rNum]->head->next;
		while (e != usersInRoom[rNum]->head) {
			write(fd, e->name, strlen(e->name));
			write(fd, "\r\n", strlen("\r\n"));
			e = e->next;
		}
		write (fd, "\r\n", strlen("\r\n"));
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write (fd, msg, strlen(msg));
	}
	return;
}

	void
IRCServer::getAllUsers(int fd, const char * user, const char * password,const  char * args)
{
	if ((checkPassword(fd, user, password)) == 1) {
		LINKED_LIST_ENTRY * e;
		e = users->head->next;
		while (e != users->head) {
			write (fd, e->name, strlen(e->name));
			write (fd, "\r\n", strlen("\r\n"));
			e = e->next;
		}
		write (fd, "\r\n", strlen("\r\n"));
	} else {
		const char * msg = "ERROR (Wrong password)\r\n";
		write (fd, msg, strlen(msg));
		//write (fd, "\r\n", strlen("\r\n"));
	}
	return;
}

