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

void *get_in_addr2(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char peers[][20] = {"localhost"};
unsigned int num_peers = 1; // Match above

void *client_thread(){
  for(unsigned int i = 0; i < num_peers; i++){
    // FOrk this process and connect!
    if (!fork()) { // this is the child process
      
      int sockfd, numbytes;  
      char buf[MAXDATASIZE];
      struct addrinfo hints, *servinfo, *p;
      int rv;
      char s[INET6_ADDRSTRLEN];

      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;

      if ((rv = getaddrinfo(peers[0], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
      }

      // loop through all the results and connect to the first we can
      for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
          perror("client: socket");
          continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          perror("client: connect");
          close(sockfd);
          continue;
        }

        break;
      }

      if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return NULL;
      }

      inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr),
          s, sizeof s);
      printf("client: connecting to %s\n", s);

      freeaddrinfo(servinfo); // all done with this structure

      if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
          perror("recv");
          exit(1);
      }

      buf[numbytes] = '\0';

      printf("client: received '%s'\n",buf);

      close(sockfd);



			exit(0);
		}
  }
  
  // Now pop off the queues whenever something is added!
  while(1){
    // Add to Block and TX Queue
  }

	return 0;
}