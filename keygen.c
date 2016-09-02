//keygen.c

# include <stdio.h>
# include <stdlib.h>
# include <time.h>

int main(int argc, char * argv[]) {

    srand(time(NULL)); //seeding rand
    char * key; //Storing the generated key in
    int key_len = 0; //length of the key, which should be given in the argument as $keygen 30
    int dice1 = 0; 
    int dice2 = 0;
    int i = 0;

    if (argc < 2) {
        printf("Error: very few arguments\n");
        exit(1);
    }

    key_len = atoi(argv[1]); //converting from string type to int 
    key = (char *) malloc(key_len);

    //We have A-Z, which are a total of 26 character, plus a space. Gives a total of 27 characters
    //Statistically, the chances of a space appearing in the string is 1/27.
    for (i = 0 ; i < key_len ; ++i) {
        dice1 = rand() % 27;
        if (dice1 == 26) //That is the space character
            key[i] = 32; //32 is the space character in ascii
        else {
            dice2 = 65 + (rand() % 26); //generating a random number from 65 to 90, which corresponds
            key[i] = dice2;             // to ascii characters: A to Z
        }
    }

    printf("%s\n", key);

    return 0;
}
