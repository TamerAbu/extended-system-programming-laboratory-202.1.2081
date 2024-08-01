#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
 char debug_mode;
 char file_name[128];
 int unit_size;
 unsigned char mem_buf[10000];
 size_t mem_count;

 char display_mode;
} state;

struct fun_desc {
    char* name;
    void (*fun)(state*);
};

void Toggle_Debug_Mode (state* s){
    s->debug_mode = !s->debug_mode;
    if(s->debug_mode){
        fprintf(stderr, "Debug flag now on\n");
    } else {
        fprintf(stderr, "Debug flag now off\n");
    }
}

void Set_File_Name (state* s){
    printf("Please enter file name:\n");
    fgets(s->file_name, 100, stdin);
    s->file_name[strlen(s->file_name)-1] = '\0';
    if(s->debug_mode){ 
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void Set_Unit_Size(state* s){
    printf("Please enter unit size:\n");
    char buffer[100];
    fgets(buffer, 100, stdin);
    int val;
    sscanf(buffer, "%d\n", &val);
    if(val == 1 || val == 2 || val == 4){
        s->unit_size = val;
        if(s->debug_mode){
            fprintf(stderr, "Debug: unit size set to '%d'\n", s->unit_size);
        }
    } else {
        fprintf(stderr,"Invalid unit size\n");
    }
}

void Load_Into_Memory (state* s){
    if (strcmp (s->file_name, "") == 0){
        fprintf(stderr, "File name is empty\n");
        return;
    }
    FILE* file = fopen(s->file_name, "r");
    if(file == NULL){
        perror("fopen");
        return;
    }
    printf("Please enter location(hexadecimal) length(decimal):\n");
    char buffer[100];
    fgets(buffer, 100, stdin);
    int location, length;
    sscanf(buffer, "%x %d\n", &location, &length);
    if(s->debug_mode){
        fprintf(stderr, "Debug: file_name: %s\n", s->file_name);
        fprintf(stderr, "Debug: location: %x\n", location);
        fprintf(stderr, "Debug: length: %d\n", length);
    }
    fseek(file, location, SEEK_SET);
    int nread = fread(s->mem_buf, s->unit_size, length, file);
    fprintf(stderr, "Loaded %d units into memory\n", nread);
    fclose(file);
}

void Toggle_Display_Mode(state* s){
    s->display_mode = !s->display_mode;
    if(s->display_mode){
        fprintf(stderr, "Display flag now on, hexadecimal representation\n");
    } else {
        fprintf(stderr, "Display flag now off, decimal representation\n");
    }
}

void Memory_Display(state* s){
    static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    static char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

    printf("Please enter address(hexadecimal) length(decimal):\n");
    char input[100];
    fgets(input, 100, stdin);
    int address, length;
    sscanf(input, "%x %d\n", &address, &length);
    void* buffer = (void*)address;
    if(address == 0){
        buffer = s->mem_buf;
    }
    for(int i = 0; i < length; i++){
        int var = *((int*)(buffer + i * s->unit_size));
        if(s->display_mode){
            printf(hex_formats[s->unit_size-1], var);
        } else {
            printf(dec_formats[s->unit_size-1], var);
        }
    }
}

void Save_Into_File(state* s){
    if(strcmp(s->file_name, "") == 0){
        fprintf(stderr, "File name is empty\n");
        return;
    }
    FILE* file = fopen(s->file_name, "r+");
    if(file == NULL){
        perror("fopen");
        return;
    }

    printf("Please enter address(hexadecimal) target(hexadecimal) length(decimal):\n");
    char input[100];
    fgets(input, 100, stdin);
    int source, target, length;
    sscanf(input, "%x %x %d\n", &source, &target, &length);
    void* buffer = (void*)source;

    if(source == 0){
        buffer = s->mem_buf;
    }

    fseek(file, target, SEEK_SET);
    fwrite(buffer, s->unit_size, length, file);
    
    fclose(file);
}

void Memory_Modify (state* s){
    char input[100];
    int location, val;
    fprintf(stderr, "Please enter location(hexadecimal) val(hexadecimal)\n");
    fgets(input, 100, stdin);
    sscanf(input, "%x %x", &location, &val);
    memcpy(s->mem_buf+location, &val, s->unit_size);
}

void Quit(state* s){
    free(s);
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv){
    struct fun_desc menu[] = { { "Toggle Debug Mode", Toggle_Debug_Mode}, { "Set File Name", Set_File_Name}, 
    { "Set Unit Size", Set_Unit_Size}, { "Load Into Memory", Load_Into_Memory}, 
    { "Toggle Display Mode", Toggle_Display_Mode}, { "Memory Display", Memory_Display}, 
    { "Save Into File", Save_Into_File}, { "Memory Modify", Memory_Modify}, 
    { "Quit", Quit }, { NULL, NULL } };
    int menuSize = sizeof(menu) / sizeof(struct fun_desc) - 1;
    state* s = malloc(sizeof(state));
    s->debug_mode = 0;
    s->display_mode = 0;
    strcpy(s->file_name, "");
    s->unit_size = 1;
    char input[100];
    while(1){
        printf("Choose action:\n");
        for(int i = 0; i < menuSize; i++){
            printf("%d-%s\n", i, menu[i].name);
        }
        printf("option: ");
        if(fgets(input, 100, stdin) == NULL){
            break;
        }
        int option;
        sscanf(input, "%d\n", &option);
        if(option < 0 || option > menuSize-1){
            printf("Not within bounds\n");
            break;
        }
        printf("Within bounds\n");
        menu[option].fun(s);
        printf("DONE.\n\n");
    } 
}