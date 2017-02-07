;
; Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.
;
; Governed by the TrueCrypt License 3.0 the full text of which is contained in
; the file License.txt included in TrueCrypt binary and source code distribution
; packages.
;

.MODEL tiny
.386
_TEXT SEGMENT USE16

INCLUDE BootDefs.i

ORG 7C00h	; Standard boot sector offset

start:
	; BIOS executes boot sector from 0:7C00 or 7C0:0000 (default CD boot loader address).
	; Far jump to the next instruction sets IP to the standard offset 7C00.
	db 0EAh				; jmp 0:main
	dw main, 0

loader_name_msg:
	db ' GostCrypt Boot Loader', 13, 10, 0
	
main:
	cli	
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov sp, 7C00h
	sti

	; Display boot loader name
	test byte ptr [start + GST_BOOT_SECTOR_USER_CONFIG_OFFSET], GST_BOOT_USER_CFG_FLAG_SILENT_MODE
	jnz skip_loader_name_msg

	lea si, loader_name_msg
	call print
skip_loader_name_msg:

	; Determine boot loader segment
	mov ax, GST_BOOT_LOADER_SEGMENT

	; Check available memory
	cmp word ptr [ds:413h], GST_BOOT_LOADER_SEGMENT / 1024 * 16 + GST_BOOT_MEMORY_REQUIRED
	jge memory_ok
	
	mov ax, GST_BOOT_LOADER_SEGMENT_LOW
	
	cmp word ptr [ds:413h], GST_BOOT_LOADER_SEGMENT_LOW / 1024 * 16 + GST_BOOT_MEMORY_REQUIRED
	jge memory_ok
	
	; Insufficient memory
	mov ax, GST_BOOT_LOADER_LOWMEM_SEGMENT

memory_ok:
	mov es, ax

	; Clear BSS section
	xor al, al
	mov di, GST_COM_EXECUTABLE_OFFSET
	mov cx, GST_BOOT_MEMORY_REQUIRED * 1024 - GST_COM_EXECUTABLE_OFFSET - 1
	cld
	rep stosb
	
	mov ax, es
	sub ax, GST_BOOT_LOADER_DECOMPRESSOR_MEMORY_SIZE / 16	; Decompressor segment
	mov es, ax
	
	; Load decompressor
	mov cl, GST_BOOT_LOADER_DECOMPRESSOR_START_SECTOR
retry_backup:
	mov al, GST_BOOT_LOADER_DECOMPRESSOR_SECTOR_COUNT
	mov bx, GST_COM_EXECUTABLE_OFFSET
	call read_sectors

	; Decompressor checksum
	xor ebx, ebx
	mov si, GST_COM_EXECUTABLE_OFFSET
	mov cx, GST_BOOT_LOADER_DECOMPRESSOR_SECTOR_COUNT * GST_LB_SIZE
	call checksum
	push ebx
	
	; Load compressed boot loader
	mov bx, GST_BOOT_LOADER_COMPRESSED_BUFFER_OFFSET
	mov cl, GST_BOOT_LOADER_START_SECTOR
	mov al, GST_MAX_BOOT_LOADER_SECTOR_COUNT
	
	test backup_loader_used, 1
	jz non_backup
	mov al, GST_BOOT_LOADER_BACKUP_SECTOR_COUNT - GST_BOOT_LOADER_DECOMPRESSOR_SECTOR_COUNT
	mov cl, GST_BOOT_LOADER_START_SECTOR + GST_BOOT_LOADER_BACKUP_SECTOR_COUNT
	
non_backup:
	call read_sectors

	; Boot loader checksum
	pop ebx
	mov si, GST_BOOT_LOADER_COMPRESSED_BUFFER_OFFSET
	mov cx, word ptr [start + GST_BOOT_SECTOR_LOADER_LENGTH_OFFSET]
	call checksum
	
	; Verify checksum
	cmp ebx, dword ptr [start + GST_BOOT_SECTOR_LOADER_CHECKSUM_OFFSET]
	je checksum_ok

	; Checksum incorrect - try using backup if available
	test backup_loader_used, 1
	jnz loader_damaged
	
	mov backup_loader_used, 1
	mov cl, GST_BOOT_LOADER_DECOMPRESSOR_START_SECTOR + GST_BOOT_LOADER_BACKUP_SECTOR_COUNT
	
	test GST_BOOT_CFG_FLAG_BACKUP_LOADER_AVAILABLE, byte ptr [start + GST_BOOT_SECTOR_CONFIG_OFFSET]
	jnz retry_backup
	
loader_damaged:
	lea si, loader_damaged_msg
	call print
	lea si, loader_name_msg
	call print
	jmp $
checksum_ok:

	; Set up decompressor segment
	mov ax, es
	mov ds, ax
	cli
	mov ss, ax
	mov sp, GST_BOOT_LOADER_DECOMPRESSOR_MEMORY_SIZE
	sti
	
	push dx
	
	; Decompress boot loader
	push GST_BOOT_LOADER_COMPRESSED_BUFFER_OFFSET + GST_GZIP_HEADER_SIZE			; Compressed data
	push GST_MAX_BOOT_LOADER_DECOMPRESSED_SIZE									; Output buffer size
	push GST_BOOT_LOADER_DECOMPRESSOR_MEMORY_SIZE + GST_COM_EXECUTABLE_OFFSET		; Output buffer

	push cs
	push decompressor_ret
	push es
	push GST_COM_EXECUTABLE_OFFSET
	retf
decompressor_ret:

	add sp, 6
	pop dx
	
	; Restore boot sector segment
	push cs
	pop ds

	; Check decompression result
	test ax, ax
	jz decompression_ok

	lea si, loader_damaged_msg
	call print
	jmp $
decompression_ok:

	; DH = boot sector flags
	mov dh, byte ptr [start + GST_BOOT_SECTOR_CONFIG_OFFSET]
	
	; Set up boot loader segment
	mov ax, es
	add ax, GST_BOOT_LOADER_DECOMPRESSOR_MEMORY_SIZE / 16
	mov es, ax
	mov ds, ax
	cli
	mov ss, ax
	mov sp, GST_BOOT_LOADER_STACK_TOP
	sti

	; Execute boot loader
	push es
	push GST_COM_EXECUTABLE_OFFSET
	retf
	
	; Print string
print:
	xor bx, bx
	mov ah, 0eh
	cld
	
@@:	lodsb
	test al, al
	jz print_end
	
	int 10h
	jmp @B

print_end:
	ret

	; Read sectors of the first cylinder
read_sectors:
	mov ch, 0           ; Cylinder
	mov dh, 0           ; Head
						; DL = drive number passed from BIOS
	mov ah, 2
	int 13h
	jnc read_ok
	
	lea si, disk_error_msg
	call print
read_ok:
	ret
	
	; Calculate checksum
checksum:
	push ds
	push es
	pop ds
	xor eax, eax
	cld
	
@@:	lodsb
	add ebx, eax
	rol ebx, 1
	loop @B
	
	pop ds
	ret

backup_loader_used		db 0
	
disk_error_msg			db 'Disk error', 13, 10, 7, 0
loader_damaged_msg		db 7, 'Loader damaged! Use Rescue Disk: Repair Options > Restore', 0

ORG 7C00h + 510
	dw 0AA55h			; Boot sector signature

_TEXT ENDS
END start
