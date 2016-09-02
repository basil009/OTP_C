//otp_enc_d.c
# include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

# define MAX 69340 
# define SIZE 50

//One of the main operations of this program is to convert letter strings, to it's equavelent character numbers. 
//For example, G(23), T(22), I(6), and so on.
//Whe converting from "letter string" to "number string", the functions convertes from "THE RED GOOSE...." to "275427192739"

//Prototype
int * Reference(char * a_string); //Used to convert letter strings to numbers
char * Dereference(int * number_string, int len_of_array); //Used to convert number strings to letter strings
int mod (int a, int b); //Does the modulu operation


int main(int argc, char * argv[]) {
   int sockfd, newsockfd, portno, clilen; //socket variables
   char buffer[MAX]; 
   struct sockaddr_in serv_addr, cli_addr; 
   int n; //used to determine the number of bytes read and recieved
   int file_counter = 0; //counts file read from the socket
   int * number_plaintext; //to store the converted letter string to number string
   int * number_ciphertext;
   int * number_key;
   char * ciphertext;
   char * plaintext;
   char client_check[SIZE]; //used to store the program name that is trying to connect
   int plaintext_len; //length of the plaintext
   int i = 0;
   int pid;

   if (argc < 2) {
       printf("very few argument\n");
       exit(1);
   }

   portno = atoi(argv[1]); //converting string to int, given the port number in the argument

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will */
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1) {
       /* Accept actual connection from the client */
       newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
       if (newsockfd < 0) {
          perror("ERROR on accept");
          exit(1);
       }

       //As soon as we accept, we fork to allow for incoming requests of connections
       //and we do the operations on a seperate process (the child process)
       pid = fork();
       if (pid < 0) {
           perror("Error: unable to fork");
           exit(1);
       }

       if (pid == 0) { //child
           close(sockfd); //we close sockfd, becasue it will be used by the parent to accept other connection
                            //The child connection is working on the socket 'newsockfd'.

            //Checking if the connection is from otp_enc
            bzero(client_check, SIZE);
            n = read(newsockfd, client_check, SIZE);
            if (n < 0) {
                perror("ERROR reading from socket");
                exit(1);
            }
            if (! strcmp(client_check, "otp_dec")) { //otp_dec or otp_enc will send the name of the program ("otp_dec" or "otp_enc")
                printf("Error: Connections from 'otp_dec' are not allowed\n");
                exit(1);
            }

           /* If connection is established then start communicating */
            while (file_counter < 2) {
                bzero(buffer, MAX);
                n = read(newsockfd, buffer, MAX);
                if (n < 0) {
                    perror("ERROR reading from socket");
                    exit(1);
                }
                buffer[n - 1] = '\0'; //-1 to strip off the \n

                if (file_counter == 0) { //We recieved plaintext first
                    plaintext_len = strlen(buffer);
                    plaintext = (char *) malloc(sizeof(char) * (plaintext_len+1));
                    strcpy(plaintext, buffer);
                    number_plaintext = Reference(buffer); //we reference the plaintext and convert it to numbers to do the encryption operations
                    ++file_counter;
                }

                else if (file_counter == 1) { //We recieved the key second
                    number_key = Reference(buffer); //converting to numbers
                    number_ciphertext = (int *) malloc(sizeof(int) * (plaintext_len + 1));
                    //Encryption operation
                    for (i = 0 ; i < plaintext_len ; ++i) {
                        number_ciphertext[i] = (number_plaintext[i] + number_key[i]); //plaintext + key
                        number_ciphertext[i] = mod(number_ciphertext[i], 27); //(plaintext + key) mod 27
                    }
                    number_ciphertext[i] = '\0';
                    ciphertext = Dereference(number_ciphertext, plaintext_len); //converting back from numbers to letters
                    ++file_counter; //to indicate we recieved all two files, and breaks the loop
                }
            }

            //Sending ciphertext back to client
            if (send(newsockfd, ciphertext, strlen(ciphertext), 0) < 0) {
                printf("Send failed");
                exit(1);
            }
            exit(0);
        }

        else if (pid > 0)  //parent processes
                close(newsockfd); //so it can be used by the child process
    } 


   return 0;
}



