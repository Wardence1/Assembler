
; Print "Hello, World!\n"
mov edx 14
mov ecx string
mov ebx 1
mov eax 4
syscall	
	
; return the program
mov eax 1; move 1 into eax
MOV ebx 0; mov 0 into ebx
syscall

string:
	ds "Hello, World!\n"
