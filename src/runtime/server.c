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

int build_listen_socket() {
  int sockfd, yes, rv;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and bind to the first we can
  yes = 1;
	for (p = servinfo; p != NULL; p = p->ai_next) {
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

  return sockfd;
}

void build_sig_handler() {
	struct sigaction sa;
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
}

// From Beej's Guide
int send_all(int s, char *buf, size_t *len) {
  size_t total = 0;        // how many bytes we've sent
  size_t bytes_left = *len; // how many we have left to send
  int n;

  while (total < *len) {
    n = send(s, buf + total, bytes_left, 0);
    if (n == -1) { break; }
    total += n;
    bytes_left -= n;
  }

  *len = total; // return number actually sent here

  return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

void server_fork(Globals *globals, int sockfd, pid_t pid) {
  mqd_t child_mq;
  struct mq_attr attr;
  char in_buffer[MAX_MSG_SIZE];
  unsigned long counter;
  size_t buf_size, total_size;

  printf(
    "Opening Queue from server child: %s\n",
    globals->q_server_individual[globals->connected]
  );
  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
  child_mq = mq_open(
    globals->q_server_individual[globals->connected],
    O_RDONLY | O_CREAT,
    QUEUE_PERMISSIONS,
    &attr
  );
  if (child_mq == -1) {
    perror("Server: mq_open (server)");
    exit(1);
  }

  counter = 1;
  while (pid != 1){
    printf("SERVER FORK LOOP\n");
    // Check to see if parent killed
    if(counter % 10000){
      pid = getpid();
    }

    if (mq_receive(child_mq, in_buffer, MAX_MSG_SIZE, NULL) == -1) {
      perror("Client: mq_receive");
      exit(1);
    }

    buf_size = *(size_t*)(in_buffer + sizeof(int));
    total_size = buf_size + sizeof(int) + sizeof(size_t);

    if (send_all(sockfd, in_buffer, &total_size) == -1) {
      perror("Server: send_all");
      exit(1);
    }
  }

  if (mq_close(child_mq) == -1) {
    perror("Server: mq_close");
    exit(1);
  }
  close(sockfd);
  exit(0);
}

void handle_accept(Globals *globals, int sockfd) {
  int new_fd;
  size_t name_size;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];

  sin_size = sizeof(their_addr);
  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
  if (new_fd == -1) {
    perror("accept");
    return;
  }

  inet_ntop(
    their_addr.ss_family,
    get_in_addr((struct sockaddr *)&their_addr),
    s, sizeof s
  );
  printf("server: got connection from '%s'\n", s);

  name_size = snprintf(NULL, 0, OUTGOING_QUEUE_FORMAT, globals->connected);
  globals->q_server_individual[globals->connected] = malloc(name_size + 1);
  sprintf(
    globals->q_server_individual[globals->connected],
    OUTGOING_QUEUE_FORMAT,
    getpid()
  );
  printf(
    "Made new queue name '%s'\n",
    globals->q_server_individual[globals->connected]
  );

  pid_t pid = fork();
  if (!pid) { // this is the child process
    close(sockfd); // child doesn't need the listener
    server_fork(globals, new_fd, pid);
  }

  close(new_fd);  // parent doesn't need this
  globals->connected++;
}

void *server_thread(void *arg){
  Globals *globals = arg;
  int sockfd;

  sockfd = build_listen_socket();
  build_sig_handler();

	printf("server: waiting for connections...\n");
	while (1) {
    handle_accept(globals, sockfd);
  }

	return NULL;
}
