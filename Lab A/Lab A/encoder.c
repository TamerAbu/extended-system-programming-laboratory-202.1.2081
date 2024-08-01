#include <stdio.h>
#include <string.h>

int main (int argc, char** argv){
    FILE* input = stdin;
    FILE* output = stdout;
    int debugMode = 0;
    int encoderMode = 0;
    char* key;
    for (int i = 1; i < argc; i++){
        if (strcmp (argv[i], "+D") == 0){
            debugMode = 1;
        } else if (strcmp (argv[i], "-D") == 0){
            debugMode = 0;
        } else if(debugMode){
            fprintf (stderr, "%s\n", argv[i]);
        }
        if (strncmp (argv[i], "+e", 2) == 0){
            encoderMode = 1;
            key = argv[i]+2;
        } else if (strncmp (argv[i], "-e", 2) == 0){
            encoderMode = -1;
            key = argv[i]+2;
        }
        if (strncmp (argv[i], "-i", 2) == 0){
            input = fopen(argv[i]+2, "r");
            if(input == NULL){
                fprintf(stderr, "error\n");
                return 0;
            }
        }
        if (strncmp (argv[i], "-o", 2) == 0){
            output = fopen(argv[i]+2, "w");
            if(output == NULL){
                fprintf(stderr, "error\n");
                return 0;
            }
        }

    }
    char inputChar;
    int index = 0;
    while (inputChar != EOF){
        inputChar = fgetc(stdin);
        char encodeChar = inputChar;
        if ((inputChar >= 'a' && inputChar <='z') || (inputChar >= 'A' && inputChar <='Z') || (inputChar >= '0' && inputChar <='9')){
            encodeChar = inputChar + (key[index]-0) * encoderMode;
        }
        if (inputChar >= 'A' && inputChar <='Z'){
            encodeChar = (encodeChar - 'A'+26)%26 + 'A';
        } else if (inputChar >= 'a' && inputChar <='z'){
            encodeChar = (encodeChar - 'a'+26)%26 + 'a';
        } else if (inputChar >= '0' && inputChar <='9'){
            encodeChar = (encodeChar - '0'+10)%10 + '0';
        }
        index++;
        if (key[index] == '\0'){
            index = 0;
        }
        if (inputChar != EOF){
            putc(encodeChar, output);
        }
    }
}
