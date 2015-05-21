
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

#include "gui.h"

int lastMessage = 0;

char * host;
char * sport;
char * user = "user"; //default
char * password = "password"; //default
char * room = "default"; //default
int port;
GtkWidget *tree_view_room;
GtkWidget *messages, *myMessage;
GtkWidget *view, *view_user;
GtkListStore * list_rooms;
GtkListStore * list_users;

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

typedef struct loginInfo {
    GtkWidget *window;
    GtkWidget * entry1;
    GtkWidget * entry2;
} loginInfo;

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
    return(0);
}

void printUsage()
{
    printf("Usage: talk-client host port\n");
    exit(1);
}

//add user
void add_user() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "ADD-USER", user, password, "", response);

    if(!strcmp(response,"OK\r\n")) {
        g_print("User %s added\n", user);
    }
}

//create room
void create_room() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "CREATE-ROOM", user, password, room, response);   

    g_print("%s\n", response);
}

//enter room
void enter_room() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "ENTER-ROOM", user, password, room, response);

    g_print("%s\n", response);
}

//leave room
void leave_room() {
    char response[MAX_RESPONSE];
    sendCommand(host, port, "LEAVE-ROOM", user, password, room, response);

    //set message counter back to 0
    lastMessage = 0;

    //clear message box
    clear_text(); 
}

//send message
void send_message(char * msg) {
    if(msg == NULL || strcmp(msg, "") == 0) {
        //invalid message
        return;
    }

    char * s = strtok(msg, "\n"); // no new lines allowed
    char response[MAX_RESPONSE];

    while (s != NULL) {
        memset(response, 0, sizeof response); //clear
        if (strcmp(s, "") == 0) { //if no text

        }
        else {
            //prep
            char * message = strdup(room);
            strcat(message, " ");
            strcat(message, s);

            //send
            sendCommand(host, port, "SEND-MESSAGE", user, password, message, response);
        }
        s = strtok(NULL, "\n");
    }
}

//GUI BELOW

int roomN = 0;

//get list of rooms
void update_list_rooms(char * msg) {
    GtkTreeIter iter;
    //int i;
    char * msg2 = strdup(msg);
    int msgN = 0;

    char * n = strtok(msg2, "\r\n");
    while (n != NULL && strcmp(n, "ERROR (Wrong password)") != 0) {
        msgN++;
        n = strtok(NULL, "\r\n");
    }

    //g_print("mn: %d rn: %d\n",msgN, roomN);
    while (msgN > roomN) {
        gchar * s;
        s = strtok(msg, "\r\n");

        if (roomN != 0) { //not initiating
            int i = 0;
            while (i < roomN) {
                s = strtok(NULL, "\r\n");
                i++;
            }
        }

        while(s != NULL && msgN >= roomN) {
            gchar *m = g_strdup_printf(s);
            gtk_list_store_append(GTK_LIST_STORE(list_rooms), &iter);
            gtk_list_store_set(GTK_LIST_STORE(list_rooms), &iter, 0, m, -1);
            roomN++;
            s = strtok(NULL, "\r\n");
        }
    }
}

//get list of users in room
void update_list_users(char * msg) {
    GtkTreeIter iter;

    gtk_list_store_clear(GTK_LIST_STORE(list_users));
    //g_print("list: \n %s\n", msg); 

    gchar* s = strtok(msg, "\r\n");
    while (s != NULL) {
        gchar *m = g_strdup_printf("%s", s);
        gtk_list_store_append(GTK_LIST_STORE(list_users), &iter);
        gtk_list_store_set(GTK_LIST_STORE(list_users), &iter, 0, m, -1);
        g_free(m);
        s = strtok(NULL, "\r\n");
    }
}
//int i = 0;
void update_messages() {
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

    char messageCount[100];

    //format request
    sprintf(messageCount, "%d", lastMessage);
    strcat(messageCount, " ");
    strcat(messageCount, room);

    char messageResponse[MAX_RESPONSE];
    sendCommand(host, port, "GET-MESSAGES", user, password, messageCount, messageResponse);

    //g_print("response:\n %s\n", messageResponse);

    char * s;
    s = strtok(messageResponse, "\r\n");

    //g_print("initial strtok parse\n");

    char m[100];

    while (s != NULL && strcmp(s, "") != 0 && strcmp(s, "NO-NEW-MESSAGES") != 0) {
        memset(m, 0, sizeof m);

        lastMessage++;
        int count = 0;
        int s_count = 0;

        //g_print("about to start looping string\n");

        while (s[count] != '\0') {

            if (s[count] == ' ' && s_count == 0) { //first space
                strcat(m, ": ");
                s_count++;
                //g_print("current string: %s\ncount:%d\n", m, count);
            }
            else if (s[count] == ' ' && s_count == 1) { //second space
                strcat(m, " - ");
                s_count++;
                //g_print("current string: %s\ncount:%d\n", m,count); 
            }
            else {
                char buf[2];
                sprintf(buf, "%c", s[count]);
                strcat(m, buf);
            }
            count++;
            /* 
               else {
               char buf[1]; 
               sprintf(buf, "%c", s[count]);
               strcat(m, buf);
               g_print("current string: %s\ncount%d\n", m,count);
               }
               count++;
               */
        }
        strcat(m, "\n");
        //g_print("string: %s\n", m);
        insert_text(buffer, m);
        s = strtok(NULL, "\r\n");
    }

    sprintf(messageCount, "%d", lastMessage);

    memset(messageResponse, 0, sizeof messageResponse);
    //char text[10];
    //sprintf(text, "%d", i);
    //g_print("text: %s\n", text);

    //insert_text(buffer, text);
    //i++;
}


