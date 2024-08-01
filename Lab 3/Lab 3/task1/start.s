section .data
    counter: dd 0
    argv_len: dd 0
    argc: dd 0
    argv: dd 0
    inFile: dd 0
    outFile: dd 1
    myChar: db 0
    new_line: db 10
    error_msg: db "Can't open file!", 10

section .text
global main
extern strlen
main:
set_arguments_variables:
    mov eax, dword[esp+8]    
    mov dword[argv], eax 
    mov eax, dword[esp+4] 
    mov dword[argc], eax 

print_arguments:
    start_loop:
        mov ecx, dword[argv]  
        mov edi, dword[counter]    
        mov ecx, dword[ecx + edi * 4] 
    get_argv_length:
        push ecx      
        call strlen      
        add esp, 4            
        mov dword[argv_len], eax     
    print_argv:
        mov eax, 0x4             
        mov ebx, 1                  
        mov edi, dword[counter]        
        mov ecx, dword[argv]          
        mov ecx, dword[ecx + edi * 4]  
        mov edx, dword[argv_len]   
        int 0x80                
    print_new_line:
        mov eax, 0x4        
        mov ebx, 1             
        mov ecx, new_line       
        mov edx, 1            
        int 0x80          
    check_argv:
        mov edi, dword[counter]   
        mov ecx, dword[argv]      
        mov ecx, dword[ecx + edi * 4]
        cmp word[ecx], "-i"  
        je open_input     
        cmp word[ecx], "-o" 
        je open_output      
    continued_loop:
        add dword[counter], 1      
        mov edi, dword[counter]  
        cmp edi, dword[argc]         
        jne start_loop   
    jmp encoder   
open_input:
    mov eax, 0x5     
    mov ebx, ecx          
    add ebx, 2            
    mov ecx, 0        
    int 0x80        
    cmp eax, -1  
    jle error            
    mov dword[inFile], eax      
    jmp continued_loop     
open_output:
    mov eax, 0x5          
    mov ebx, ecx           
    add ebx, 2               
    mov ecx, 101o             
    mov edx, 777o         
    int 0x80              
    cmp eax, -1              
    jle error
    mov dword[outFile], eax 
    jmp continued_loop    
encoder:
    get_char:
        mov eax, 0x3          
        mov ebx, dword[inFile]  
        mov ecx, myChar       
        mov edx, 1         
        int 0x80        
    check_read:
        cmp eax, 0          
        jle exit         
    compare_char:
        cmp byte[myChar], 'A'  
        jl print_char           
        cmp byte[myChar], 'z'
        jg print_char        
    encode_char:
        add byte[myChar], 1  
    print_char:
        mov eax, 0x4    
        mov ebx, dword[outFile]   
        mov ecx, myChar    
        mov edx, 1   
        int 0x80    
        jmp encoder     

error:
    popad
    mov eax, 0x4       
    mov ebx, 2     
    mov ecx, error_msg        
    mov edx, 17         
    int 0x80         
    jmp exit_fail      

exit:
    mov eax, 0x1      
    mov ebx, 0     
    int 0x80

exit_fail:
    mov eax, 0x1  
    mov ebx, 01    
    int 0x80
