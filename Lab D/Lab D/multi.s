section .bss
    struct: resd 1
    buffer: resb 600
section .data
    state: dw 0xACE1
    smaller_size: db 0
    bigger_size: db 0
section .rodata
    mask: dw 0x002D
    newline: db 10,0
    format: db "%02hhx", 0
    x_struct: db 5
    x_num: db 0xaa, 1,2,0x44,0x4f
    y_struct: db 6
    y_num: db 0xaa, 1,2,3,0x44,0x4f
section .text
global main
extern printf, stdin, fgets, strlen, malloc, free
main:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]        
    mov ebx, [ebp+12]       
    cmp eax, 1             
    je noArgs         
    mov eax, [ebx + 4]     
    cmp word[eax], "-I"    
    je flagI
    cmp word[eax], "-R"    
    je flagR   
    pop ebp
    ret                     

freePushs:
    call free               
    add esp, 4              
    call free               
    add esp, 4              
    call free               
    add esp, 4              
    pop ebp
    ret                    

flagR:
    call PRmulti            
    push eax                
    call PRmulti            
    push eax                
    call add_multi          
    push eax               
    call print_multi       
    jmp freePushs  

flagI:
    call getmulti           
    push eax               
    call getmulti          
    push eax               
    call add_multi         
    push eax              
    call print_multi        
    jmp freePushs 

noArgs:
    push y_struct         
    push x_struct           
    call add_multi         
    push eax               
    call print_multi        
    call free               
    add esp, 12             
    pop ebp
    ret                     



print_multi:
    push ebp                    
    mov ebp, esp                
    pushad                      
    mov edi, [ebp+8]            
    movzx ebx, byte[edi]        

printLoop:
    movzx ecx, byte[edi + ebx]  
    push ecx                    
    push format                 
    call printf                 
    add esp, 8                  
    sub ebx, 1                  
    cmp bl, 0                   
    jne printLoop                

NLPrint:
    push newline                
    call printf                 
    add esp, 4                 
    popad                       
    pop ebp                     
    ret                         



getmulti:
    push ebp                    
    mov ebp, esp                
    pushad                      
stdinRead:
    push dword[stdin]           
    push 600                    
    push buffer                 
    call fgets                  
    add esp, 12                 
getSize:
    push buffer                 
    call strlen                 
    add esp, 4                  
changeInfo:
    mov edi, eax                
    sub edi, 2                  
    shr eax, 1                  
    add eax, 1                  
allocateMemory:
    push eax                       
    call malloc                 
    mov dword[struct], eax      
saveInfo:
    mov esi, eax                
    pop eax                     
    dec eax                     
structSize:
    mov byte[esi], al           
    mov ecx, 1                  

scanInput:
    mov ebx, 0                  
    mov bh, byte[buffer + edi]  
    dec edi                     
    
    cmp bh, 'a'                 
    jge letter                  
    sub bh, '0'                
    jmp swapRegs
letter:
    sub bh, 'a'                 
    add bh, 0xa                 

swapRegs:
    mov bl, bh                  
    mov bh, 0                   
    cmp edi, 0
    jl combine
    mov bh, byte[buffer + edi]  
    dec edi                     

    cmp bh, 'a'                 
    jge letter2                
    sub bh, '0'                 
    jmp combine
letter2:
    sub bh, 'a'                
    add bh, 0xa                 

combine:
    shl bh, 4                   
    or bl, bh                  

addDigit:
    mov byte[esi + ecx], bl     
    inc ecx                     
    cmp edi, 0                  
    jge scanInput           
afterScan:
    popad                           
    mov eax, dword[struct]          
    pop ebp                         
    ret                             


MaxMin:
    movzx ecx, byte[eax]            
    movzx edx, byte[ebx]            
    cmp ecx, edx                    
    jae no_swap                     
    xchg eax, ebx                   
    no_swap:            
    ret                            



add_multi:
    push ebp                    
    mov ebp, esp                
    pushad                     

    mov eax, [ebp+8]            
    mov ebx, [ebp+12]         
    call MaxMin                
    
    push eax                    
    call print_multi            
    add esp, 4                 
    push ebx                    
    call print_multi           
    add esp, 4                  

    mov esi, eax                
    mov edi, ebx                
    movzx eax, byte[edi]        
    mov byte[smaller_size], al  
    movzx eax, byte[esi]        
    mov byte[bigger_size], al  
    add eax, 2                  
    push eax                   
    call malloc                 
    mov dword[struct], eax      
    pop ecx                     
    dec ecx                    
    mov byte[eax], cl          
    mov ecx, 0                  
    mov edx, 0                  
    inc esi                     
    inc edi                     
    inc eax                     
addMultiLoop:
    movzx ebx, byte[esi]       
    add ebx, ecx                
    movzx ecx, byte[edi]       
    add ebx, ecx              
    mov cl, bh                
    mov byte[eax], bl        
    inc edx                    
    inc esi                     
    inc edi                    
    inc eax                     
    cmp dl, byte[smaller_size]  
    jne addMultiLoop            

    cmp dl, byte[bigger_size]   
    je skip_add
addMultiLoop2:
    movzx ebx, byte[esi]        
    add ebx, ecx                
    mov cl, bh                  
    mov byte[eax], bl          
    inc edx                    
    inc esi                   
    inc eax                    
    cmp dl, byte[bigger_size]          
    jne addMultiLoop2
    skip_add:
    mov byte[eax], cl          
    popad                     
    pop ebp                  
    mov eax, dword[struct]    
    ret                       

rand_num:
    push ebp                    
    mov ebp, esp              
    pushad                    
    randNumLoop:
        mov bx, 0              
        movzx eax, word[state]  
        and ax, [mask]          
        jnp parity             
        mov bx, 0x8000          
    parity:
        movzx eax, word[state]  
        shr ax, 1               
        or ax, bx               
        mov word[state], ax     
    
    popad                       
    pop ebp                     
    movzx eax, word[state]      
    ret                         


PRmulti:
    push ebp                    
    mov ebp, esp               
    pushad                      
PRmiltiLoop:
    call rand_num               
    cmp al, 0                  
    je PRmiltiLoop     

    movzx ebx, al               
    add ebx, 1                  
    push ebx                    
    call malloc                
    mov dword[struct], eax      
    pop ebx                   
    dec ebx                     
    mov byte[eax], bl        
    mov esi, eax              
    mov edx, 0                 
    randLoop:
        call rand_num           
        mov byte[esi + edx + 1], al 
        inc edx                 
        dec ebx                 
        jnz randLoop         
    popad                       
    pop ebp                     
    mov eax, [struct]          
    ret                         