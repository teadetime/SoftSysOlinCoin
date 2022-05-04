#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include "client.h"
#include "server.h"
#include "runtime.h"
#include "ser_block.h"
#include "ser_tx.h"

char peers[][20] = {"192.168.32.251"};//"ubuntu.local"};//"localhost",
unsigned int num_peers = 1; // Match above

void *get_in_addr2(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int recv_all(int s, char *buf, size_t *len) {
  size_t total = 0;        // how many bytes we've read
  size_t bytes_left = *len; // how many we have left to read
  int n;

  while (total < *len) {
    n = recv(s, buf + total, bytes_left, 0);
    if (n == 0)
      return 1;
    else if (n == -1)
      return -1;
    total += n;
    bytes_left -= n;
  }

  *len = total; // return number actually sent here

  return 0;
}

int recv_obj(int s, char *buf, size_t *total_size) {
  int rv;
  size_t head_size, buf_size;

  head_size = sizeof(int) + sizeof(long);
  if ((rv = recv_all(s, buf, &head_size)) != 0)
    return rv;

  buf_size = *(size_t*)(buf + sizeof(int));
  if ((rv = recv_all(s, buf + head_size, &buf_size)) != 0)
    return rv;

  *total_size = buf_size + head_size;
  return 0;
}

void client_fork(Globals *globals, pid_t pid, int i) {
  int sockfd, rv, priority;
  size_t numbytes;
  unsigned long counter;
  char buf[MAX_MSG_SIZE];
  struct sockaddr_in serv_addr;
  struct mq_attr attr;
  mqd_t incoming;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("\n Socket creation error \n");
      exit(0);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT_INT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, peers[i], &serv_addr.sin_addr) <= 0) {
      printf("\nInvalid address/ Address not supported \n");
      exit(0);
  }
  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      printf("\nConnection Failed \n");
      exit(0);
  }

  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
  incoming = mq_open(
    globals->q_client,
    O_WRONLY | O_CREAT,
    QUEUE_PERMISSIONS,
    &attr
  );
  if (incoming == -1) {
    perror ("Client: mq_open (server)");
    exit (1);
  }
  printf("Opened Incoming Queue for writing\n");

  counter = 0;
  while (pid != 1) {
    // Check to see if parent killed
    if (counter % 10000) {
      pid = getpid();
    }

    // Get data over the socket
    rv = recv_obj(sockfd, buf, &numbytes);
    if (rv == -1) {
      perror("recv obj");
      exit(1);
    } else if (rv == 1) {
      // Connection has been closed from the other side (by the socket server)
      break;
    }

    // Add the data to the queue
    priority = *(int*)buf;
    if (mq_send(incoming, buf, numbytes, priority) == -1) {
        perror ("Client: Not able to send message to main process");
        continue;
    }
    printf("client: received %lu bytes\n", numbytes);

    counter++;
  }

  close(sockfd);
  exit(0);
}

void *client_thread(void *arg){
  Globals *globals = arg;
  for (unsigned int i = 0; i < num_peers; i++) {
    // Fork this process and connect!
    pid_t pid = fork();
    if (!pid) { // this is the child process
      client_fork(globals, pid, i);
		}
  }

  //INCOMING PARENT
  struct mq_attr attr;
  mqd_t incoming_parent;
  char in_buffer[MSG_BUFFER_SIZE], *ser_buffer;
  unsigned int priority;
  int id;
  size_t buf_size;

  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
  incoming_parent = mq_open(
    globals->q_client,
    O_RDONLY | O_CREAT,
    QUEUE_PERMISSIONS,
    &attr
  );
  if (incoming_parent == -1) {
    perror ("Client: mq_open (server)");
    exit (1);
  }

  // Now pop off the queues whenever something is added!
  while (1) {
    if (mq_receive(incoming_parent, in_buffer, MAX_MSG_SIZE, &priority) == -1) {
      perror ("Client: mq_receive");
      exit (1);
    }

    id = *(int*)in_buffer;
    buf_size = *(size_t*)(in_buffer + sizeof(int));
    ser_buffer = in_buffer + sizeof(int) + sizeof(size_t);

    printf(
      "Client parent recieved %lu byte object of id '%d'\n",
      buf_size, id
    );
    if (id == BLOCK_ID) {
      Block *new_block = deser_block_alloc(NULL, (unsigned char *)ser_buffer);
      queue_add_void(globals->queue_block, new_block);
    }
    else if (id == TX_ID) {
      Transaction *new_tx = deser_tx_alloc(NULL, (unsigned char *)ser_buffer);
      queue_add_void(globals->queue_tx, new_tx);
    }
  }

  return 0;
}
