#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#define BUFF_SIZE 300
char prev_message[BUFF_SIZE];
/***********************************CLIENT*********************/
void error(char *msg){
	perror(msg);
	exit(0);
}


void * receiveMessage(void *socket) {
 int sockfd, ret;
 char buffer[BUFF_SIZE]; 
 sockfd = (intptr_t) socket;
 memset(buffer, 0, BUFF_SIZE);  
 for (;;) {
  ret = recvfrom(sockfd, buffer, BUFF_SIZE, 0, NULL, NULL);

  if (ret < 0) {  
   printf("Error receiving data!\n");    
  }else if(strcmp(prev_message,buffer)=='0'){
  	close(sockfd);
	exit(0);}	
	 else {
	printf("Server : ");;	
   fputs(buffer,stdout);
   strncpy(prev_message,buffer,BUFF_SIZE);
   //printf("\n");
  }  
	
 }
}


int main(int argc, char* argv[]){
	int sockfd, portno,n,ret;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[300];
        pthread_t rThread;

	
	if (argc<2){
		fprintf(stderr,"usage %s hostname port\n",argv[0]);
		exit(0);
	}
	else{
            printf("<============Client1 active==========>\n");
        }
	portno=atoi(argv[2]);/*getting port number*/
	
	//creating a socket
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		error("Socket not created");
	}
	
	server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	/*
	serv_addr.sin_family=AF_INET;//defining protocol to be used
	//inet_addr() ::=:: standard string representing host address expressed in standard dotted format. & then returns a host address suitable for internet address.
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	//internet address tonetwork address
	bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	serv_addr.sin_port=htons(portno);
	*/
	
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
  
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
 /*   
	while(1){
	printf("Client_12S: ");
        bzero(buffer,299);
        fgets(buffer,299,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        printf("Server : %s\n",buffer);	
	if(buffer=="stop"){
                close(sockfd);
		error("ThanksC");
	}
        }
*/
 memset(buffer, 0, BUFF_SIZE);
 printf("Enter your messages one by one and press return key!\n");

 //creating a new thread for receiving messages from the server
 ret = pthread_create(&rThread, NULL, receiveMessage, (void *)(intptr_t) sockfd);
 if (ret) {
  printf("ERROR: Return Code from pthread_create() is %d\n", ret);
  exit(1);
 }


 while (fgets(buffer, BUFF_SIZE, stdin) != NULL) {
	ret = sendto(sockfd, buffer, BUFF_SIZE, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));  
  		if (ret < 0) {  
   			printf("Error sending data!%s", buffer);
   			close(sockfd);
 		    pthread_exit(NULL);
 		    exit(0);  
	}
}
	 //buffer= "Client1:"+buffer;
  
 return 0;    
}  