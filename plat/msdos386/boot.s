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
    !   ax: pmode code segment of stub
    !   bx: pmode data segment of stub
    !   cx: rmode segment of stub
    !   dx: pointer to realloc routine (in stub)
    !   si: pointer to interrupt routine (in stub)
    !   di: pointer to transfer buffer (in stub)
    !   bp: rmode PSP segment

    ! Resize the segment to include the BSS.

    o16 mov (realloc_ptr+4), ax
    mov (realloc_ptr+0), edx
    o16 mov (pmode_cs), ax
    o16 mov (pmode_ds), bx
    o16 mov (rmode), cx
    o16 mov (interrupt_ptr+4), ax
    mov (interrupt_ptr+0), esi
    o16 mov (transfer_buffer_ptr), di

    mov eax, __end
    callf (realloc_ptr)

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
    mov es, bx

    xor ecx, ecx
    xor edx, edx
    o16 mov ax, 0x0007
    int 0x31                    ! set base address to 0
    o16 dec dx
    o16 mov cx, 0x000f
    o16 mov ax, 0x0008
    int 0x31                    ! set limit to 1MB

    mov cx, ds
    and cx, 3
    shl cx, 5
    or cx, 0xc093               ! 32-bit, big, data, r/w, expand-up
    mov ax, 0x0009
    int 0x31                    ! set descriptor access rights

    ! Locate the PSP.
    
    movzx esi, bp
    shl esi, 4                  ! convert to linear address

    ! Copy the whole thing into 32-bit memory.

    mov ecx, 0x100/4
    mov edi, _psp
    cld
1:
    eseg lods
    mov (edi), eax
    add edi, 4
    loop 1b

    ! Find the environment.

    mov es, (_psp + 0x002C)     ! converted to pmode segment
    
    ! Count the size of the environment variable block.

    xorb al, al                 ! byte to test for
    xor edi, edi
    xor edx, edx
    cld
    mov ecx, -1
    scasb
    jz 2f                       ! empty environment
1:
    repnz scasb
    inc edx
    scasb
    jnz 1b
2:
    add edi, 2                  ! skip the mysterious word
    repnz scasb                 ! program name

    ! Copy the whole environment block onto the stack.

    mov esi, edi
    add esi, 3
    and esi, ~3                 ! round up
    jz empty_environment
    mov ecx, esi
    shr ecx, 2                  ! number of dwords
    std
    sub esi, 4
1:
    eseg lods
    push eax
    loop 1b
empty_environment:
    mov ecx, esp                ! environment data

    ! Reset DF and es properly.

    cld
    push ss
    pop es

    ! Reserve space for argc and the argv and envp pointers on the
    ! stack.  These will be passed to __m_a_i_n later.

    sub esp, 3*4
    mov eax, esp

    ! Build up argc, argv[], and envp[].
    push eax            ! output buffer for argc, argv, envp
    push 3              ! MS-DOS version
    push ecx            ! env. string data
    push edx            ! count of env. vars.
    push _psp+0x80      ! raw command line
    call __sys_initmain
    add esp, 5*4

    ! Bail out if something went wrong.
    test eax, eax
    jnz no_room

    ! argc, argv, and envp are now at the stack top.  Now go.
    call __m_a_i_n
    add sp, 3*4
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

    ! These must be in the code segment due to bootstrap issues.
realloc_ptr:         .space 6 ! far
interrupt_ptr:       .space 6 ! far
transfer_buffer_ptr: .space 2 ! near
rmode:               .space 2
pmode_cs:            .space 2
pmode_ds:            .space 2

.define realloc_ptr
.define interrupt_ptr
.define rmode
.define pmode_cs
.define pmode_ds
.define transfer_buffer_ptr

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
.comm _psp, 256

.sect .bss
    .space 32*1024
.stack:

! vim: ts=4 sw=4 et ft=asm

