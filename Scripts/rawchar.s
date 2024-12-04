section .data
    STDIN equ 0
    STDOUT equ 1
    F_GETFL equ 3
    F_SETFL equ 4
    O_NONBLOCK equ 0x800
    FD_ZERO equ 0
    FD_SET equ 1
    TCGETS equ 0x5401    ; ioctl code for getting terminal settings
    TCSETS equ 0x5402    ; ioctl code for setting terminal settings
    ICANON equ 0x0000002 ; canonical mode flag
    ECHO equ 0x0000008   ; echo flag

section .bss
    char resb 1
    fd_set resb 128
    termios:
        c_iflag resd 1
        c_oflag resd 1
        c_cflag resd 1
        c_lflag resd 1
        c_line resb 1
        c_cc resb 19
        pad resb 4
    orig_termios:
        resb 44
    timeval:
        tv_sec resd 1
        tv_usec resd 1

section .text
    global init_terminal      ; Note the underscore prefix for C compatibility
    global restore_terminal
    global setup_and_read
    global set_nonblocking

; Initialize terminal settings
init_terminal:
    push ebp
    mov ebp, esp
    push ebx                ; Save registers that cdecl requires to be preserved
    push esi
    push edi

    ; Get current terminal settings
    mov eax, 54            ; sys_ioctl
    mov ebx, STDIN
    mov ecx, TCGETS
    mov edx, termios
    int 0x80

    ; Check for error
    cmp eax, -1
    je .error

    ; Save original settings
    push ecx
    mov ecx, 44            ; size of termios struct
    mov esi, termios
    mov edi, orig_termios
    rep movsb
    pop ecx

    ; Modify flags
    mov eax, [termios + 12] ; c_lflag
    and eax, ~(ICANON | ECHO)
    mov [termios + 12], eax

    ; Set VMIN and VTIME
    mov byte [termios + 17], 1  ; VMIN = 1
    mov byte [termios + 18], 0  ; VTIME = 0

    ; Apply new settings
    mov eax, 54            ; sys_ioctl
    mov ebx, STDIN
    mov ecx, TCSETS
    mov edx, termios
    int 0x80

    ; Check for error
    cmp eax, -1
    je .error

    xor eax, eax           ; Return 0 on success
    jmp .exit

.error:
    mov eax, -1            ; Return -1 on error

.exit:
    pop edi                ; Restore saved registers
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret

; Restore terminal settings
restore_terminal:
    push ebp
    mov ebp, esp
    push ebx               ; Save registers

    ; Restore original settings
    mov eax, 54           ; sys_ioctl
    mov ebx, STDIN
    mov ecx, TCSETS
    mov edx, orig_termios
    int 0x80

    ; Check for error
    cmp eax, -1
    je .error

    xor eax, eax          ; Return 0 on success
    jmp .exit

.error:
    mov eax, -1           ; Return -1 on error

.exit:
    pop ebx               ; Restore saved registers
    mov esp, ebp
    pop ebp
    ret

; Set non-blocking mode
set_nonblocking:
    push ebp
    mov ebp, esp
    push ebx              ; Save registers

    ; Get current flags
    mov eax, 221         ; sys_fcntl
    mov ebx, STDIN
    mov ecx, F_GETFL
    int 0x80

    ; Check for error
    cmp eax, -1
    je .error

    ; Save original flags
    push eax

    ; Set non-blocking mode
    or eax, O_NONBLOCK
    mov ecx, F_SETFL
    int 0x80

    ; Return original flags
    pop eax
    jmp .exit

.error:
    mov eax, -1

.exit:
    pop ebx              ; Restore saved registers
    mov esp, ebp
    pop ebp
    ret

; Read character with timeout
setup_and_read:
    push ebp
    mov ebp, esp
    push ebx             ; Save registers
    push esi
    push edi

    ; Clear fd_set
    xor eax, eax
    mov ecx, 32         ; 128 bytes = 32 dwords
    mov edi, fd_set
    rep stosd

    ; Set stdin bit
    bts dword [fd_set], 0

    ; Set timeout
    mov dword [tv_sec], 0
    mov dword [tv_usec], 100000  ; 0.1 seconds

    ; Call select
    mov eax, 142        ; sys_select
    mov ebx, 1          ; nfds
    mov ecx, fd_set     ; read fds
    xor edx, edx        ; write fds
    xor esi, esi        ; error fds
    mov edi, timeval    ; timeout
    int 0x80

    ; Check select result
    cmp eax, 0
    je .timeout
    cmp eax, -1
    je .error

    ; Read character
    mov eax, 3         ; sys_read
    mov ebx, STDIN
    mov ecx, char
    mov edx, 1
    int 0x80

    cmp eax, 1         ; Check bytes read
    jne .error

    ; Return character
    movzx eax, byte [char]
    jmp .exit

.timeout:
    mov eax, -1
    jmp .exit

.error:
    mov eax, -1

.exit:
    pop edi            ; Restore saved registers
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noexec