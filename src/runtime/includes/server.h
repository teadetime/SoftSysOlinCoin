#include <sys/socket.h>


#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);

void *server_thread();