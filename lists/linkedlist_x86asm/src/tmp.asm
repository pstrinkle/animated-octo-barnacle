
; static void unshift(struct item **head, int value);
_prepend:
    ; setup stack frame
    push ebp
    mov ebp, esp

    pusha

    mov ebx, [ebp+8]  ; head
    mov ecx, [ebp+12] ; value

    push ecx
    call _allocate
    add esp, 4

    mov edx, eax ; store allocated memory in edx

    mov eax, [ebx]
    cmp eax, 0     ; is *head NULL?
    je prependexit

    mov [edx], eax ; ins->next = *head

prependexit:

    mov [eax], edx ; *head = ins;

    popa

    ; fixup
    mov esp, ebp
    pop ebp
    ret
