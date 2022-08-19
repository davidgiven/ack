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
    .data2 exe_last_page        ! number of bytes in last loadable page
    .data2 exe_text_pages       ! size of .exe, in pages
    .data2 0                    ! number of relocation entries
    .data2 0                    ! start of loadable area, in 16-byte paragraphs
    .data2 exe_ram_paras        ! required RAM size, in 16-byte paragraphs
    .data2 exe_ram_paras        ! maximum RAM siz, in 16-byte paragraphs
    .data2 0                    ! initial SS, relative to program
    .data2 stack                ! initial SP
    .data2 0                    ! checksum (ignored)
    .data2 exe_start            ! initial IP
    .data2 0                    ! initial CS, relative to program
    .data2 0                    ! offset of relocation table
    .data2 0                    ! overlay number

exe_start:
    ! On entry, DS=ES=PSP. Make DS=CS, so we're running in tiny mode.

    push cs
    pop ds
    mov (rseg), ds
    mov (pspseg), es

    ! Ensure that at least two handles are available by blindly closing some.

	mov	bx, 19                  ! close handle
	movb ah, 0x3e
	int	0x21
	mov	bx, 18                  ! close handle
	movb ah, 0x3e
	int	0x21
     
    ! Scan the environment to find the program name.

    eseg mov es, (0x2c)         ! environment pointer segment from PSP
    xor di, di
    xorb al, al
    mov cx, 0xffff
    cld
1:
    repne scasb                 ! find end of next string
    eseg cmpb (di), 0
    jnz 1b
2:
    add di, 3
    mov si, di
    repne scasb

    ! es:si now points at the program name, and es:di at one past the end.

    mov cx, di
    sub cx, si
    sub sp, cx                  ! allocate space on stack
    mov di, sp                  ! es:si source, ds:di dest
1:
    eseg movb al, (si)
    inc si
    movb (di), al
    inc di
    cmpb al, 0
    jnz 1b

    ! Open the file. (0x5f)

    mov dx, sp
    mov ax, 0x3d00
    int 0x21
    jc no_file
    mov (fh), ax
    add sp, cx                  ! don't need filename any more

    ! Get the file length (== text len).

    mov bx, ax
    mov ax, 0x4202
    xor cx, cx                  ! high offset
    xor dx, dx                  ! low offset
    int 0x21                    ! lseek
    mov (pmemlen+0), ax
    mov (pmemlen+2), dx

    ! Initialise DPMI.

    mov ax, 0x1687
    int 0x2f
    or ax, ax
    jnz no_dpmi
    mov (pmode_switch+0), di    ! write back PMODE switch routine
    mov (pmode_switch+2), es
    or si, si                   ! do we need a DPMI private area?
    jz 1f

    mov bx, si
    movb ah, 0x48
    int 0x21                    ! allocate memory from DOS
    mov es, ax                  ! data area segment -> es
1:
    ! Switch to protected mode.

    mov ax, 1                   ! 32-bit app
    callf (pmode_switch)
    jc bad_dpmi

    ! We're now in protected mode.

    mov (psegcs), cs
    mov (psegds), ds

    ! Allocate space for the code segment.

    mov ax, 0x0000
    mov cx, 1
    int 0x31                    ! allocate LDT
    jc bad_dpmi
    mov es, ax
    mov (psegcs32), ax

    mov cx, (pmemlen+0)
    mov bx, (pmemlen+2)
    mov ax, 0x0501
    int 0x31                    ! allocate linear address
    jc bad_dpmi
    mov (pmemhandle+0), di
    mov (pmemhandle+2), si

    mov dx, cx
    mov cx, bx
    mov bx, es
    mov ax, 0x0007
    int 0x31                    ! set segment base address
    jc bad_dpmi

    mov bx, es
    mov dx, (pmemlen+0)
    mov cx, (pmemlen+2)
    mov ax, 0x0008
    int 0x31                    ! set segment limit

    mov cx, cs
    and cx, 3
    shl cx, 5
    or cx, 0xc09b               ! 32-bit, big, code, non-conforming, readable
    mov bx, es
    mov ax, 0x0009
    int 0x31                    ! set descriptor access rights

    ! Allocate the data segment (as a simple clone of the code segment). (10e)

    mov ax, 0x000a
    mov bx, es
    int 0x31
    mov fs, ax
    mov (psegds32), ax

    mov cx, ax
    and cx, 3
    shl cx, 5
    or cx, 0xc093               ! 32-bit, big, data, r/w, expand-up
    mov bx, fs
    mov ax, 0x0009
    int 0x31                    ! set descriptor access rights

    ! Load the program.

    mov bx, (fh)
    mov ax, 0x4200
    xor cx, cx                  ! high offset
    mov dx, text_top            ! low offset
    int 0x21                    ! lseek

    o32 xor edi, edi            ! destination 32-bit register
1:
    movb ah, 0x3f
    mov bx, (fh)
    mov cx, TRANSFER_BUFFER_SIZE
    mov dx, transfer_buffer
    o32 push edi
    call int21                  ! read up to 32kB into transfer buffer
    o32 pop edi
    cmp ax, 0
    je 2f

    o32 movzx ecx, ax           ! number of bytes read
    o32 mov esi, transfer_buffer
    cld
    o32 rep movsb
    jmp 1b
2:

    ! Close the file.

    movb ah, 0x3e
    mov bx, (fh)
    int 0x21                    ! close

    ! Jump to the new segment and enter 32-bit mode!

    o32 movzx eax, (psegcs)
    o32 movzx ebx, (psegds)
    o32 movzx ecx, (rseg)
    o32 mov edx, realloc
    o32 mov esi, interruptf
    o32 mov edi, transfer_buffer
    push es
    pop ds
    push es
    push 0
    retf ! 19b

    ! Helper routine which reallocates the linear block that the 32-bit code
    ! is running from. This can't happen from inside the 32-bit code itself
    ! because it might move.
    !
    ! On entry, ds and ss are ignored. On exit, ds is set to the 32-bit segment.
    !  eax: new block size
