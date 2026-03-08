.code
; External variables - these match C++ global variables
EXTERN jmpBackAddress:QWORD
EXTERN entityList:QWORD
EXTERN hookHits:DWORD

; Export the hook function
PUBLIC hk_coords

hk_coords PROC
    ; Save entity pointer to entityList
    mov entityList, rbx
    
    ; Increment hook counter
    mov eax, hookHits
    inc eax
    mov hookHits, eax
    
    ; Original instructions that were overwritten
    movss xmm2, dword ptr [rbx + 54h]
    movss xmm0, dword ptr [rbx + 58h]
    subss xmm0, dword ptr [rdi + 58h]
    
    ; Jump back to original code
    jmp qword ptr [jmpBackAddress]
hk_coords ENDP

END