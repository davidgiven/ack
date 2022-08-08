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
.use16
exe_header:
    .data2 0x5a4d               ! magic number
    .data2 0                    ! number of bytes in last loadable page
    .data2 exe_text_paras       ! size of .exe, in pages
    .data2 0                    ! number of relocation entries
    .data2 0                    ! start of loadable area, in 16-byte paragraphs
    .data2 exe_ram_paras        ! required RAM size, in 16-byte paragraphs
    .data2 0                    ! maximum RAM siz, in 16-byte paragraphse
    .data2 0                    ! initial SS, relative to program
    .data2 exe_stack            ! initial SP
    .data2 0                    ! checksum (ignored)
    .data2 exe_start            ! initial IP
    .data2 0                    ! initial CS, relative to program
    .data2 0                    ! offset of relocation table
    .data2 0                    ! overlay number

dpmi_edi    = 0x00
dpmi_esi    = 0x04
dpmi_ebp    = 0x08
dpmi_ebx    = 0x10
dpmi_edx    = 0x14
dpmi_ecx    = 0x18
dpmi_eax    = 0x1c
dpmi_flags  = 0x20
dpmi_es     = 0x22
dpmi_ds     = 0x24
dpmi_fs     = 0x26
dpmi_gs     = 0x28
dpmi_ip     = 0x2a
dpmi_cs     = 0x2c
dpmi_sp     = 0x2e
dpmi_ss     = 0x30

dpmi_rs     = 0x32 ! WORD: real segment
dpmi_psp    = 0x34 ! WORD: PSP segment
dpmi_switch = 0x38 ! DWORD: far pointer of pmode switch routine

    .seek 0x3c
exe_start:
    ! On entry, DS=ES=PSP. Make DS=CS, so we're running in tiny mode.

    push cs
    pop ds
    mov (dpmi_rs), ds
    mov (dpmi_psp), es

    ! Initialise DPMI.

    mov ax, 0x1687
    int 0x2f
    or ax, ax
    jnz no_dpmi
    mov (dpmi_switch+0), di     ! write back PMODE switch routine
    mov (dpmi_switch+2), es
    or si, si                   ! do we need a DPMI private area?
    jz 1f

    mov bx, si
    movb ah, 0x48
    int 0x21                    ! allocate memory from DOS
    mov es, ax                  ! data area segment -> es
1:
    ! Switch to protected mode.

    mov ax, 1                   ! 32-bit app
    callf (dpmi_switch)
    jnc pmode                   ! Success!

    ! Could not switch to protected mode.

    mov dx, no_pmode_msg
    jmp exit_with_error

no_dpmi:
    mov dx, no_dpmi_msg
    ! fall through

! Displays the message in dx and exits.
exit_with_error:
    movb ah, 9
    int 0x21                    ! print $-terminated string
    mov ax, 0x4cff
    int 0x21                    ! terminate with error code al

pmode:
    ! We're now in protected mode! Switch our code segment to 32-bit mode.

    mov cx, cs
    lar cx, cx
    movb cl, ch
    movb ch, 0xc0
    mov bx, cs
    mov ax, 0x0009
    int 0x31                    ! Make selector 32 bit

    .use32
    o16 mov (dpmi_edx), go_msg
    o16 mov ax, (dpmi_rs)
    o16 mov (dpmi_ds), ax
    movb (dpmi_eax+1), 9
    call int21
    o16 mov ax, 0x4c00
    int 0x21

    ! Simulate DOS interrupt bx.
int21:
    o16 mov bx, 0x21
callint:
    o16 xor ax, ax
    o16 mov (dpmi_ss), ax           ! zero stack: DPMI host allocates one.
    o16 mov (dpmi_sp), ax
    push ds
    pop es
    mov di, ax
    o16 mov ax, 0x300
    int 0x31                    ! simulate DOS interrupt
    push cs
    pop ds
    ret

go_msg:
    .ascii "Go!$"
no_pmode_msg:
    .ascii "Couldn't switch to protected mode$"
no_dpmi_msg:
    .ascii "No DPMI$"

exe_top:

exe_stack = exe_top + 512
exe_text_paras = [exe_top - exe_header + 511] / 512
exe_ram_paras = [exe_stack - exe_top + 15] / 16


#define STACK_BUFFER 128 /* number of bytes to leave for stack */

begtext:
    ! Make sure we are running under MS-DOS 2 or above.
    !
    ! While at it, also remember the actual DOS version, so that we know
    ! whether DOS gives us the program's name in the environment
    ! segment.  (DOS 3+ does; DOS 2.x does not.)
    movb ah, 0x30
    int 0x21
    cbw
    cmpb al, 2
    xchg bp, ax
    jnc ok_sys

    mov dx, bad_sys_msg
dos_msg:
    ret

ok_sys:
    ! Resize the program's memory control block (MCB) to cover only the
    ! program's near code and data space.  Use the starting sp value as
    ! a guide to how much memory we can grab.  Abort on any failure.
    !
    ! As a side effect, this also frees up any memory allocated to our
    ! program beyond 64 KiB.  (The freed memory can possibly be used by
    ! e.g. child processes, in the future.)
    !
    ! Also check that we have some space between the BSS end and the
    ! starting sp.
    cmp sp, endbss+STACK_BUFFER
    jb no_room

    movb ah, 0x4a
    mov bx, sp
    movb cl, 4
    shr bx, cl
    inc bx
    int 0x21
    jc no_room

    ! Clear BSS.
    mov di, begbss
    mov cx, endbss+1
    sub cx, di
    shr cx, 1
    xor ax, ax
    cld
    rep stosw

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
    call dos_msg
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

! vim: ts=4 sw=4 et ft=asm

