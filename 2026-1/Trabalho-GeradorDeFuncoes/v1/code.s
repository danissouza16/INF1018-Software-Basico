/*
Comandos para gerar linguagem de máquina:
	gcc -c code.s
	objdump -d code.o

*/

.text
pushq %rbp
movq %rsp, %rbp
movq %rdi, %rax
call *%rax
movl %edi, %esi

leave 
ret 
