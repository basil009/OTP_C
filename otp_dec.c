//otp_dec.c

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

# define MAX 69340
# define SIZE 50
  
int main(int argc, char *argv[]) {
   int sockfd, portno, n; //Socket variables //n is used to keey track of bytes from read files
   struct sockaddr_in serv_addr;
   char ciphertext[MAX];
   char buffer[MAX];
   char key[MAX];
   FILE * fp;
   char client_check[SIZE] = "otp_dec"; //used to authenticate the program
   
   if (argc < 4) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }

   portno = atoi(argv[3]); //converting string to int type

   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //home address
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }

    /***** Reading, Checking and Sending Files ******/
    if (send(sockfd, client_check, strlen(client_check) , 0) < 0) { //Sending the name of the program 'otp_dec'
        printf("Send failed");                                      //to check if connection was allowed
        exit(1);                                                    //since otp_dec cannot connect to otp_enc_d.
    }

    //Reading ciphertext from the given file in the argument
    fp = fopen(argv[1], "r"); 
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    bzero(ciphertext, MAX);
    n = fread(ciphertext, MAX, 1, fp);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    //Reading KEY from the file given in the the argument
    fp = fopen(argv[2], "r"); 
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    bzero(key, MAX);
    n = fread(key, MAX, 1, fp);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    if (strlen(key) < strlen(ciphertext)) { //Checking if the key is larger or equal in length to the cipher.
        printf("Error: Key is shorter than ciphertext\n"); //If so, program prints error message and terminates
        exit(1);
    }
 
    //Sending ciphertext and Key to otp_dec_d to be decrypted
    if (send(sockfd, ciphertext, strlen(ciphertext), 0) < 0) {
        printf("Send failed");
        exit(1);
    }
    sleep(1); //sleeping one second, so otp_dec_d can distinguish and seperate key from ciphertext, and allow time to fork

   if (send(sockfd, key, strlen(key), 0) < 0) {
        printf("Send failed");
        exit(1);
    }

    //Recieving decrypted plaintext from server
    bzero(buffer, MAX);
    n = read(sockfd, buffer, MAX);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    else if (n == 0) //nothing was recieved
        return 1;

    printf("%s\n",buffer);

    return 0;
}
  