void get_room_selection(GtkWidget * widget, gpointer p){
    GtkTreeSelection * selection;
    GtkTreeIter iter;
    GtkTreeModel * model;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view_room));
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *name;
        gtk_tree_model_get(model, &iter, 0, &name, -1);

        //g_print("Selected row is: %s\n", name);

        //leave old room
        leave_room();

        room = strdup(name); //change room to new room

        //enter room
        enter_room();   

        g_free(name);
    }
}

/* Create the list of "messages" */
static GtkWidget *create_user_list( const char * titleColumn, GtkListStore *model )
{
    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
    //GtkListStore *model;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
            GTK_POLICY_AUTOMATIC, 
            GTK_POLICY_AUTOMATIC);

    //model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
    gtk_widget_show (tree_view);

    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes (titleColumn,
            cell,
            "text", 0,
            NULL);

    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
            GTK_TREE_VIEW_COLUMN (column));

    return scrolled_window;
}

/* Create the list of "messages" */
static GtkWidget *create_room_list( const char * titleColumn, GtkListStore *model )
{
    GtkWidget *scrolled_window;
    //GtkWidget *tree_view;
    //GtkListStore *model;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
            GTK_POLICY_AUTOMATIC, 
            GTK_POLICY_AUTOMATIC);

    //model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view_room = gtk_tree_view_new ();

    g_signal_connect(tree_view_room, "row-activated", G_CALLBACK(get_room_selection), NULL);

    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view_room);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view_room), GTK_TREE_MODEL (model));
    gtk_widget_show (tree_view_room);

    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes (titleColumn,
            cell,
            "text", 0,
            NULL);

    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view_room),
            GTK_TREE_VIEW_COLUMN (column));

    return scrolled_window;
}

/* Add some text to our text widget - this is a callback that is invoked
   when our window is realized. We could also force our window to be
   realized with gtk_widget_realize, but it would have to be part of
   a hierarchy first */

static void insert_text( GtkTextBuffer *buffer, const char * initialText )
{
    GtkTextIter iter;

    gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    gtk_text_buffer_insert (buffer, &iter, initialText,-1);
}

static void clear_text() {
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    gtk_text_buffer_delete(buffer, &start, &end);
}

/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text( const char * initialText, int b )
{
    GtkWidget *scrolled_window;
    //GtkWidget *view;
    GtkTextBuffer *buffer;

    view = gtk_text_view_new ();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), b);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (scrolled_window), view);
    insert_text (buffer, initialText);

    gtk_widget_show_all (scrolled_window);

    return scrolled_window;
}

