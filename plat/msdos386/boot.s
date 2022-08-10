#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
#define STACK_BUFFER 128 /* number of bytes to leave for stack */

! g 18b to break at the retf before coming here
begtext:
    ! On entry, the stub has cs and ds pointing at the 32-bit
    ! segment, but ss is still pointing at the 16-bit segment.
    !
    !   si:di memory handle of linear block
    !   ax: pmode code segment of stub
    !   dx: pointer to realloc routine
    !   fs: data segment (just a clone of the code segment)

    ! Resize the segment to include the BSS.

    o16 cseg mov (realloc_ptr+4), ax
    cseg mov (realloc_ptr+0), edx
    mov eax, endbss
    cseg callf (realloc_ptr)

    ! Clear BSS.

    mov edi, begbss
    mov ecx, endbss+1
    sub ecx, edi
    shr ecx, 1
    xor eax, eax
    cld
    rep stosw

    ! It's now safe to switch stacks.

    cli
    mov eax, ds
    mov ss, eax
    mov sp, .stack
    sti

    ! Create a handle for low 1MB access.

    o16 mov ax, 0x0000
    o16 mov cx, 1
    int 0x31                    ! allocate LDT
    mov es, ax
    o16 mov bx, ax
    o16 mov (.doshandle), bx

    xor ecx, ecx
    xor edx, edx
    o16 mov ax, 0x0007
    int 0x31                    ! set base address to 0
    o16 mov cx, 0x0010
    o16 mov ax, 0x0008
    int 0x31                    ! set limit to 1MB

    mov cx, ds
    and cx, 3
    shl cx, 5
    or cx, 0xc093               ! 32-bit, big, data, r/w, expand-up
    mov ax, 0x0009
    int 0x31                    ! set descriptor access rights

    ! Locate the PSP.
    
    movb ah, 0x62
    int 0x21
    movzx ebx, bx
    shl ebx, 4                  ! convert to linear address
    mov (.psp), ebx

    ! Get the size of the environment variables plus (if present) the
    ! program name.  Also count the number of environment variables.
    xor di, di
    mov es, 0x002C(di)
    ! ax = 0 from above
    cwd             ! dx = count of env. vars.
    ! cx = 0 from above
    dec cx              ! cx = max. str. bytes to scan
    scasb               ! handle special case of empty env.
    jz is_empty_env
size_env:
    inc dx
    repnz scasb
    scasb
    jnz size_env
is_empty_env:
    cmp bp, 2
    jz no_argv0
    scasw
    repnz scasb
no_argv0:

    ! Copy out the environment variables and (possibly) program name
    ! onto the stack.
    mov si, di
    dec si
    std
copy_env:
    and si, -2
    eseg lodsw
    push ax
    jnz copy_env
    mov cx, sp

    ! Reset DF and es properly.
    cld
    push ss
    pop es

    ! Reserve space for argc and the argv and envp pointers on the
    ! stack.  These will be passed to __m_a_i_n later.
    sub sp, 6
    mov ax, sp

    ! Build up argc, argv[], and envp[].
    push ax             ! output buffer for argc, argv, envp
    push bp             ! MS-DOS version
    push cx             ! env. string data
    push dx             ! count of env. vars.
    mov ax, 0x0080
    push ax             ! raw command line
    call __sys_initmain
    add sp, 10

    ! Bail out if something went wrong.
    test ax, ax
    jnz no_room

    ! argc, argv, and envp are now at the stack top.  Now go.
    call __m_a_i_n
    add sp, 6
    push ax
    call _exit

no_room:
    mov dx, no_room_msg
    !call dos_msg
    movb al, -1
    jmp al_exit

    ! Exit.
.define __exit
.extern __exit
.define EXIT
.extern EXIT
__exit:
EXIT:
    pop bx
    pop ax
al_exit:
    movb ah, 0x4c
    int 0x21

    ! This must be in the code segment due to bootstrap issues.
realloc_ptr:
    .data2 0
    .data4 0

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

.sect .rom

! Some text messages.
bad_sys_msg: .ascii 'Bad DOS$'
no_room_msg: .ascii 'No room$'

! Some magic data. All EM systems need these.

.define .trppc, .ignmask, _errno
.comm .trppc, 4
.comm .ignmask, 4
.comm _errno, 4

.comm .doshandle, 2
.comm .psp, 4

.sect .bss
    .space 512
.stack:

! vim: ts=4 sw=4 et ft=asm

