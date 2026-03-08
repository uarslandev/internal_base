.code

; External variables - these match C++ global variables
EXTERN jmpBackAddress:QWORD
EXTERN entityList:QWORD
EXTERN hookHits:DWORD

; Export the hook function
PUBLIC hk_coords

hk_coords PROC
    ; Save registers we'll modify
    push rax
    push rcx
    
    ; Original instructions
    movss xmm2, DWORD PTR [rbx + 54h]
    movss xmm0, DWORD PTR [rbx + 58h]
    subss xmm0, DWORD PTR [rdi + 58h]
    
    ; Store rbx value to entityList variable
    mov rax, OFFSET entityList
    mov QWORD PTR [rax], rbx
    
    ; Increment hookHits
    mov rax, OFFSET hookHits
    mov ecx, DWORD PTR [rax]
    inc ecx
    mov DWORD PTR [rax], ecx
    
    ; Get jump address
    mov rax, OFFSET jmpBackAddress
    mov rax, QWORD PTR [rax]
    
    ; Restore registers
    pop rcx
    pop rax
    
    ; Jump back to original code
    jmp rax
    
    ; We should never reach here, but MASM likes a ret
    ret
hk_coords ENDP

END