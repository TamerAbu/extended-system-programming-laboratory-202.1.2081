#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  /* TODO: Complete during task 2.a */
  for(int i = 0; i < array_length; i++){
    mapped_array[i] = f(array[i]);
  }
  return mapped_array;
}
//////////////////////////////////////////////////////
char my_get(char c){
  return fgetc(stdin);
}
/* Ignores c, reads and returns a character from stdin using fgetc. */

char cprt(char c){
  if( c >= 0x20 && c <= 0x7E){
    printf("%c\n", c);
  }  else {
    printf(".\n");
    c = '.';
  }
  return c;
}
/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */

char encrypt(char c){
  if( c >= 0x20 && c <= 0x7E){
    c++;
  }
  return c;
}
/* Gets a char c and returns its encrypted form by adding 1 to its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char decrypt(char c){
  if( c >= 0x20 && c <= 0x7E){
    c--;
  }
  return c;
}
/* Gets a char c and returns its decrypted form by reducing 1 from its value. If c is not between 0x20 and 0x7E it is returned unchanged */

char xprt(char c){
  printf("%x\n", c);
  return c;
}
/* xprt prints the value of c in a hexadecimal representation followed by a new line, and returns c unchanged. */ 
//////////////////////////////////////////////////////

struct fun_desc {
    char *name;
    char (*fun)(char);
};

int main(int argc, char **argv){
  char input[20];
  int num;
  struct fun_desc menu[] = {
    {"Get string", my_get},
    {"Print string", cprt},
    {"Encrypt",encrypt},
    {"Decrypt",decrypt},
    {"Print hex",xprt},
    {NULL,NULL}
  };
  char* carray = malloc(5);
  printf("please choose a function (cntrl^D for exit):\n");
  for (int i = 0; i < 5; i++){
      printf("%d.) %s\n", i, menu[i].name);
  }
  printf("Option: ");
  while (fgets(input, sizeof(input), stdin) != NULL){
    if (input[0] < 48 || input[0] > 57){
      printf("invalid input exiting...\n");
      exit(0);
    }
    num = atoi(input);
    if(num >=0 && num < 5){
      printf("Within bounds:\n");
      carray = map(carray, 5, menu[num].fun);
    } else {
      printf("Not within bounds");
      exit(0);
    }
    printf("DONE.\n\n");
    printf("please choose a function (cntrl^D for exit):\n");
    for (int i = 0; i < 5; i++){
        printf("%d.) %s\n", i, menu[i].name);
    }
    printf("Option: ");
  }
  free(carray);
  return 0;
}