/* Create a scrolled text area that displays YOUR MESSAGE */
static GtkWidget *create_user_text( const char * initialText, int b )
{
    GtkWidget *scrolled_window;
    //GtkWidget *view_user;
    GtkTextBuffer *buffer;

    view_user = gtk_text_view_new ();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view_user), b);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view_user));

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (scrolled_window), view_user);
    insert_text (buffer, initialText);

    gtk_widget_show_all (scrolled_window);

    return scrolled_window;
}
void createRoom(GtkWidget *widget, gpointer p) {
    GtkWidget *window3;
    GtkWidget *entry3;

    window3 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window3), "Create Room");
    g_signal_connect(window3, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window3), 10);
    gtk_widget_set_size_request(GTK_WIDGET(window3), 300, 75);

    GtkWidget *table3 = gtk_table_new (4, 2, TRUE);
    gtk_container_add(GTK_CONTAINER(window3), table3);
    gtk_table_set_row_spacings(GTK_TABLE(table3), 2);
    gtk_table_set_col_spacings(GTK_TABLE(table3), 4);
    gtk_widget_show(table3);

    entry3 = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry3), 50);
    gtk_table_attach_defaults(GTK_TABLE(table3), entry3, 0, 2, 0, 1);
    gtk_widget_show(entry3);

    loginInfo * li = g_malloc(sizeof(*li));
    li->window = window3;
    li->entry1 = entry3;

    GtkWidget *submit_room = gtk_button_new_with_label("Create Room");
    gtk_signal_connect(GTK_OBJECT(submit_room), "clicked", G_CALLBACK(set_room), (gpointer) li);

    gtk_table_attach_defaults(GTK_TABLE (table3), submit_room, 0, 2, 2, 4);
    gtk_widget_show (submit_room);


    gtk_widget_show(window3);
}

void get_message(GtkWidget* widget, gpointer p) {
    GtkTextBuffer *buffer;
    GtkTextIter start;
    GtkTextIter end;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view_user));
   
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    gchar * message = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    gtk_text_buffer_delete(buffer, &start, &end);

    send_message(message);
}

void create_account(GtkWidget* widget, gpointer p) {
    gchar * title = (gchar*) p;
    GtkWidget *window2;
    GtkWidget *entry1, *entry2;

    window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window2), title);
    g_signal_connect(window2, "destroy", G_CALLBACK (gtk_widget_destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window2), 10);
    gtk_widget_set_size_request (GTK_WIDGET(window2), 300, 100);

    GtkWidget *table2 = gtk_table_new (5, 3, TRUE);
    gtk_container_add(GTK_CONTAINER(window2), table2);
    gtk_table_set_row_spacings(GTK_TABLE(table2), 5);
    gtk_table_set_col_spacings(GTK_TABLE(table2), 5);
    gtk_widget_show(table2);

    entry1 = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry1), 50);
    gtk_entry_set_text(GTK_ENTRY(entry1), "username");
    //tmp_pos = GTK_ENTRY(entry1)->text_length;
    gtk_entry_set_editable(GTK_ENTRY(entry1), TRUE);
    gtk_table_attach_defaults(GTK_TABLE(table2), entry1, 0, 3, 0, 1);
    gtk_widget_show(entry1);

    entry2 = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry2), 50);
    gtk_entry_set_text(GTK_ENTRY(entry2), "");
    //tmp_pos = GTK_ENTRY(entry2)->text_length;
    gtk_entry_set_visibility(GTK_ENTRY(entry2), FALSE);
    gtk_entry_set_editable(GTK_ENTRY(entry2), TRUE);
    gtk_table_attach_defaults(GTK_TABLE(table2), entry2, 0, 3, 2, 3);
    gtk_widget_show(entry2);

    loginInfo* li = g_malloc(sizeof(*li));
    li->window = window2;
    li->entry1 = entry1;
    li->entry2 = entry2;

    GtkWidget *submit_button = gtk_button_new_with_label(title);

    gtk_signal_connect (GTK_OBJECT(submit_button), "clicked", G_CALLBACK(set_user), (gpointer) li);
    gtk_signal_connect (GTK_OBJECT(submit_button), "clicked", G_CALLBACK (gtk_widget_destroy), NULL);


    //gtk_signal_connect (GTK_OBJECT(submit_button), "clicked", G_CALLBACK(set_pass()), entry2);

    gtk_table_attach_defaults(GTK_TABLE (table2), submit_button, 0, 3, 4, 6);
    gtk_widget_show (submit_button);


    gtk_widget_show(window2);
}
void set_room(GtkWidget *widget, gpointer p) {
    loginInfo * li = (loginInfo*) p;
    const gchar *entry_text;

    entry_text = gtk_entry_get_text(GTK_ENTRY(li->entry1));
    //g_print("Room name is %s\n", entry_text);

    room = strdup(entry_text);
    create_room();
    enter_room();

    gtk_widget_destroy(li->window);
}
void set_user(GtkWidget *widget, gpointer p) {
    loginInfo* li = (loginInfo*) p;
    const gchar *entry_text1;
    const gchar *entry_text2;
    entry_text1 = gtk_entry_get_text(GTK_ENTRY(li->entry1));
    entry_text2 = gtk_entry_get_text(GTK_ENTRY(li->entry2));

    //user = (gchar*) entry_text1;
    //password = (gchar*) entry_text2;

    user = strdup(entry_text1);
    password = strdup(entry_text2);

    //g_print ("Contents of entries:\n%s\n%s\n", entry_text1, entry_text2); 
    add_user(); 
    gtk_widget_destroy(li->window);
}

