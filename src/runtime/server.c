#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "server.h"
#include "mqueue.h"
#include "runtime.h"

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void *server_thread(void *arg){
  Globals *globals = arg;
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

    char *q_name = "/outgoing_";
    char con_num = '0' + globals->connected;
    globals->q_server_individual[globals->connected] = calloc(sizeof(char), strlen(q_name)+2); //digits etc
    strncpy(globals->q_server_individual[globals->connected], q_name, strlen(q_name)+1);
    strncat(globals->q_server_individual[globals->connected], &con_num, 1);
    globals->connected++; //TODO MAKE THIS LESS than 10  peers

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
      mqd_t child_mq;
      struct mq_attr attr;

      attr.mq_flags = 0;
      attr.mq_maxmsg = MAX_MESSAGES;
      attr.mq_msgsize = MAX_MSG_SIZE;
      attr.mq_curmsgs = 0;
      char in_buffer [MSG_BUFFER_SIZE];
      printf("Opening Queue from server child: %s", globals->q_server_individual[globals->connected]);
      // if ((child_mq = mq_open (globals->q_server_individual[globals->connected], O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
      //     perror ("Server: mq_open (server)");
      //     exit (1);
      // }
      while (1) {
        sleep(5);
        // if (mq_receive(child_mq, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
        //   perror ("Client: mq_receive");
        //   exit (1);
        // }
        char *test = "dummy_data_from_server 1";
        // Send the queue data over a socket
        if (send(new_fd, test, strlen(test)+1, 0) == -1)
          perror("send");
      }
      // if (mq_close(child_mq) == -1) {
      //   perror ("Client: mq_close");
      //   exit (1);
      // }
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return NULL;
}