realloc:
    cseg mov ds, (psegds)
    cli                         ! atomically switch stacks
    o32 mov (dpmi_ebp), esp     ! yes, saving esp into the ebp field
    mov (dpmi_ss), ss
    mov ss, (psegds)
    mov sp, dosstack
    sti
    pusha

    o32 add eax, 1024*1024 - 1
    o32 and eax, ~[1024*1024 - 1]
    o32 mov (pmemlen), eax
    mov cx, (pmemlen+0)
    mov bx, (pmemlen+2)
    mov di, (pmemhandle+0)
    mov si, (pmemhandle+2)
    mov ax, 0x0503
    int 0x31                    ! resize memory block
    jc bad_dpmi
    mov (pmemhandle+0), di
    mov (pmemhandle+2), si
    mov (pmemaddr+0), cx
    mov (pmemaddr+2), bx

    mov bx, (psegds32)
    mov dx, (pmemlen+0)
    mov cx, (pmemlen+2)
    mov ax, 0x0008
    int 0x31                    ! set ds segment limit
    jc bad_dpmi
    mov dx, (pmemaddr+0)
    mov cx, (pmemaddr+2)
    mov ax, 0x0007
    int 0x31                    ! set ds linear address
    jc bad_dpmi
    mov bx, (psegcs32)
    int 0x31                    ! set cs linear address
    jc bad_dpmi
    
    popa
    o32 mov eax, (pmemlen)
    cli                         ! atomically switch stacks back
    mov ss, (dpmi_ss)
    o32 mov esp, (dpmi_ebp)
    mov ds, (psegds32)
    sti

    o32 retf

bad_dpmi:
    mov si, bad_dpmi_msg
    jmp exit_with_error

no_file:
    mov si, no_file_msg
    jmp exit_with_error

no_dpmi:
    mov si, no_dpmi_msg
    ! fall through

! Displays the message in si and exits.
! This uses a loop because from protected mode any interrupt which
! takes a pointer parameter requires special treatment.
exit_with_error:
    movb dl, (si)
    cmpb dl, 0
    je 1f
    inc si

    movb ah, 2
    int 0x21                    ! print character
    jmp exit_with_error
1:
    mov ax, 0x4cff
    int 0x21                    ! terminate with error code al

    ! Simulate DOS interrupt.
int21:
    o32 or ebx, 0x210000
    ! Simulate interrupt in the high half of ebx.
interrupt:
    mov (dpmi_eax), ax
    mov (dpmi_ebx), bx
    mov (dpmi_ecx), cx
    mov (dpmi_edx), dx
    mov (dpmi_esi), si
    mov (dpmi_edi), di
    mov ax, (rseg)
    mov (dpmi_ds), ax
    mov (dpmi_ss), ax
    push es
    mov (dpmi_sp), dosstack     ! auto stack is too small
    push ds
    pop es
    o32 mov edi, dpmi_edi
    mov ax, 0x300
    o32 shr ebx, 16
    int 0x31                    ! simulate DOS interrupt
    pop es
    o32 movzx eax, (dpmi_eax)
    o32 movzx ebx, (dpmi_ebx)
    o32 movzx ecx, (dpmi_ecx)
    o32 movzx edx, (dpmi_edx)
    o32 movzx esi, (dpmi_esi)
    o32 movzx edi, (dpmi_edi)
    push (dpmi_flags)
    popf
    ret

! Far call wrapper around interrupt.
interruptf:
    push ds
    cseg mov ds, (psegds)
    call interrupt
    pop ds
    o32 retf

bad_dpmi_msg:
    .asciz "DPMI error during setup"
no_file_msg:
    .asciz "Couldn't open .exe"
no_dpmi_msg:
    .asciz "No DPMI host installed"
.align 4
text_top:

exe_text_pages = [text_top - exe_header + 511] / 512
exe_last_page = [text_top - exe_header] % 512

.sect .rom

.sect .bss
bss_start:

dpmi_edi:       .space 4
dpmi_esi:       .space 4
dpmi_ebp:       .space 4
                .space 4 ! reserved
dpmi_ebx:       .space 4
dpmi_edx:       .space 4
dpmi_ecx:       .space 4
dpmi_eax:       .space 4
dpmi_flags:     .space 2
dpmi_es:        .space 2
dpmi_ds:        .space 2
dpmi_fs:        .space 2
dpmi_gs:        .space 2
dpmi_ip:        .space 2
dpmi_cs:        .space 2
dpmi_sp:        .space 2
dpmi_ss:        .space 2

pmode_switch:   .space 4
rseg:           .space 2 ! real mode
pspseg:         .space 2 ! real mode
psegcs:         .space 2 ! protected mode 16-bit code segment
psegds:         .space 2 ! protected mode 16-bit data segment
psegcs32:       .space 2 ! protected mode 32-bit code segment
psegds32:       .space 2 ! protected mode 32-bit data segment
pmemaddr:       .space 4 ! protected mode linear memory address
pmemhandle:     .space 4 ! protected mode linear memory handle
pmemlen:        .space 4 ! protected mode linear memory length
fh:             .space 2

    .space 512
stack:
    .space 512
dosstack:

TRANSFER_BUFFER_SIZE = 32*1024
transfer_buffer:
    .space TRANSFER_BUFFER_SIZE

bss_top:
exe_ram_paras = [bss_top - bss_start + 15] / 16

! vim: ts=4 sw=4 et ft=asm

