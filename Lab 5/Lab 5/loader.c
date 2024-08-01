#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

extern int startup(int argc, char **argv, void (*start)());

void task0(Elf32_Phdr * phdr, int index) {
    printf("Program header number %d at address %x\n", index, phdr->p_vaddr);
}
//from TA
char* type_string(Elf32_Phdr* phdr){
    if(phdr->p_type == PT_NULL) { 
        return "NULL";
    }
    if(phdr->p_type == PT_LOAD) {
        return "LOAD";
    }
    if(phdr->p_type == PT_DYNAMIC) {
        return "DYNAMIC";
    }
    if(phdr->p_type == PT_INTERP) {
        return "INTERP";
    }
    if(phdr->p_type == PT_NOTE) {
        return "NOTE";
    }
    if(phdr->p_type == PT_SHLIB) {
        return "SHLIB";
    }
    if(phdr->p_type == PT_PHDR) {
        return "PHDR";
    }
    if(phdr->p_type == PT_TLS) {
        return "TLS";
    }
    if(phdr->p_type == PT_GNU_EH_FRAME) {
        return "GNU_EH_FRAME";
    }
    if(phdr->p_type == PT_GNU_STACK) {
        return "GNU_STACK";
    }
    if(phdr->p_type == PT_GNU_RELRO) {
        return "GNU_RELRO";
    }
    return "UNKNOWN";
}

void task1a (Elf32_Phdr * phdr, int index){
    char type[4];
    type[3] = '\0';
    if(phdr->p_flags & PF_W) {
        type[1] = 'W'; 
    }else {
            type[1] = ' ';
    }
    if(phdr->p_flags & PF_X) { 
        type[2] = 'E';  
    }else {
        type[2] = ' ';  
    }  
    if(phdr->p_flags & PF_R) {
        type[0] = 'R'; 
    }else {
        type[0] = ' ';
    }   
    printf("%s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x %s 0%x\n",
        type_string(phdr), phdr->p_offset, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz, type, phdr->p_align);
}

void task1b(Elf32_Phdr * phdr, int index){
    printf("Mapping flags: MAP_PRIVATE | MAP_FIXED\n");
    printf("Protection flags: ");
    if(phdr->p_flags & PF_X) {
        printf("PROT_EXEC ");
    }
    if(phdr->p_flags & PF_W) {
        printf("PROT_WRITE ");
    }
    if(phdr->p_flags & PF_R) {
        printf("PROT_READ ");
    }
    printf("\n");
}

void load_phdr(Elf32_Phdr *phdr, int fd){
    if(phdr->p_type == PT_LOAD){
        int mapping_flags = MAP_PRIVATE | MAP_FIXED;
        int protection_flags = PROT_NONE;
        if(phdr->p_flags & PF_R) {
            protection_flags = protection_flags | PROT_READ;
        }
        if(phdr->p_flags & PF_W) {
            protection_flags = protection_flags | PROT_WRITE;
        }
        if(phdr->p_flags & PF_X) {
            protection_flags = protection_flags | PROT_EXEC;
        }
        void* vaddr = (void*)(phdr->p_vaddr&0xfffff000);
        int offset = phdr->p_offset&0xfffff000;
        int padding = phdr->p_vaddr & 0xfff;
        void* map = mmap(vaddr, phdr->p_memsz+padding, protection_flags, mapping_flags, fd, offset);  
        if(map == MAP_FAILED){
            perror("map failed");
        } else {
            task1a(phdr, -1);
        }
    }
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr* header = (Elf32_Ehdr*) map_start;
    Elf32_Phdr* phdr = (Elf32_Phdr*)(map_start + header->e_phoff);
    for(int i = 0; i < header->e_phnum; i++){
        if(arg == -1) {
            func(phdr + i, i);
        } else {
            func(phdr + i, arg);
        }
    }
    return 0;
}

int main(int argc, char** argv){
    if(argc == 1){
        fprintf(stderr, "Bad use: %s file_name\n", argv[0]);
        exit(1);
    }
    struct stat sb;
    int fd = open(argv[1], O_RDONLY);
    if(fd < 0){
        perror("could not open");
        exit(1);
    }
    if(fstat(fd, &sb) < 0){
        perror("fstat failed");
        close(fd);
        exit(1);
    }

    void* map_start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(map_start == MAP_FAILED){
        perror("map failed");
        close(fd);
        exit(1);
    }

    foreach_phdr(map_start, task0, -1);
    printf("Type Offset VirtAddr PhysAddr FileSiz MemSiz Flg Align\n");
    foreach_phdr(map_start, task1a, -1);
    foreach_phdr(map_start, task1b, -1);
    foreach_phdr(map_start, load_phdr, fd);
    startup(argc-1, argv+1, (void *)(((Elf32_Ehdr*)map_start)->e_entry));
    munmap(map_start, sb.st_size);
}