//Reference a string characters to numbers.
//INPUT: letter string
//OUTPUT: number string that is being referenced to it
int * Reference(char * a_string) {
    int * number_string;
    int len = 0;
    int i = 0 ;
    len = strlen(a_string);
    number_string = (int *) malloc(sizeof(int) * (len+1));

    for (i = 0 ; i < len ; ++i) {
        if (a_string[i] == 'A') number_string[i] = 0;
        else if (a_string[i] == 'B') number_string[i] = 9;
        else if (a_string[i] == 'C') number_string[i] = 12;
        else if (a_string[i] == 'D') number_string[i] = 22;
        else if (a_string[i] == 'E') number_string[i] = 7;
        else if (a_string[i] == 'F') number_string[i] = 5;
        else if (a_string[i] == 'G') number_string[i] = 3;
        else if (a_string[i] == 'H') number_string[i] = 2;
        else if (a_string[i] == 'I') number_string[i] = 19;
        else if (a_string[i] == 'J') number_string[i] = 24;
        else if (a_string[i] == 'K') number_string[i] = 26;
        else if (a_string[i] == 'L') number_string[i] = 6;
        else if (a_string[i] == 'M') number_string[i] = 1;
        else if (a_string[i] == 'N') number_string[i] = 13;
        else if (a_string[i] == 'O') number_string[i] = 18;
        else if (a_string[i] == 'P') number_string[i] = 20;
        else if (a_string[i] == 'Q') number_string[i] = 25;
        else if (a_string[i] == 'R') number_string[i] = 16;
        else if (a_string[i] == 'S') number_string[i] = 14;
        else if (a_string[i] == 'T') number_string[i] = 23;
        else if (a_string[i] == 'U') number_string[i] = 4;
        else if (a_string[i] == 'V') number_string[i] = 8;
        else if (a_string[i] == 'W') number_string[i] = 10;
        else if (a_string[i] == 'X') number_string[i] = 11;
        else if (a_string[i] == 'Y') number_string[i] = 15;
        else if (a_string[i] == 'Z') number_string[i] = 21;
        else if (a_string[i] == ' ') number_string[i] = 17;
        else {
            printf("Error: badfile: contains characters outside of the 27 characters range\n");
            exit(1);
        }
    }
    return number_string;
}



//Dereference an array of numbers to an array of characters.
//INPUT: number sting
//OUTPUT: it is equvelent letter string
char * Dereference(int * number_string, int len_of_array) {
    int i = 0;
    char * a_string;
    a_string = (char *) malloc (sizeof(char) * len_of_array + 1);

    for (i = 0 ; i < len_of_array ; ++i) {
        if (number_string[i] == 0) a_string[i] = 'A'; 
        else if (number_string[i] == 9) a_string[i] = 'B';
        else if (number_string[i] == 12) a_string[i] = 'C';
        else if (number_string[i] == 22) a_string[i] = 'D'; 
        else if (number_string[i] == 7) a_string[i] = 'E'; 
        else if (number_string[i] == 5) a_string[i] = 'F'; 
        else if (number_string[i] == 3) a_string[i] = 'G'; 
        else if (number_string[i] == 2) a_string[i] = 'H';
        else if (number_string[i] == 19) a_string[i] = 'I';
        else if (number_string[i] == 24) a_string[i] = 'J';
        else if (number_string[i] == 26) a_string[i] = 'K';
        else if (number_string[i] == 6) a_string[i] = 'L';
        else if (number_string[i] == 1) a_string[i] = 'M';
        else if (number_string[i] == 13) a_string[i] = 'N';
        else if (number_string[i] == 18) a_string[i] = 'O';
        else if (number_string[i] == 20) a_string[i] = 'P';
        else if (number_string[i] == 25) a_string[i] = 'Q';
        else if (number_string[i] == 16) a_string[i] = 'R';
        else if (number_string[i] == 14) a_string[i] = 'S';
        else if (number_string[i] == 23) a_string[i] = 'T'; 
        else if (number_string[i] == 4) a_string[i] = 'U';
        else if (number_string[i] == 8) a_string[i] = 'V';
        else if (number_string[i] == 10) a_string[i] = 'W';
        else if (number_string[i] == 11) a_string[i] = 'X';
        else if (number_string[i] == 15) a_string[i] = 'Y';
        else if (number_string[i] == 21) a_string[i] = 'Z';
        else if (number_string[i] == 17) a_string[i] = ' ';
        else {
            printf("Error: badfile: contains characters outside of the 27 characters range\n");
            exit(1);
        }
    }
    return a_string;
}



//Modulu mathmatical function
//a is the number, adn b is the mod. (such as a mod b) 
int mod (int a, int b)
{
    if(b < 0) 
        return mod(-a, -b);   
    int ret = a % b;
    if(ret < 0)
        ret+=b;
    return ret;
}

