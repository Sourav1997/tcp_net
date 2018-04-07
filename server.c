#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<pthread.h>
#define BUFF_SIZE 300
char prev_message[1000];

void error(char *msg){
	perror(msg);
	exit(1);
}


void * read_data(void *socket) {
 int sockfd, ret;
 char buffer[300]; 
 sockfd = (intptr_t) socket;
 memset(buffer, 0, BUFF_SIZE);  
 for (;;) {
  ret = recvfrom(sockfd, buffer, BUFF_SIZE, 0, NULL, NULL);  
  if (ret < 0) {  
   printf("Error receiving data!\n");    
  }else if(strcmp(prev_message,buffer)=='0'){
  	close(sockfd);
 	//pthread_exit(NULL);
	exit(0);}	
	 else {
	printf("Client : ");;	
   fputs(buffer,stdout);
  strncpy(prev_message,buffer,BUFF_SIZE);
   //printf("\n");
  }  
	
 }
}

int main(int argc,char *argv[]){
	int socketfd,newsocketfd , portno,client_data,n;
	char buffer[300];
	struct sockaddr_in server,client;
	
	if (argc<2){
		printf("Error , no port\n");
		exit(1);
	}
	else{
        printf("<==========Server Active:=====>\n");
        }
	/*Sockte creation*/
	if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0){
		error("Error openning Socket");
	}	
	
	/*sets the buffer to zero : (buffer pointer)* ,size of buffer*/
	bzero((char *)&server,sizeof(server));
	
	portno=atoi(argv[1]);
	server.sin_family=AF_INET;
	server.sin_port=htons(portno); //gets the port number 
	server.sin_addr.s_addr=INADDR_ANY; //get the ip address of the machine
	
	/*binding ; type casting sockaddr_in to sockaddr (socket,address for binding,the size of the address*/
	if (bind(socketfd,(struct sockaddr*)&server,sizeof(server))<0){
		error("No binding");
	}
	/*listenning ; (the socket , the max.number of connections that can be waiting )*/
	listen(socketfd,5);
        client_data=sizeof(client);
        newsocketfd=accept(socketfd,(struct sockaddr *)&client,&client_data);
	
	if (newsocketfd <0){
		error("Accept error");
	}

	pthread_t read_thread;
	int ret;
	/*
	ret=pthread_create(&read_thread,NULL,read_data,(void *)(intptr_t)newsocketfd);
	if(ret<0){error("....Cannot read thread....");}

	while(fgets(buffer,300,stdin)!=NULL){
	ret=sendto(newsocketfd,buffer,300,0,(struct sockaddr *)&client,client_data);
	if (ret<0){
		printf("Error Sending data");
		exit(1);
	}
	}
*/
memset(buffer, 0, BUFF_SIZE);
// printf("Enter your messages one by one and press return key!\n");

 //creating a new thread for receiving messages from the server
 ret = pthread_create(&read_thread, NULL, read_data, (void *)(intptr_t) newsocketfd);
 if (ret<0) {
  printf("ERROR: Return Code from pthread_create() is %d\n", ret);
  exit(1);
 }


 while (fgets(buffer, BUFF_SIZE, stdin) != NULL) {
		ret = sendto(newsocketfd, buffer, BUFF_SIZE, 0, (struct sockaddr *) &client, client_data);  
  		if (ret < 0) {  
   				printf("Error sending data!\n\t-%s", buffer);
   				close(newsocketfd);
 				pthread_exit(NULL);
 				exit(0);  
			}
	}

return 0;
}
