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

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

void *get_in_addr2(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char peers[][20] = {"192.168.32.251"};//"ubuntu.local"};//"localhost", 
unsigned int num_peers = 1; // Match above

void *client_thread(void *arg){
  Globals *globals = arg;
  for(unsigned int i = 0; i < num_peers; i++){
    // FOrk this process and connect!
    if (!fork()) { // this is the child process
      int sockfd, numbytes;  
      char buf[MAXDATASIZE];
      struct sockaddr_in serv_addr;
      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
          printf("\n Socket creation error \n");
          continue;
      }
  
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(PORT_INT);
  
      // Convert IPv4 and IPv6 addresses from text to binary
      // form
      if (inet_pton(AF_INET, peers[i], &serv_addr.sin_addr)
          <= 0) {
          printf(
              "\nInvalid address/ Address not supported \n");
          continue;
      }
  
      if (connect(sockfd, (struct sockaddr*)&serv_addr,
                  sizeof(serv_addr))
          < 0) {
          printf("\nConnection Failed \n");
          continue;
      }

     
      // // using Hostname
      // struct addrinfo hints, *servinfo, *p;
      // int rv;
      // char s[INET6_ADDRSTRLEN];

      // memset(&hints, 0, sizeof hints);
      // hints.ai_family = AF_UNSPEC;
      // hints.ai_socktype = SOCK_STREAM;

      // if ((rv = getaddrinfo(peers[i], PORT, &hints, &servinfo)) != 0) {
      //   fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      //   return NULL;
      // }

      // // loop through all the results and connect to the first we can
      // for(p = servinfo; p != NULL; p = p->ai_next) {
      //   if ((sockfd = socket(p->ai_family, p->ai_socktype,
      //       p->ai_protocol)) == -1) {
      //     perror("client: socket");
      //     continue;
      //   }

      //   if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      //     perror("client: connect");
      //     close(sockfd);
      //     continue;
      //   }

      //   break;
      // }

      // if (p == NULL) {
      //   fprintf(stderr, "client: failed to connect\n");
      //   return 2;
      // }

      // inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr),
      //     s, sizeof s);
      // printf("client: connecting to %s\n", s);

      // freeaddrinfo(servinfo); // all done with this structure

      
      struct mq_attr attr;

      attr.mq_flags = 0;
      attr.mq_maxmsg = MAX_MESSAGES;
      attr.mq_msgsize = MAX_MSG_SIZE;
      attr.mq_curmsgs = 0;
      mqd_t incoming;
      // if ((incoming = mq_open (globals->q_client, O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
      //   perror ("Server: mq_open (server)");
      //   exit (1);
      // }
      printf("Opened Incoming Queue for writing, now waiting for incoming data to socket");
      while(1){
        // Get data over the socket
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
          perror("recv");
          exit(1);
        }

        // Add the data to the queue
        //char *test = "recieved fake_block";
        // if (mq_send (incoming, buf, numbytes, 0) == -1) {
        //     perror ("Client: Not able to send message to server");
        // }
        buf[numbytes] = '\0';

        printf("client: received %i bytes '%s'\n",numbytes, buf);
        sleep(1);

      }

      close(sockfd);
			exit(0);
		}
  }

  //INCOMING PARENT
  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
  mqd_t incoming_parent;
  // if ((incoming_parent = mq_open (globals->q_client, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
  //   perror ("Server: mq_open (server)");
  //   exit (1);
  // }
  // Now pop off the queues whenever something is added!
  while(1){
    // Add to Block and TX Queue
   
    // char in_buffer [MSG_BUFFER_SIZE];
    // unsigned int priority;
    // if (mq_receive (incoming_parent, in_buffer, MSG_BUFFER_SIZE, &priority) == -1) {
    //   perror ("Server: mq_receive");
    //   exit (1);
    // }
    // // Block
    // if(priority == 1){
    //   // Dummy text data
    //   printf("Incoming data to client parent: %s", in_buffer);
    //   continue;
    //   Block *new_block = deser_block_alloc(NULL, (unsigned char *)in_buffer);
    //   queue_add_void(globals->queue_block, new_block);
    // }
    // //TX
    // else if (priority == 2)
    // {
    //   Transaction *new_tx = deser_tx_alloc(NULL, (unsigned char *)in_buffer);
    //   queue_add_void(globals->queue_tx, new_tx);
    // }
    // //Garbage
    // else{

    // }
  }

	return 0;
}