//get list of rooms
static gboolean timer(GtkWidget *widget) {
    if (widget->window == NULL) return FALSE;

    char response[MAX_RESPONSE];
    char response2[MAX_RESPONSE];

    sendCommand(host, port, "LIST-ROOMS", user, password, "", response); 
    //g_print("room response:\n%s\n",response);
    update_list_rooms(response);


    sendCommand(host, port, "GET-USERS-IN-ROOM", user, password, room, response2);
    //g_print("user response:\n%s\n", response2);
    update_list_users(response2);

    update_messages();

    memset(response, 0, sizeof response);
    memset(response2, 0, sizeof response2);

    return TRUE;
}

int main( int   argc,
        char *argv[] )
{

    //do check
    if (argc < 3) {
        printUsage(); 
    }

    host = argv[1];
    sport = argv[2];

    printf("\nStarting IRC client %s %s\n", host, sport);

    //convert port to number
    sscanf(sport, "%d", &port);

    //startGetMessagesThread();

    GtkWidget *window;
    GtkWidget *list;
    GtkWidget *u_list;
    //GtkWidget *messages;
    //GtkWidget *myMessage;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "IRC Client");
    g_signal_connect (window, "destroy",
            G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (GTK_WIDGET (window), 450, 400);

    // Create a table to place the widgets. Use a 10x6 Grid
    GtkWidget *table = gtk_table_new (10, 6, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

    // Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
    list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    update_list_rooms("");
    list = create_room_list ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), list, 3, 6, 0, 3);
    gtk_widget_show (list);

    // Add list of users.
    list_users = gtk_list_store_new (1, G_TYPE_STRING);
    update_list_users("");
    u_list = create_user_list("Users", list_users);
    gtk_table_attach_defaults (GTK_TABLE (table), u_list, 0, 3, 0, 3);
    gtk_widget_show (u_list);

    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    messages = create_text ("", 0);

    //gtk_entry_set_editable(GTK_ENTRY(messages), FALSE);

    gtk_table_attach_defaults (GTK_TABLE (table), messages, 0, 6, 4, 7);
    gtk_widget_show (messages);

    // Add to-send text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    myMessage = create_user_text ("", 1);
    gtk_table_attach_defaults (GTK_TABLE (table), myMessage, 0, 5, 7, 8);
    gtk_widget_show (myMessage);

    // Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
    GtkWidget *send_button = gtk_button_new_with_label ("Send");
    gtk_signal_connect(GTK_OBJECT(send_button), "clicked", GTK_SIGNAL_FUNC(get_message), NULL); 

    gtk_table_attach_defaults(GTK_TABLE (table), send_button, 5, 6, 7, 8); 
    gtk_widget_show (send_button);

    //login button
    GtkWidget *create_login_button = gtk_button_new_with_label("Login"); 
    gtk_signal_connect (GTK_OBJECT(create_login_button), "clicked", GTK_SIGNAL_FUNC(create_account), (gpointer) "Login");

    gtk_table_attach_defaults(GTK_TABLE (table), create_login_button, 0, 2, 9, 10);
    gtk_widget_show (create_login_button);

    // Add create account button
    GtkWidget *create_user_button = gtk_button_new_with_label("Create Account");
    gtk_signal_connect (GTK_OBJECT(create_user_button), "clicked", GTK_SIGNAL_FUNC(create_account), (gpointer) "Create Account");

    gtk_table_attach_defaults(GTK_TABLE (table), create_user_button, 2, 4, 9, 10);
    gtk_widget_show (create_user_button);

    // Add create room button
    GtkWidget *create_room_button = gtk_button_new_with_label("Create Room");
    gtk_signal_connect (GTK_OBJECT(create_room_button), "clicked", GTK_SIGNAL_FUNC(createRoom), NULL);
    gtk_table_attach_defaults(GTK_TABLE(table), create_room_button, 4, 6, 9, 10);
    gtk_widget_show (create_room_button);

    //timer functions
    g_timeout_add(5000, (GSourceFunc) timer, (gpointer) window);

    gtk_widget_show (table);
    gtk_widget_show (window);

    gtk_main ();
    //gui created, now connect pipes

    return 0;
}

