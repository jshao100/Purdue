
void printUsage();
void add_user();
void create_room();
void leave_room();
void enter_room();
void send_message(char *msg);
int open_client_socket(char * host, int port);
int sendCommand(char * host, int port, char * command, char * user, char * password,
                char * args, char * response);
void * getMessagesThread(void * arg);
void startGetMessagesThread();


static gboolean timer(GtkWidget * widget);
void update_list_rooms(char * msg);
void update_list_users(char * msg);
void update_messages();
static void insert_text(GtkTextBuffer *buffer, const char * initialText);
static void clear_text();
static GtkWidget *create_text(const char * initialText, int b); 
void createRoom(GtkWidget * widget, gpointer p);
void create_account(GtkWidget * widget, gpointer p);
void set_room(GtkWidget *widget, gpointer p);
void set_user(GtkWidget *widget, gpointer p);
static GtkWidget *create_user_list(const char * titleColumn, GtkListStore *model);
static GtkWidget *create_room_list(const char * titleColumn, GtkListStore *model);
void get_room_selection(GtkWidget * widget, gpointer p);


