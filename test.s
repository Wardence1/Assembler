; Program should start at the beginning of the text segment
	; return the program
	mov eax 1; move 1 into eax
	MOV ebx 0; mov 0 into ebx
	int 0X80

label:
	; That... is a label, that's right.
