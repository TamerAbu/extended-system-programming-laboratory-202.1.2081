#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    unsigned char* sig;
    char virusName[16];
} virus;

typedef struct link link;
struct link {
    link *nextVirus;
    virus *vir;
};

typedef struct fun_desc {
  char *name;
  void (*fun)();
} fun_desc;

//global
int magicNumberFlag = 0;
char *fileName;


link* allVirus = NULL;
virus* readVirus(FILE* file){
    virus* temp = malloc(sizeof(virus));
    if (fread(&(temp->SigSize), sizeof(unsigned short), 1, file) != 1){
        free(temp);
        return NULL;
    }
    if (magicNumberFlag == 1){
        temp->SigSize = (temp->SigSize >> 8) | (temp->SigSize << 8);
    }
    if (fread(temp->virusName, sizeof(char), 16, file) != 16){
        free(temp);
        return NULL;
    }
    temp->virusName[15] = '\0';
    temp->sig = malloc(temp->SigSize);
    if (fread(temp->sig, sizeof(unsigned char), temp->SigSize, file) != temp->SigSize){
        free(temp->sig);
        free(temp);
        return NULL;
    }
    return temp;
}

void printVirus(virus *virus, FILE *output){
    if (virus == 0){
    return; 
    }else{
    fprintf(output, "Virus name: %s\n", virus->virusName);
    fprintf(output, "Virus size: %d\n", virus->SigSize);
    fprintf(output, "signature:\n");
    int i;
    for (i = 0; i < virus->SigSize; i++){
        fprintf(output, "%02hhX ", (unsigned int)((virus->sig)[i]));
        if (i % 32 == 31)
            fprintf(output, "\n");
    }
    if (i % 32 != 0)
        fprintf(output, "\n");
    fprintf(output, "\n");
    }
}

void list_print(link *virus_list, FILE *output) {
    while(virus_list != NULL){
        printVirus(virus_list->vir, output);
        virus_list = virus_list->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data){
    link *NextLink = malloc(sizeof(link));
    NextLink->vir = data;
    NextLink->nextVirus = NULL;
	if(virus_list == NULL){
        return NextLink;
    }
	link *temp = virus_list;
	while(temp->nextVirus != 0)
	    temp = temp->nextVirus;
	temp->nextVirus = NextLink;
	return virus_list;
}

void list_free(link *virus_list){
    link *current;
    while (virus_list != NULL){
        current = virus_list;
        virus_list = virus_list->nextVirus;
        if(current->vir){
            free(current->vir->sig);
            free(current->vir);
        }
        free(current);
    }
}


void quit(link* virus_list){
    printf ("exiting...\n");
    list_free(virus_list);
    exit(0);
}

void loadSig(link* virusList){
    printf("Please enter a signature file name:\n");
    char tmp[128];
    fgets(tmp, 128, stdin);
    char fileName[128];
    sscanf(tmp, "%s", fileName);
    FILE *file = fopen(fileName, "r");

    if (file == NULL){
        fprintf(stderr, "ERROR! invalid file!\n");
        exit(1);
    }
    char magicNumber[5];
    if (fread(magicNumber, sizeof(char), 4, file) != 4){
        fprintf(stderr, "ERROR! invalid magic number!\n");
        exit(1);
    }
    magicNumber[4] = '\0';
    if (strcmp(magicNumber, "VISB") == 0){
        magicNumberFlag = 1;
    }
    else if (strcmp(magicNumber, "VISL") == 0){
        magicNumberFlag = 0;
    }
    else{
        fprintf(stderr, "ERROR! invalid magic number!\n");
    }

    link *next = virusList;

    int c;
    while ((c = fgetc(file)) != EOF) {
        ungetc(c, file);
        virus *nextvirus = readVirus(file);
        if (nextvirus != NULL) {
            next = list_append(next, nextvirus);
        }
    }
    fclose(file);
    allVirus = next;
}


void printall(link* virusLink){
    list_print(virusLink, stdout);
}

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    for (char *ptr = buffer; ptr < buffer + size; ptr++) {
        for (link *curr = virus_list; curr != NULL; curr = curr->nextVirus) {
            virus *curr_virus = curr->vir;
            if (curr_virus != NULL && curr_virus->SigSize != 0 && curr_virus->SigSize <= buffer + size - ptr) {
                if (memcmp(ptr, curr_virus->sig, curr_virus->SigSize) == 0) {
                    printf("\nvirus detected!");
                    printf("\nlocation: %d\n", (int)(ptr - buffer));
                    printf("virus name: %s\n", curr_virus->virusName); 
                    printf("size: %d\n\n",curr_virus->SigSize); 
                    printf("DONE.\n\n"); 
                }
            }
        }
    }
} 

void detectSig (link *virus_list){
    FILE *file = fopen(fileName, "r");
    if (file == NULL){
        fprintf(stderr, "ERROR! invalid file!\n");
        exit(1);
    }
    char *buffer = malloc(10000);
    int numBytes = fread(buffer, 1, 10000, file);
    detect_virus(buffer, numBytes, virus_list);
    fclose(file);
    free(buffer);
}

void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "r+");
    char a = 0xc3;
    fseek(file,signatureOffset,SEEK_SET);
    fwrite(&a, 1, 1, file);
    fclose(file);
}

void fix_file_detector(char *buffer, unsigned int size, link *virus_list) {
    for (char *ptr = buffer; ptr < buffer + size; ptr++) {
        for (link *curr = virus_list; curr != NULL; curr = curr->nextVirus) {
            virus *curr_virus = curr->vir;
            if (curr_virus->SigSize <= buffer + size - ptr) {
                if (memcmp(ptr, curr_virus->sig, curr_virus->SigSize) == 0) {
                    neutralize_virus(fileName, (int)(ptr - buffer));
                }
            }
        }
    }
} 

void fixFile(link *virus_list){
    FILE *file = fopen(fileName, "r");
    if (file == NULL){
        fprintf(stderr, "ERROR! invalid file!\n");
        exit(1);
    }
    char *buffer = malloc(10000);
    int numBytes = fread(buffer, 1, 10000, file);
    fix_file_detector(buffer, numBytes, virus_list);
    fclose(file);
    free(buffer);

}

int main(int argc, char **argv){
    struct fun_desc menu[] = {
        {"Load signatures", &loadSig},
            {"Print signatures", &printall},
                {"Detect viruses", &detectSig},
                    {"Fix file", &fixFile},
                        {"Quit", &quit},
                            {NULL, NULL}
    };

    if(argc != 2){
        fprintf(stderr,"ERROR %s filename\n", argv[0]); 
        exit(1);
    }
    fileName = argv[1];
    while (1){
        printf("please choose a function (cntrl^D for exit):\n");
        for (int i = 0; i < 5; i++){
            printf("%d) %s\n", i+1, menu[i].name);
        }
        char input[4];
        fgets (input, 4, stdin);
        int choice = atoi(input);
        if (input[0] < 1 || choice >= sizeof(menu)/sizeof(menu[0])){
            printf("invalid input exiting...\n");
            return 0;
        }
        printf("Option: %d\n", choice);
        printf("Within bounds\n");
        menu[choice-1].fun(allVirus);
    }
    return 0;
}

