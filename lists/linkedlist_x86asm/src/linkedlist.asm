; obviously all developed to work with 32-bit and not a 64-bit architecture.

%define SEVEN 7
%define SIZEOF_ITEM 8

; _thefun can be linked by calling 'thefun' in the C code.
global _returnseven
global _allocate
global _findvalue

extern _malloc

section .text

; int returnseven()
_returnseven:
    ; setup stack frame
    push ebp
    mov ebp, esp

    mov eax, SEVEN

    ; fixup
    mov esp, ebp
    pop ebp
    ret

; struct item {
;   struct item *next; [+0]
;   int value; [+4]
; };

; item *allocate(int value)
; sizeof(struct item) == 8
_allocate:
    ; setup stack frame
    push ebp
    mov ebp, esp

    ; so size_t is 32-bits when I checked the man page, and we're compiling
    ; this to 32-bits, but malloc seems to really want a 64-bit input value...
    push dword SIZEOF_ITEM
    push dword 0
    call _malloc
    add esp, 8

    mov ebx, eax ; store the pointer in ebx

    mov dword [ebx], 0 ; null pointer to it.

    mov ecx, [ebp+8] ; the value passed in
    mov [ebx+4], ecx ; the value passed in

    mov eax, ebx

    ; fixup
    mov esp, ebp
    pop ebp
    ret

; item *findvalue(item *list, int value)
_findvalue:
    ; setup stack frame
    push ebp
    mov ebp, esp

    ; walk linked-list
    mov ebx, [ebp+8]  ; list
    mov eax, [ebp+12] ; value

top:
    ; are we at the end of the list?
    cmp ebx, 0
    je exit

    ; is this the item?
    mov ecx, [ebx+4] ; get value from struct
    cmp ecx, eax
    je exit

    mov ebx, [ebx] ; curr = curr->next
    jmp top

exit:
    mov eax, ebx

    ; fixup
    mov esp, ebp
    pop ebp
    ret

